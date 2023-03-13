#include <cstdio>
#include <algorithm>
#include "pico/stdlib.h"
#include "hardware/sync.h"
#include "pico_camera.hpp"
#include "drivers/fatfs/ff.h"

namespace pimoroni {
    void PicoCamera::init(uint32_t* buffer, uint32_t buffer_len) {
        // Ensure SD card is deselected
        gpio_init(SDCARD_CS);
        gpio_put(SDCARD_CS, 1);
        gpio_set_dir(SDCARD_CS, GPIO_OUT);

        ov2640.init(ImageSize::SIZE_1600x1200, ImageMode::MODE_RGB565);
        aps6404.init();

        if (buffer_len > 0) {
            buffer_len_in_words = std::min(buffer_len / (NUM_BUFFERS * 4), (uint32_t)APS6404::PAGE_SIZE >> 2);
        }
        else {
            buffer_len_in_words = APS6404::PAGE_SIZE >> 2;
        }
        for (int i = 0; i < NUM_BUFFERS; ++i) {
            if (buffer) {
                buffers[i] = buffer + (i * buffer_len_in_words);
            }
            else {
                buffers[i] = (uint32_t*)malloc(buffer_len_in_words << 2);
            }
        }
    }

    void PicoCamera::memory_test() {
        uint32_t data[16];
        for (uint32_t i = 0; i < 16; ++i) {
            data[i] = 0x12345670u + i * 0x10101010u;
        }
        aps6404.write(0, data, 16);

        for (int j = 0; j < 10; ++j)
        for (int i = 0; i < 16; ++i) {
            uint32_t read_data = 1;
            aps6404.read_blocking(i * 4, &read_data, 1);
            if (read_data != data[i]) {
                printf("RAM test failed: Wrote %lx, read back %lx\n", data[i], read_data);
            }
            else {
                printf("RAM test OK: Wrote %lx, read back %lx\n", data[i], read_data);
            }
        }
        printf("RAM test complete\n");
    }

    void PicoCamera::capture_image(int slot) {
        transfer_addr = get_address_for_slot(slot);

        ov2640.start_capture(buffers, NUM_BUFFERS, buffer_len_in_words, [this](uint32_t* buffer) {
            aps6404.enqueue_write(transfer_addr, buffer, buffer_len_in_words);
            transfer_addr += APS6404::PAGE_SIZE;
        });

        while (ov2640.is_capture_in_progress()) {
            __wfi();
        }
    }

    int PicoCamera::count_image_slots() {
        const uint32_t image_len_in_pages = (get_image_len_in_bytes() + APS6404::PAGE_SIZE - 1) / APS6404::PAGE_SIZE;
        return APS6404::RAM_SIZE / (image_len_in_pages * APS6404::PAGE_SIZE);
    }

    void PicoCamera::read_data(int slot, uint32_t address, uint32_t len_in_bytes, uint32_t* buffer) {
        const uint32_t addr = get_address_for_slot(slot) + address;

        aps6404.read_blocking(addr, buffer, len_in_bytes >> 2);
    }

    uint32_t PicoCamera::get_address_for_slot(int slot) const {
        const uint32_t image_len_in_pages = (get_image_len_in_bytes() + APS6404::PAGE_SIZE - 1) / APS6404::PAGE_SIZE;
        return slot * image_len_in_pages * APS6404::PAGE_SIZE;
    }

    void PicoCamera::cs_enable(bool psram, bool sdcard){
        if (psram && sdcard){   // Make sure onlt one is selected at a time
            return;
        }
        if (psram){
            gpio_put(SDCARD_CS, 1);
            sleep_us(100);
            gpio_put(PSRAM_CS, 0);
        }
        if (sdcard){
            gpio_put(PSRAM_CS, 1);
            sleep_us(100);
            gpio_put(SDCARD_CS, 0);
        }
    }
    void PicoCamera::enable_psram(){
        gpio_set_function(18, GPIO_FUNC_PIO0);
        gpio_set_function(19, GPIO_FUNC_PIO0);
        gpio_set_function(20, GPIO_FUNC_PIO0);

    }

    void PicoCamera::enable_sdcard(){
        gpio_set_function(18, GPIO_FUNC_SPI);
        gpio_set_function(19, GPIO_FUNC_SPI);
        gpio_set_function(20, GPIO_FUNC_SPI);

    }

        void PicoCamera::mount_sdcard(){
            enable_sdcard();

            printf("mounting sd card.. \n");

            printf(" cs pin %d ", SDCARD_PIN_SPI0_CS);

            file_result = f_mount(file_system_p, nullptr, 1);
            if (file_result != FR_OK) {
                printf("Failed to mount SD card, error: %d\n", file_result);
                sdcard_mounted = true;
            }
            else
            {
                sd_hardware_initialised = true;
                sdcard_mounted = true;
                printf("done!\n");
            }
            enable_psram();
        }

        void PicoCamera::print_directory_listing(const char* path){
            if (sd_hardware_initialised && sdcard_mounted){
            f_opendir(dir_p, path);

            while(f_readdir(dir_p, &file_info) == FR_OK && file_info.fname[0]) {
                printf("- %s %lld\n", file_info.fname, file_info.fsize);
            }
            f_closedir(dir_p);
            }
            else{
                printf("Please mount sd card");
            }
        }
        void PicoCamera::open_file(const char* path, bool append=false){
            
        }
        void PicoCamera::close_file(){

        }
        void PicoCamera::create_file(){

        }
        void PicoCamera::write_buffer_to_file(const void* buffer, uint buffer_size){

        }
        void PicoCamera::read_file_to_buffer(const void* buffer, uint bytes_to_read){

        }
        void PicoCamera::print_sdcard_status(){

        }
}
