#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/spi.h"
#include "drivers/aps6404/aps6404.hpp"

using namespace pimoroni;

const uint8_t CMD_REG_WRITE = 0xAA;
const uint8_t CMD_REG_READ = 0xBB;
const uint8_t CMD_CAPTURE = 0xCC;


struct aps6404_config ram_config;


int main() {
	// The PSRAM supports up to 84MHz, so 168MHz is the fastest we can go
	set_sys_clock_khz(168000, true);
	stdio_init_all();




	ram_config.pin_csn = 17;
	ram_config.pin_mosi = 19;
	ram_config.pin_miso = 16;
	ram_config.pin_sck = 18;
	ram_config.spi = spi0;
	ram_config.spi_baud = 100000;

	aps6404_init(&ram_config);
	while (true){
		uint8_t data[16];
		for (int i = 0; i < 16; ++i) {
			data[i] = 0x12 + i;
		}
		aps6404_write(&ram_config, 0, &data[0], 16);

		for (int i = 0; i < 16; ++i) {
			uint8_t read_data = 1;
			aps6404_read(&ram_config, i , &read_data, 1);
			if (read_data != data[i]) {
				printf("RAM test failed: Wrote %x, read back %x\n", data[i], read_data);
			}
		}	
		printf("cycle complete\n");
		sleep_ms(1000);
	}
}



