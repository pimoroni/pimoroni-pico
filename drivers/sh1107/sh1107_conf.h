/**
 * Private configuration file for the SH1107 library, based on the SSD1306 one.
 * This example is configured for STM32F0, I2C and including all fonts.
 */

#ifndef __SH1107_CONF_H__
#define __SH1107_CONF_H__

// Include only needed fonts
#define SH1107_INCLUDE_FONT_6x8
#define SH1107_INCLUDE_FONT_7x10
#define SH1107_INCLUDE_FONT_11x18
#define SH1107_INCLUDE_FONT_16x26

// DEPRECATED:
// Below values must be specified in display constructor

// Some OLEDs don't display anything in first two columns.
// In this case change the following macro to 130.
// The default value is 128.
// #define SH1107_WIDTH           130

// The height can be changed as well if necessary.
// It can be 32, 64 or 128.
// #define SH1107_HEIGHT          128

#endif /* __SH1107_CONF_H__ */
