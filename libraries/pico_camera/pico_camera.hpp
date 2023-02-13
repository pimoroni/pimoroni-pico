#pragma once

#include "common/pimoroni_i2c.hpp"
#include "drivers/aps6404/aps6404.hpp"
#include "drivers/ov2640/ov2640.hpp"

namespace pimoroni {

    class PicoCamera {
    public:
        static const uint8_t ACT_LED = 6;
        static const uint8_t SW_A = 7;
        static const uint8_t EXT_LED = 2;
        static const uint8_t EXT_INT = 3;

        using ImageSize = OV2640::ImageSize;
        using ImageMode = OV2640::ImageMode;

        PicoCamera() : ov2640(new I2C(CAMERA_PACK, 100000)) {}
        PicoCamera(I2C* i2c) : ov2640(i2c) {}

        void init(uint32_t* buffer=nullptr, uint32_t buffer_len=0);

        void memory_test();

        // Set the size of image to be captured
        void set_image_size(ImageSize size) { ov2640.set_image_size(size); }
        ImageSize get_image_size() const { return ov2640.get_image_size(); }

        // Set the mode of the image
        void set_image_mode(ImageMode mode) { ov2640.set_image_mode(mode); }
        ImageMode get_image_mode() const { return ov2640.get_image_mode(); }

        // Return the length of a captured image at the current size
        uint32_t get_image_len_in_bytes() const { return ov2640.get_image_len_in_bytes(); }

        // Capture an image.  The number of available slots varies by image size.
        void capture_image(int slot);

        // Return the number of image slots at this image size.
        int count_image_slots();

        // Read part of a previously captured image.
        // The address is the byte offset through the image to start reading at.
        // Read length is specified in bytes but must be a multiple of 4.
        // The supplied buffer must be 4 byte aligned, which is why it is a uint32_t pointer.
        void read_data(int slot, uint32_t address, uint32_t len_in_bytes, uint32_t* buffer);

    private:
        APS6404 aps6404;
        OV2640 ov2640;

        static constexpr int NUM_BUFFERS = 4;
        uint32_t* buffers[NUM_BUFFERS];
        uint32_t buffer_len_in_words;
        uint32_t transfer_addr;

        uint32_t get_address_for_slot(int slot) const;
    };
}
