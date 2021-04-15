#include "hardware/spi.h"
#include "hardware/sync.h"
#include "pico/binary_info.h"

#include "../../../libraries/pico_scroll/pico_scroll.hpp"

using namespace pimoroni;

PicoScroll *scroll = nullptr;


extern "C" {
#include "pico_scroll.h"
#include "pico_scroll_font.h"

#define NOT_INITIALISED_MSG     "Cannot call this function, as picoscroll is not initialised. Call picoscroll.init() first."
#define BUFFER_TOO_SMALL_MSG "bytearray too small: len(image) < width * height."
#define INCORRECT_SIZE_MSG "Scroll height wrong: > 8 pixels."

mp_obj_t picoscroll_init() {
    if(scroll == nullptr)
        scroll = new PicoScroll();
    scroll->init();
    return mp_const_none;
}

mp_obj_t picoscroll_get_width() {
    return mp_obj_new_int(PicoScroll::WIDTH);
}

mp_obj_t picoscroll_get_height() {
    return mp_obj_new_int(PicoScroll::HEIGHT);
}

mp_obj_t picoscroll_update() {
    if(scroll != nullptr)
        scroll->update();
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    return mp_const_none;
}

mp_obj_t picoscroll_set_pixel(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t v_obj) {
    if(scroll != nullptr) {
        int x = mp_obj_get_int(x_obj);
        int y = mp_obj_get_int(y_obj);
        int val = mp_obj_get_int(v_obj);

        if(x < 0 || x >= PicoScroll::WIDTH || y < 0 || y >= PicoScroll::HEIGHT)
            mp_raise_ValueError("x or y out of range.");
        else {
            if(val < 0 || val > 255)
                mp_raise_ValueError("val out of range. Expected 0 to 255");
            else
                scroll->set_pixel(x, y, val);
        }
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}


  mp_obj_t picoscroll_show_text(mp_obj_t text_obj, mp_obj_t brightness_obj, mp_obj_t offset_obj) {
    if(scroll != nullptr) {
        mp_buffer_info_t bufinfo;
	unsigned char * buffer;
	int text_len, bfr_len;
	
	mp_get_buffer_raise(text_obj, &bufinfo, MP_BUFFER_RW);
	unsigned char * values = (unsigned char *) bufinfo.buf;
        int brightness = mp_obj_get_int(brightness_obj);
        int offset = mp_obj_get_int(offset_obj);
	
	text_len = bufinfo.len;
	bfr_len = 6 * text_len;
	
	int width = PicoScroll::WIDTH;
	int height = PicoScroll::HEIGHT;

	// clear the scroll, so only need to write visible bytes
	scroll->clear();

	if ((offset < -width) || (offset > bfr_len)) {
	    return mp_const_none;
	}

	// allocate buffer, render text, free buffer #TODO probably can do
	// without the buffer here
	
	buffer = (unsigned char *) m_malloc (sizeof(unsigned char) * bfr_len);
	render(values, text_len, buffer, bfr_len);
	
	for (int x = 0; x < width; x++) {
	    int k = offset + x;
	    if ((k >= 0) && (k < bfr_len)) {
	        unsigned char col = buffer[k];
		for (int y = 0; y < height; y++) {
		    int val = brightness * ((col >> y) & 1);
		    scroll->set_pixel(x, y, val);
		}
	    }
	}
	m_free(buffer);
    } else {
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    }

    return mp_const_none;
}
	
  
  
mp_obj_t picoscroll_set_pixels(mp_obj_t image_obj) {
    if(scroll != nullptr) {
        mp_buffer_info_t bufinfo;
	mp_get_buffer_raise(image_obj, &bufinfo, MP_BUFFER_RW);

	if (bufinfo.len < (PicoScroll::WIDTH * PicoScroll::HEIGHT)) {
	    mp_raise_msg(&mp_type_IndexError, BUFFER_TOO_SMALL_MSG);
	}

	unsigned char * values = (unsigned char *) bufinfo.buf;

        for (int y = 0; y < PicoScroll::HEIGHT; y++) {
	    for (int x = 0; x < PicoScroll::WIDTH; x++) {
	        int val = values[y * PicoScroll::WIDTH + x];
                scroll->set_pixel(x, y, val);
	    }
        }
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

mp_obj_t picoscroll_show_bitmap_1d(mp_obj_t bitmap_obj, mp_obj_t brightness_obj, mp_obj_t offset_obj) {
    if(scroll != nullptr) {
        mp_buffer_info_t bufinfo;
	mp_get_buffer_raise(bitmap_obj, &bufinfo, MP_BUFFER_RW);
        int offset = mp_obj_get_int(offset_obj);
        int brightness = mp_obj_get_int(brightness_obj);
	int length = bufinfo.len;
	int width = PicoScroll::WIDTH;
	int height = PicoScroll::HEIGHT;

	// this obviously shouldn't happen as the scroll is 17x7 pixels
	// would fall off end of byte if this the case
	if (height > 8) {
	    mp_raise_msg(&mp_type_RuntimeError, INCORRECT_SIZE_MSG);
	}

	unsigned char * values = (unsigned char *) bufinfo.buf;

	// clear the scroll, so only need to write visible bytes
	scroll->clear();

	if ((offset < -width) || (offset > length)) {
	    return mp_const_none;
	}

	for (int x = 0; x < width; x++) {
	    int k = offset + x;
	    if ((k >= 0) && (k < length)) {
	        unsigned char col = values[k];
		for (int y = 0; y < height; y++) {
		    int val = brightness * ((col >> y) & 1);
		    scroll->set_pixel(x, y, val);
		}
	    }
	}
    } else {
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    }

    return mp_const_none;
}

mp_obj_t picoscroll_clear() {
    if(scroll != nullptr)
        scroll->clear();
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    return mp_const_none;
}

mp_obj_t picoscroll_is_pressed(mp_obj_t button_obj) {
    bool buttonPressed = false;
    
    if(scroll != nullptr) {
        int buttonID = mp_obj_get_int(button_obj);    
        switch(buttonID) {
        case 0:
            buttonPressed = scroll->is_pressed(PicoScroll::A);
            break;

        case 1:
            buttonPressed = scroll->is_pressed(PicoScroll::B);
            break;

        case 2:
            buttonPressed = scroll->is_pressed(PicoScroll::X);
            break;

        case 3:
            buttonPressed = scroll->is_pressed(PicoScroll::Y);
            break;

        default:
            mp_raise_ValueError("button not valid. Expected 0 to 3");
            break;
        }
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return buttonPressed ? mp_const_true : mp_const_false;
}
}
