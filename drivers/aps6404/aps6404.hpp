#pragma once

#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/spi.h"

#ifndef __APS6404_H__ 

#define __APS6404_H__

namespace pimoroni{

	struct aps6404_config {
		uint pin_csn;  // CSn, SCK
		uint pin_mosi;
		uint pin_miso;
		uint pin_sck;
		spi_inst_t *spi;
		uint spi_baud;

	};

	void aps6404_init(struct aps6404_config* config);

	// Start a write, this completes asynchronously, this function only blocks if another 
	// transfer is already in progress
	void aps6404_write(struct aps6404_config* config, uint32_t addr, uint8_t* data, size_t len);

	// Start a read, this completes asynchronously, you can check / wait for completion
	// by checking the DMA channel.
	void aps6404_read(struct aps6404_config* config, uint32_t addr, uint8_t* read_buf, size_t len);

}
#endif