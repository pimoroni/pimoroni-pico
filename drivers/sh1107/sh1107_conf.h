/**
 * Private configuration file for the SH1107 library, based on the SSD1306 one.
 * This example is configured for STM32F0, I2C and including all fonts.
 */

#ifndef __SH1107_CONF_H__
#define __SH1107_CONF_H__

//Choose a bus - default to I2C
//uncomment next line to use SPI
// #define SH1107_USE_SPI
#ifndef SH1107_USE_I2C
#ifndef SH1107_USE_SPI
#define SH1107_USE_I2C
#endif
#endif

// Mirror the screen if needed
#define SH1107_MIRROR_VERT
#define SH1107_MIRROR_HORIZ

// Set inverse color if needed
// # define SH1107_INVERSE_COLOR

// Include only needed fonts
#define SH1107_INCLUDE_FONT_6x8
#define SH1107_INCLUDE_FONT_7x10
#define SH1107_INCLUDE_FONT_11x18
#define SH1107_INCLUDE_FONT_16x26

// Some OLEDs don't display anything in first two columns.
// In this case change the following macro to 130.
// The default value is 128.
// #define SH1107_WIDTH           130

// The height can be changed as well if necessary.
// It can be 32, 64 or 128.
// #define SH1107_HEIGHT          128

#endif /* __SH1107_CONF_H__ */
