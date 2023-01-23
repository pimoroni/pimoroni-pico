#include "aps6404.hpp"
#include "hardware/dma.h"
#include "hardware/spi.h"

namespace pimoroni{
	
	uint8_t APS6404_READ = 0x03; //slow read 33Mhz max
	uint8_t APS6404_WRITE = 0x02;
	uint8_t AP6404_RESET_ENABLE = 0x66;
	uint8_t AP6404_RESET = 0x99;


	void long_address_to_8bit_array_MSB(uint32_t long_address, uint8_t * uint8_address_buffer){

		//Swaps the byte order to make sure that bits 24-31 write out first
		* uint8_address_buffer = (uint8_t) (long_address >> 24) & 0xff; // this byte gets dropped in the tranfser
		uint8_address_buffer++;
		* uint8_address_buffer = (uint8_t) (long_address >> 16) & 0xff;
		uint8_address_buffer++;
		* uint8_address_buffer = (uint8_t) (long_address >> 8) & 0xff;
		uint8_address_buffer++;
		* uint8_address_buffer = (uint8_t) long_address  & 0xff;
	} 

	void aps6404_init(struct aps6404_config* config) {
		spi_init(config->spi, config->spi_baud);

		gpio_set_function(config->pin_csn, GPIO_FUNC_SIO);
		gpio_set_dir(config->pin_csn, GPIO_OUT);

		gpio_set_function(config->pin_sck, GPIO_FUNC_SPI);
		gpio_set_function(config->pin_mosi, GPIO_FUNC_SPI);
		gpio_set_function(config->pin_miso, GPIO_FUNC_SPI);

		gpio_put(config->pin_csn, 0);
		spi_write_blocking(config->spi, &AP6404_RESET_ENABLE, 1);
		gpio_put(config->pin_csn, 1);
		sleep_us(100);
		gpio_put(config->pin_csn, 0);
		spi_write_blocking(config->spi, &AP6404_RESET, 1);
		gpio_put(config->pin_csn, 1);
		sleep_us(100);
	}


	void aps6404_write(struct aps6404_config* config, uint32_t addr, uint8_t* data, size_t len) {
		uint8_t address_arr[4];
		long_address_to_8bit_array_MSB(addr, &address_arr[0]);
		gpio_put(config->pin_csn, 0);
		spi_write_blocking(config->spi, &APS6404_WRITE, 1);
		spi_write_blocking(config->spi, &address_arr[1], 3);
		spi_write_blocking(config->spi, data, len);
		gpio_put(config->pin_csn, 1);
	}

	void aps6404_read(struct aps6404_config* config, uint32_t addr, uint8_t* read_buf, size_t len) {
		uint8_t address_arr[4];
		long_address_to_8bit_array_MSB(addr, &address_arr[0]);
		gpio_put(config->pin_csn, 0);
		spi_write_blocking(config->spi, &APS6404_READ, 1);
		spi_write_blocking(config->spi, &address_arr[1], 3);
		spi_read_blocking(config->spi, 0x00, read_buf, len);
		gpio_put(config->pin_csn, 1);

	}
}
