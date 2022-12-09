#include "xpt2046.hpp"

#include <cstdlib>
#include <math.h>

namespace pimoroni {
	template <typename T> T Clamp(const T& value, const T& low, const T& high) 
	{
  	return value < low ? low : (value > high ? high : value); 
	}

	enum reg {
		TOUCH_READ_X = 0x90,
		TOUCH_READ_Y = 0xD0
	};

	void XPT2046::update(uint16_t average_samples) {
    static const uint8_t cmd_read_x = reg::TOUCH_READ_X;
	  static const uint8_t cmd_read_y = reg::TOUCH_READ_Y;
		
		touch_down = !gpio_get(irq);

		if(touch_down)
		{
			gpio_put(cs, 0);

			uint32_t raw_x = 0;
			uint32_t raw_y = 0;

			for(uint16_t u = 0 ; u < average_samples; u++)
			{
				spi_write_blocking(spi, &cmd_read_x, 1);
				uint8_t x_buffer[2] = {0, 0};
				spi_read_blocking(spi, 0, x_buffer, 2);

				spi_write_blocking(spi, &cmd_read_y, 1);
				uint8_t y_buffer [2] = {0, 0};
				spi_read_blocking(spi, 0, y_buffer, 2);


				raw_x += (((uint16_t)x_buffer[0]) << 8) | ((uint16_t)x_buffer[1]);
				raw_y += (((uint16_t)y_buffer[0]) << 8) | ((uint16_t)y_buffer[1]);
			}

			raw_x /= average_samples;
			raw_y /= average_samples;
			
			// Deselect
			gpio_put(cs, 0);

			// recheck irq
			touch_down = !gpio_get(irq);

			if(touch_down) {
				// set our raw touch point
				raw_touch = {(int32_t)raw_x, (int32_t)raw_y};
				
				// clamp raw to calibration data
				raw_x = Clamp<uint16_t>(raw_x, raw_min.x, raw_max.x);
				raw_y = Clamp<uint16_t>(raw_y, raw_min.y, raw_max.y);

				// convert to screen (from calibration data)
				int16_t y, x, t;

				// calculate for no rotation
				x = (raw_x - raw_min.x) * width / (raw_max.x - raw_min.x);
				y = (raw_y - raw_min.y) * height / (raw_max.y - raw_min.y);

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
				touch = {x, y};
			}
		}
	}

	void XPT2046::cleanup() {
	}

}
