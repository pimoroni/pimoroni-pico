#include "xpt2046.hpp"

#include <cstdlib>
#include <math.h>

namespace pimoroni {
	template <typename T> T Clamp(const T& value, const T& low, const T& high) 
	{
  	return value < low ? low : (value > high ? high : value); 
	}

	enum reg {
		TOUCH_SLEEP	 = 0x00,
		TOUCH_READ_Y = 0x90,
		TOUCH_READ_Z1 = 0xB0,
		TOUCH_READ_Z2 = 0xC0,
		TOUCH_READ_X = 0xD0
	};

	uint16_t XPT2046::transfer_16(uint8_t cmd) {
		spi_write_blocking(spi, &cmd, 1);
		uint8_t buffer[2] = {0, 0};
		spi_read_blocking(spi, 0, buffer, 2);
		return (((uint16_t)buffer[0]) << 8) | ((uint16_t)buffer[1]);
	}

	void XPT2046::update(uint16_t average_samples) {
		touch_down = !gpio_get(irq);

		if(touch_down)
		{
			gpio_put(cs, 0);

			int32_t raw_x = 0;
			int32_t raw_y = 0;
			int32_t raw_z1 = 0;
			int32_t raw_z2 = 0;

			for(uint16_t u = 0 ; u < average_samples; u++)
			{
				raw_x += transfer_16(reg::TOUCH_READ_X)>>3;
				raw_y += transfer_16(reg::TOUCH_READ_Y)>>3;
				raw_z1 += transfer_16(reg::TOUCH_READ_Z1)>>3;
				raw_z2 += transfer_16(reg::TOUCH_READ_Z2)>>3;
			}

			raw_x /= average_samples;
			raw_y /= average_samples;
			raw_z1 /= average_samples;
			raw_z2 /= average_samples;
			
			// Deselect
			gpio_put(cs, 0);

			float raw_z = (((((float) raw_z2) / raw_z1) -1) * raw_x);

			printf("raw = %lu, %lu, %lu, %lu = %f, %f\n", raw_x, raw_y, raw_z1, raw_z2, raw_z, raw_z);
			// recheck irq
			touch_down = !gpio_get(irq);

			if(touch_down) {
				// set our raw touch point
				raw_touch = {raw_x, raw_y, (int32_t)raw_z};
				
				// clamp raw to calibration data
				raw_x = Clamp<uint16_t>(raw_x, raw_min.x, raw_max.x);
				raw_y = Clamp<uint16_t>(raw_y, raw_min.y, raw_max.y);

				// calc Z from calibration data
				// if we are not at mid point of calibration set touch off.
				int32_t z = -1;
				if(z_enabled) {
					uint32_t mid_z = raw_min.z + (((raw_max.z - raw_min.z)/10)*5);
					if(raw_z > mid_z) {
						touch_down = false;
					}
					else {
						raw_z = Clamp<uint16_t>(raw_z, raw_min.z, mid_z);
						z = 100-(((raw_z - raw_min.z) * 100) / (mid_z - raw_min.z));
					}
				}
				
				if(touch_down) {
					// convert to screen (from calibration data)
					int32_t y, x, t;

					// calculate for no rotation, Y is inverted
					x = (raw_x - raw_min.x) * width / (raw_max.x - raw_min.x);
					y = height - ((raw_y - raw_min.y) * height / (raw_max.y - raw_min.y));

					// clamp x/y
					x = Clamp<int16_t>(x, 0, width);
					y = Clamp<int16_t>(y, 0, height);

					// rotate x and y if needed
					switch(rotation)
					{
						case ROTATE_0:
						break;
						
						case ROTATE_90:
							t = x;
							x = y;
							y = width - t;
						break;
						
						case ROTATE_180:
							x = width - x;
							y = height - y;
						break;
						
						case ROTATE_270:
							t = x;
							x = height - y;
							y = t;
						break;
					}

					// set our screen touch
					touch = {x, y, z};
				}
			}
		}
	}

	void XPT2046::cleanup() {
	}

}
