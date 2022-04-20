/*******************************************************************************
* Copyright (c) 2020, STMicroelectronics - All Rights Reserved
*
* This file is part of the VL53L5CX Ultra Lite Driver and is dual licensed,
* either 'STMicroelectronics Proprietary license'
* or 'BSD 3-clause "New" or "Revised" License' , at your option.
*
********************************************************************************
*
* 'STMicroelectronics Proprietary license'
*
********************************************************************************
*
* License terms: STMicroelectronics Proprietary in accordance with licensing
* terms at www.st.com/sla0081
*
* STMicroelectronics confidential
* Reproduction and Communication of this document is strictly prohibited unless
* specifically authorized in writing by STMicroelectronics.
*
*
********************************************************************************
*
* Alternatively, the VL53L5CX Ultra Lite Driver may be distributed under the
* terms of 'BSD 3-clause "New" or "Revised" License', in which case the
* following provisions apply instead of the ones mentioned above :
*
********************************************************************************
*
* License terms: BSD 3-clause "New" or "Revised" License.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its contributors
* may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*
*******************************************************************************/


#include "platform.h"
#include "pico/stdlib.h"

uint8_t RdByte(
		VL53L5CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_value)
{
	const uint8_t buf[2] = {
		RegisterAdress >> 8,
		RegisterAdress & 0xff
	};
	i2c_write_blocking(p_platform->i2c, p_platform->address, buf, sizeof(buf), true);
	if(i2c_read_blocking(p_platform->i2c, p_platform->address, p_value, 1, false) != PICO_ERROR_GENERIC){
		return 0;
	}

	return 255;
}

uint8_t WrByte(
		VL53L5CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t value)
{
	const uint8_t buf[3] = {
		RegisterAdress >> 8,
		RegisterAdress & 0xff,
		value,
	};
	if(i2c_write_blocking(p_platform->i2c, p_platform->address, buf, sizeof(buf), false) != PICO_ERROR_GENERIC) {
		return 0;
	}

	return 255;
}

uint8_t WrMulti(
		VL53L5CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_values,
		uint32_t size)
{
	uint8_t buf[2];
	buf[0] = RegisterAdress >> 8;
	buf[1] = RegisterAdress & 0xff;

	// Send the 16-bit address with no STOP condition
	int result = i2c_write_blocking(p_platform->i2c, p_platform->address, buf, sizeof(buf), true);

	// Handle an error early... it gets dicey from here
	if(result == PICO_ERROR_GENERIC) return 255;

	// The VL53L5CX does not support "Repeated Start" and the Pico's I2C API doesn't
	// let us send more bytes without sending another start condition.

	// The horrow below lets us send out "p_values" followed by a STOP condition,
	// without having to copy everything into a temporary buffer.
	uint8_t *src = p_values;

	// Send the rest of the data, followed by a STOP condition
	// This re-implements the relevant portion of i2c_write_blocking_internal which is NOT sent a timeout check function by i2c_write_blocking
    for (int byte_ctr = 0; byte_ctr < size; ++byte_ctr) {
        bool last = byte_ctr == size - 1;
        p_platform->i2c->hw->data_cmd =
                bool_to_bit(last) << I2C_IC_DATA_CMD_STOP_LSB | *src++;

        // Wait until the transmission of the address/data from the internal
        // shift register has completed. For this to function correctly, the
        // TX_EMPTY_CTRL flag in IC_CON must be set. The TX_EMPTY_CTRL flag
        // was set in i2c_init.
        do {
            tight_loop_contents();
        } while (!(p_platform->i2c->hw->raw_intr_stat & I2C_IC_RAW_INTR_STAT_TX_EMPTY_BITS));

		if (p_platform->i2c->hw->tx_abrt_source) {
			// Note clearing the abort flag also clears the reason, and
			// this instance of flag is clear-on-read! Note also the
			// IC_CLR_TX_ABRT register always reads as 0.
			p_platform->i2c->hw->clr_tx_abrt;

			// An abort on the LAST byte means things are probably fine
			if(last) {
                // TODO Could there be an abort while waiting for the STOP
                // condition here? If so, additional code would be needed here
                // to take care of the abort.
                do {
                    tight_loop_contents();
                } while (!(p_platform->i2c->hw->raw_intr_stat & I2C_IC_RAW_INTR_STAT_STOP_DET_BITS));
			} else {
				// Ooof, unhandled abort. Fail?
				return 255;
			}
		}
	}

	// Not sure it matters where we clear this, but by default a "nostop" style write
	// will set this flag so the next transaction starts with a "Repeated Start."
	p_platform->i2c->restart_on_next = false;

	return 0;
}

uint8_t RdMulti(
		VL53L5CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_values,
		uint32_t size)
{
	const uint8_t buf[2] = {
		RegisterAdress >> 8,
		RegisterAdress & 0xff
	};
	i2c_write_blocking(p_platform->i2c, p_platform->address, buf, sizeof(buf), true);
	if(i2c_read_blocking(p_platform->i2c, p_platform->address, p_values, size, false) != PICO_ERROR_GENERIC){
		return 0;
	}

	return 255;
}

uint8_t Reset_Sensor(
		VL53L5CX_Platform *p_platform)
{
	uint8_t status = 0;
	
	/* (Optional) Need to be implemented by customer. This function returns 0 if OK */
	
	/* Set pin LPN to LOW */
	/* Set pin AVDD to LOW */
	/* Set pin VDDIO  to LOW */
	WaitMs(p_platform, 100);

	/* Set pin LPN of to HIGH */
	/* Set pin AVDD of to HIGH */
	/* Set pin VDDIO of  to HIGH */
	WaitMs(p_platform, 100);

	return status;
}

void SwapBuffer(
		uint8_t 		*buffer,
		uint16_t 	 	 size)
{
	uint32_t i, tmp;
	
	/*for(auto i = 0u; i < size / 4u; i++) {
		uint32_t *dword = &((uint32_t *)buffer)[i];
		*dword = __builtin_bswap32(*dword);
	}*/

	/* Example of possible implementation using <string.h> */
	for(i = 0; i < size; i = i + 4) 
	{
		tmp = (
		  buffer[i]<<24)
		|(buffer[i+1]<<16)
		|(buffer[i+2]<<8)
		|(buffer[i+3]);
		
		memcpy(&(buffer[i]), &tmp, 4);
	}
}

uint8_t WaitMs(
		VL53L5CX_Platform *p_platform,
		uint32_t TimeMs)
{
	sleep_ms(TimeMs);
	return 0;
}
