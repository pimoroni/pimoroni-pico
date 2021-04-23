#include <stdint.h>

#ifndef __SH1107_FONTS_H__
#define __SH1107_FONTS_H__

#include "sh1107_conf.h"

typedef struct {
	const uint8_t FontWidth;    /*!< Font width in pixels */
	uint8_t FontHeight;   /*!< Font height in pixels */
	const uint16_t *data; /*!< Pointer to data font data array */
} FontDef;

#ifdef SH1107_INCLUDE_FONT_6x8
extern FontDef Font_6x8;
#endif
#ifdef SH1107_INCLUDE_FONT_7x10
extern FontDef Font_7x10;
#endif
#ifdef SH1107_INCLUDE_FONT_11x18
extern FontDef Font_11x18;
#endif
#ifdef SH1107_INCLUDE_FONT_16x26
extern FontDef Font_16x26;
#endif
#endif // __SH1107_FONTS_H__
