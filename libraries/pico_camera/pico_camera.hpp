#pragma once

#include "common/pimoroni_i2c.hpp"
#include "drivers/aps6404/aps6404.hpp"
#include "drivers/ov2640/ov2640.hpp"
#include "drivers/fatfs/ff.h"


namespace pimoroni {



    class PicoCamera {
    public:
        static const uint8_t ACT_LED = 6;
        static const uint8_t SW_A = 7;
        static const uint8_t EXT_LED = 2;
        static const uint8_t EXT_INT = 3;
        static const uint8_t PSRAM_CS = 17;
        static const uint8_t SDCARD_CS = 26;

        using ImageSize = OV2640::ImageSize;
        using ImageMode = OV2640::ImageMode;

        PicoCamera() : ov2640(new I2C(CAMERA_PACK, 100000)) {}
        PicoCamera(I2C* i2c) : ov2640(i2c) {}

        void init(uint32_t* buffer=nullptr, uint32_t buffer_len=0);

        void memory_test();
        void enable_psram();
        void enable_sdcard();
        // sdcard functions
        void mount_sdcard();
        void print_directory_listing(const char* path);
        void open_file(const char* path, bool append);
        void close_file();
        void create_file();
        void write_buffer_to_file(const void* buffer, uint buffer_size);
        void read_file_to_buffer(const void* buffer, uint bytes_to_read);
        void print_sdcard_status();

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

        // Data types for handling the file system
        FATFS *file_system_p; // Pointer to Selected file system
        FRESULT file_result; // Error handling
        FILINFO file_info; // Datatype for information about a file
        FIL *file_p; // File pointer
        DIR *dir_p; // Directory pointer

        // Specific States for sdcard and files
        uint8_t sd_hardware_initialised = 0;
        uint8_t sdcard_mounted = 0;
        uint16_t bytes_written_to_sdcard = 0;
        uint16_t bytes_remaining_to_write_to_sdcard = 0;

        static constexpr int NUM_BUFFERS = 4;
        uint32_t* buffers[NUM_BUFFERS];
        uint32_t buffer_len_in_words;
        uint32_t transfer_addr;

        void cs_enable(bool psram, bool sdcard);    // swaping between sdcard and psram
        uint32_t get_address_for_slot(int slot) const;
    };
}
