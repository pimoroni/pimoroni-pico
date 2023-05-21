/*
 * Copyright 2006-2012 Red Hat, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * on the rights to use, copy, modify, merge, publish, distribute, sub
 * license, and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
/* Author: Adam Jackson <ajax@nwnk.net> */
/* Maintainer: Hans Verkuil <hverkuil-cisco@xs4all.nl> */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <ctype.h>
#include <math.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

enum {
	EDID_PAGE_SIZE = 128u
};

static int edid_minor = 0;
static int claims_one_point_oh = 0;
static int claims_one_point_two = 0;
static int claims_one_point_three = 0;
static int claims_one_point_four = 0;
static int nonconformant_digital_display = 0;
static int nonconformant_extension = 0;
static int did_detailed_timing = 0;
static int has_name_descriptor = 0;
static int has_serial_string = 0;
static int has_ascii_string = 0;
static int has_range_descriptor = 0;
static int has_preferred_timing = 0;
static int has_valid_checksum = 1;
static int has_valid_cta_checksum = 1;
static int has_valid_displayid_checksum = 1;
static int has_valid_cvt = 1;
static int has_valid_dummy_block = 1;
static int has_valid_serial_number = 0;
static int has_valid_serial_string = 0;
static int has_valid_ascii_string = 0;
static int has_valid_name_descriptor = 0;
static int has_valid_week = 0;
static int has_valid_year = 0;
static int has_valid_detailed_blocks = 0;
static int has_valid_descriptor_ordering = 1;
static int has_valid_descriptor_pad = 1;
static int has_valid_range_descriptor = 1;
static int has_valid_max_dotclock = 1;
static int has_valid_string_termination = 1;
static int empty_string = 0;
static int trailing_space = 0;
static int has_cta861 = 0;
static int has_640x480p60_est_timing = 0;
static int has_cta861_vic_1 = 0;
static int manufacturer_name_well_formed = 0;
static int seen_non_detailed_descriptor = 0;

static int warning_excessive_dotclock_correction = 0;
static int warning_zero_preferred_refresh = 0;
static int nonconformant_hf_vsdb_position = 0;
static int duplicate_scdb = 0;
static int nonconformant_srgb_chromaticity = 0;
static int nonconformant_cta861_640x480 = 0;
static int nonconformant_hdmi_vsdb_tmds_rate = 0;
static int nonconformant_hf_vsdb_tmds_rate = 0;
static int nonconformant_hf_eeodb = 0;

static int min_hor_freq_hz = 0xfffffff;
static int max_hor_freq_hz = 0;
static int min_vert_freq_hz = 0xfffffff;
static int max_vert_freq_hz = 0;
static int max_pixclk_khz = 0;
static int mon_min_hor_freq_hz = 0;
static int mon_max_hor_freq_hz = 0;
static int mon_min_vert_freq_hz = 0;
static int mon_max_vert_freq_hz = 0;
static int mon_max_pixclk_khz = 0;
static unsigned supported_hdmi_vic_codes = 0;
static unsigned supported_hdmi_vic_vsb_codes = 0;

static int conformant = 1;
static unsigned warnings;

enum output_format {
	OUT_FMT_DEFAULT,
	OUT_FMT_HEX,
	OUT_FMT_RAW,
	OUT_FMT_CARRAY
};

/*
 * Options
 * Please keep in alphabetical order of the short option.
 * That makes it easier to see which options are still free.
 */
enum Option {
	OptCheck = 'c',
	OptExtract = 'e',
	OptHelp = 'h',
	OptOutputFormat = 'o',
	OptLast = 256
};

static char options[OptLast];

static struct option long_options[] = {
	{ "help", no_argument, 0, OptHelp },
	{ "output-format", required_argument, 0, OptOutputFormat },
	{ "extract", no_argument, 0, OptExtract },
	{ "check", no_argument, 0, OptCheck },
	{ 0, 0, 0, 0 }
};

static void usage(void)
{
	printf("Usage: edid-decode <options> [in [out]]\n"
	       "  [in]                  EDID file to parse. Read from standard input if none given\n"
	       "                        or if the input filename is '-'.\n"
	       "  [out]                 Output the read EDID to this file. Write to standard output\n"
	       "                        if the output filename is '-'.\n"
	       "\nOptions:\n"
	       "  -o, --output-format=<fmt>\n"
	       "                        if [out] is specified, then write the EDID in this format\n"
	       "                        <fmt> is one of:\n"
	       "                        hex:    hex numbers in ascii text (default for stdout)\n"
	       "                        raw:    binary data (default unless writing to stdout)\n"
	       "                        carray: c-program struct\n"
	       "  -c, --check           check if the EDID conforms to the standards\n"
	       "  -e, --extract         extract the contents of the first block in hex values\n"
	       "  -h, --help            display this help message\n");
}

struct value {
	int value;
	const char *description;
};

struct field {
	const char *name;
	int start, end;
	struct value *values;
	int n_values;
};

static const char *cur_block;
static char *s_warn;
static unsigned int s_warn_len = 1;

static void warn(const char *fmt, ...)
{
	unsigned int length;
	char buf[256];
	va_list ap;

	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);
	warnings++;
	length = strlen(buf);
	s_warn = realloc(s_warn, s_warn_len + length + strlen(cur_block) + 2);
	strcpy(s_warn + s_warn_len - 1, cur_block);
	s_warn_len += strlen(cur_block);
	strcpy(s_warn + s_warn_len - 1, ": ");
	s_warn_len += 2;
	strcpy(s_warn + s_warn_len - 1, buf);
	s_warn_len += length;
}

#define DEFINE_FIELD(n, var, s, e, ...)				\
static struct value var##_values[] =  {				\
	__VA_ARGS__						\
};								\
static struct field var = {					\
	.name = n,						\
	.start = s,		        			\
	.end = e,						\
	.values = var##_values,	        			\
	.n_values = ARRAY_SIZE(var##_values),			\
}

static void decode_value(struct field *field, int val, const char *prefix)
{
	struct value *v;
	int i;

	for (i = 0; i < field->n_values; i++) {
		v = &field->values[i];

		if (v->value == val)
			break;
	}

	if (i == field->n_values) {
		printf("%s%s: %d\n", prefix, field->name, val);
		return;
	}

	printf("%s%s: %s (%d)\n", prefix, field->name, v->description, val);
}

static void _decode(struct field **fields, int n_fields, int data, const char *prefix)
{
	int i;

	for (i = 0; i < n_fields; i++) {
		struct field *f = fields[i];
		int field_length = f->end - f->start + 1;
		int val;

		if (field_length == 32)
			val = data;
		else
			val = (data >> f->start) & ((1 << field_length) - 1);

		decode_value(f, val, prefix);
	}
}

#define decode(fields, data, prefix)    \
	_decode(fields, ARRAY_SIZE(fields), data, prefix)

static char *manufacturer_name(const unsigned char *x)
{
	static char name[4];

	name[0] = ((x[0] & 0x7C) >> 2) + '@';
	name[1] = ((x[0] & 0x03) << 3) + ((x[1] & 0xE0) >> 5) + '@';
	name[2] = (x[1] & 0x1F) + '@';
	name[3] = 0;

	if (isupper(name[0]) && isupper(name[1]) && isupper(name[2]))
		manufacturer_name_well_formed = 1;

	return name;
}

/*
 * Copied from xserver/hw/xfree86/modes/xf86cvt.c
 */
static void edid_cvt_mode(int HDisplay, int VDisplay,
			  int VRefresh, int Reduced,
			  unsigned *MinHFreq, unsigned *MaxHFreq,
			  unsigned *MaxClock)
{
	/* 1) top/bottom margin size (% of height) - default: 1.8 */
#define CVT_MARGIN_PERCENTAGE 1.8

	/* 2) character cell horizontal granularity (pixels) - default 8 */
#define CVT_H_GRANULARITY 8

	/* 4) Minimum vertical porch (lines) - default 3 */
#define CVT_MIN_V_PORCH 3

	/* 4) Minimum number of vertical back porch lines - default 6 */
#define CVT_MIN_V_BPORCH 6

	/* Pixel Clock step (kHz) */
#define CVT_CLOCK_STEP 250

	float HPeriod;
	unsigned HTotal, Clock, HorFreq;
	int VSync;

	/* 2. Horizontal pixels */
	HDisplay = HDisplay - (HDisplay % CVT_H_GRANULARITY);

	/* Determine VSync Width from aspect ratio */
	if (!(VDisplay % 3) && ((VDisplay * 4 / 3) == HDisplay))
		VSync = 4;
	else if (!(VDisplay % 9) && ((VDisplay * 16 / 9) == HDisplay))
		VSync = 5;
	else if (!(VDisplay % 10) && ((VDisplay * 16 / 10) == HDisplay))
		VSync = 6;
	else if (!(VDisplay % 4) && ((VDisplay * 5 / 4) == HDisplay))
		VSync = 7;
	else if (!(VDisplay % 9) && ((VDisplay * 15 / 9) == HDisplay))
		VSync = 7;
	else                        /* Custom */
		VSync = 10;

	if (!Reduced) {             /* simplified GTF calculation */

		/* 4) Minimum time of vertical sync + back porch interval (µs)
		 * default 550.0 */
#define CVT_MIN_VSYNC_BP 550.0

		/* 3) Nominal HSync width (% of line period) - default 8 */
#define CVT_HSYNC_PERCENTAGE 8

		float HBlankPercentage;
		int HBlank;

		/* 8. Estimated Horizontal period */
		HPeriod = ((float) (1000000.0 / VRefresh - CVT_MIN_VSYNC_BP)) /
			(VDisplay + CVT_MIN_V_PORCH);

		/* 5) Definition of Horizontal blanking time limitation */
		/* Gradient (%/kHz) - default 600 */
#define CVT_M_FACTOR 600

		/* Offset (%) - default 40 */
#define CVT_C_FACTOR 40

		/* Blanking time scaling factor - default 128 */
#define CVT_K_FACTOR 128

		/* Scaling factor weighting - default 20 */
#define CVT_J_FACTOR 20

#define CVT_M_PRIME (CVT_M_FACTOR * CVT_K_FACTOR / 256)
#define CVT_C_PRIME ((CVT_C_FACTOR - CVT_J_FACTOR) * CVT_K_FACTOR / 256 + CVT_J_FACTOR)

		/* 12. Find ideal blanking duty cycle from formula */
		HBlankPercentage = CVT_C_PRIME - CVT_M_PRIME * HPeriod / 1000.0;

		/* 13. Blanking time */
		if (HBlankPercentage < 20)
			HBlankPercentage = 20;

		HBlank = HDisplay * HBlankPercentage / (100.0 - HBlankPercentage);
		HBlank -= HBlank % (2 * CVT_H_GRANULARITY);

		/* 14. Find total number of pixels in a line. */
		HTotal = HDisplay + HBlank;
	}
	else {                      /* Reduced blanking */
		/* Minimum vertical blanking interval time (µs) - default 460 */
#define CVT_RB_MIN_VBLANK 460.0

		/* Fixed number of clocks for horizontal sync */
#define CVT_RB_H_SYNC 32.0

		/* Fixed number of clocks for horizontal blanking */
#define CVT_RB_H_BLANK 160.0

		/* Fixed number of lines for vertical front porch - default 3 */
#define CVT_RB_VFPORCH 3

		int VBILines;

		/* 8. Estimate Horizontal period. */
		HPeriod = ((float) (1000000.0 / VRefresh - CVT_RB_MIN_VBLANK)) / VDisplay;

		/* 9. Find number of lines in vertical blanking */
		VBILines = ((float) CVT_RB_MIN_VBLANK) / HPeriod + 1;

		/* 10. Check if vertical blanking is sufficient */
		if (VBILines < (CVT_RB_VFPORCH + VSync + CVT_MIN_V_BPORCH))
			VBILines = CVT_RB_VFPORCH + VSync + CVT_MIN_V_BPORCH;

		/* 12. Find total number of pixels in a line */
		HTotal = HDisplay + CVT_RB_H_BLANK;
	}

	/* 15/13. Find pixel clock frequency (kHz for xf86) */
	Clock = HTotal * 1000.0 / HPeriod;
	Clock -= Clock % CVT_CLOCK_STEP;
	HorFreq = (Clock * 1000) / HTotal;

	*MinHFreq = min(*MinHFreq, HorFreq);
	*MaxHFreq = max(*MaxHFreq, HorFreq);
	*MaxClock = max(*MaxClock, Clock);
	min_hor_freq_hz = min(min_hor_freq_hz, HorFreq);
	max_hor_freq_hz = max(max_hor_freq_hz, HorFreq);
	max_pixclk_khz = max(max_pixclk_khz, Clock);
}

static int detailed_cvt_descriptor(const unsigned char *x, int first)
{
	const unsigned char empty[3] = { 0, 0, 0 };
	const char *ratio;
	char *names[] = { "50", "60", "75", "85" };
	int width, height;
	int valid = 1;
	int fifty = 0, sixty = 0, seventyfive = 0, eightyfive = 0, reduced = 0;
	int min_refresh = 0xfffffff, max_refresh = 0;

	if (!first && !memcmp(x, empty, 3))
		return valid;

	height = x[0];
	height |= (x[1] & 0xf0) << 4;
	height++;
	height *= 2;

	switch (x[1] & 0x0c) {
	case 0x00:
		width = 8 * (((height * 4) / 3) / 8);
		ratio = "4:3";
		break;
	case 0x04:
		width = 8 * (((height * 16) / 9) / 8);
		ratio = "16:9";
		break;
	case 0x08:
		width = 8 * (((height * 16) / 10) / 8);
		ratio = "16:10";
		break;
	case 0x0c:
		width = 8 * (((height * 15) / 9) / 8);
		ratio = "15:9";
		break;
	}

	if (x[1] & 0x03)
		valid = 0;
	if (x[2] & 0x80)
		valid = 0;
	if (!(x[2] & 0x1f))
		valid = 0;

	fifty	= (x[2] & 0x10);
	sixty	= (x[2] & 0x08);
	seventyfive = (x[2] & 0x04);
	eightyfive  = (x[2] & 0x02);
	reduced	= (x[2] & 0x01);

	min_refresh = (fifty ? 50 : (sixty ? 60 : (seventyfive ? 75 : (eightyfive ? 85 : min_refresh))));
	max_refresh = (eightyfive ? 85 : (seventyfive ? 75 : (sixty ? 60 : (fifty ? 50 : max_refresh))));

	if (!valid) {
		printf("    (broken)\n");
	} else {
		unsigned min_hfreq = ~0;
		unsigned max_hfreq = 0;
		unsigned max_clock = 0;

		min_vert_freq_hz = min(min_vert_freq_hz, min_refresh);
		max_vert_freq_hz = max(max_vert_freq_hz, max_refresh);

		if (fifty)
			edid_cvt_mode(width, height, 50, 0,
				      &min_hfreq, &max_hfreq, &max_clock);
		if (sixty)
			edid_cvt_mode(width, height, 60, 0,
				      &min_hfreq, &max_hfreq, &max_clock);
		if (seventyfive)
			edid_cvt_mode(width, height, 75, 0,
				      &min_hfreq, &max_hfreq, &max_clock);
		if (eightyfive)
			edid_cvt_mode(width, height, 75, 0,
				      &min_hfreq, &max_hfreq, &max_clock);
		if (reduced)
			edid_cvt_mode(width, height, 60, 1,
				      &min_hfreq, &max_hfreq, &max_clock);

		printf("    %dx%d @ ( %s%s%s%s%s) Hz %s (%s%s preferred) HorFreq: %d-%d Hz MaxClock: %.3f MHz\n",
		       width, height,
		       fifty ? "50 " : "",
		       sixty ? "60 " : "",
		       seventyfive ? "75 " : "",
		       eightyfive ? "85 " : "",
		       reduced ? "60RB " : "",
		       ratio,
		       names[(x[2] & 0x60) >> 5],
		       (((x[2] & 0x60) == 0x20) && reduced) ? "RB" : "",
		       min_hfreq, max_hfreq, max_clock / 1000000.0);
	}

	return valid;
}

/* extract a string from a detailed subblock, checking for termination */
static char *extract_string(const char *name, const unsigned char *x, int *valid, int len)
{
	static char ret[EDID_PAGE_SIZE];
	int i, seen_newline = 0;

	memset(ret, 0, sizeof(ret));
	*valid = 1;

	for (i = 0; i < len; i++) {
		if (isgraph(x[i])) {
			ret[i] = x[i];
		} else if (!seen_newline) {
			if (x[i] == 0x0a) {
				seen_newline = 1;
				if (!i) {
					empty_string = 1;
					warn("%s: empty string\n", name);
					*valid = 0;
				} else if (ret[i - 1] == 0x20) {
					warn("%s: one or more trailing spaces\n", name);
					trailing_space = 1;
					*valid = 0;
				}
			} else if (x[i] == 0x20) {
				ret[i] = x[i];
			} else {
				has_valid_string_termination = 0;
				warn("%s: non-printable character\n", name);
				*valid = 0;
				return ret;
			}
		} else if (x[i] != 0x20) {
			has_valid_string_termination = 0;
			warn("%s: non-space after newline\n", name);
			*valid = 0;
			return ret;
		}
	}
	/* Does the string end with a space? */
	if (!seen_newline && ret[len - 1] == 0x20) {
		trailing_space = 1;
		warn("%s: one or more trailing spaces\n", name);
		*valid = 0;
	}

	return ret;
}

static const struct {
	int x, y, refresh, ratio_w, ratio_h;
	int hor_freq_hz, pixclk_khz, interlaced;
} established_timings[] = {
	/* 0x23 bit 7 - 0 */
	{720, 400, 70, 9, 5, 31469, 28320},
	{720, 400, 88, 9, 5, 39500, 35500},
	{640, 480, 60, 4, 3, 31469, 25175},
	{640, 480, 67, 4, 3, 35000, 30240},
	{640, 480, 72, 4, 3, 37900, 31500},
	{640, 480, 75, 4, 3, 37500, 31500},
	{800, 600, 56, 4, 3, 35200, 36000},
	{800, 600, 60, 4, 3, 37900, 40000},
	/* 0x24 bit 7 - 0 */
	{800, 600, 72, 4, 3, 48100, 50000},
	{800, 600, 75, 4, 3, 46900, 49500},
	{832, 624, 75, 4, 3, 49726, 57284},
	{1280, 768, 87, 5, 3, 35522, 44900, 1},
	{1024, 768, 60, 4, 3, 48400, 65000},
	{1024, 768, 70, 4, 3, 56500, 75000},
	{1024, 768, 75, 4, 3, 60000, 78750},
	{1280, 1024, 75, 5, 4, 80000, 135000},
	/* 0x25 bit 7*/
	{1152, 870, 75, 192, 145, 67500, 108000},
};

static const struct {
	int x, y, refresh, ratio_w, ratio_h;
	int hor_freq_hz, pixclk_khz, rb;
} established_timings3[] = {
	/* 0x06 bit 7 - 0 */
	{640, 350, 85, 64, 35, 37900, 31500},
	{640, 400, 85, 16, 10, 37900, 31500},
	{720, 400, 85, 9, 5, 37900, 35500},
	{640, 480, 85, 4, 3, 43300, 36000},
	{848, 480, 60, 53, 30, 31000, 33750},
	{800, 600, 85, 4, 3, 53700, 56250},
	{1024, 768, 85, 4, 3, 68700, 94500},
	{1152, 864, 75, 4, 3, 67500, 108000},
	/* 0x07 bit 7 - 0 */
	{1280, 768, 60, 5, 3, 47400, 68250, 1},
	{1280, 768, 60, 5, 3, 47800, 79500},
	{1280, 768, 75, 5, 3, 60300, 102250},
	{1280, 768, 85, 5, 3, 68600, 117500},
	{1280, 960, 60, 4, 3, 60000, 108000},
	{1280, 960, 85, 4, 3, 85900, 148500},
	{1280, 1024, 60, 5, 4, 64000, 108000},
	{1280, 1024, 85, 5, 4, 91100, 157500},
	/* 0x08 bit 7 - 0 */
	{1360, 768, 60, 85, 48, 47700, 85500},
	{1440, 900, 60, 16, 10, 55500, 88750, 1},
	{1440, 900, 60, 16, 10, 65300, 121750},
	{1440, 900, 75, 16, 10, 82300, 156000},
	{1440, 900, 85, 16, 10, 93900, 179500},
	{1400, 1050, 60, 4, 3, 64700, 101000, 1},
	{1400, 1050, 60, 4, 3, 65300, 121750},
	{1400, 1050, 75, 4, 3, 82300, 156000},
	/* 0x09 bit 7 - 0 */
	{1400, 1050, 85, 4, 3, 93900, 179500},
	{1680, 1050, 60, 16, 10, 64700, 119000, 1},
	{1680, 1050, 60, 16, 10, 65300, 146250},
	{1680, 1050, 75, 16, 10, 82300, 187000},
	{1680, 1050, 85, 16, 10, 93900, 214750},
	{1600, 1200, 60, 4, 3, 75000, 162000},
	{1600, 1200, 65, 4, 3, 81300, 175500},
	{1600, 1200, 70, 4, 3, 87500, 189000},
	/* 0x0a bit 7 - 0 */
	{1600, 1200, 75, 4, 3, 93800, 202500},
	{1600, 1200, 85, 4, 3, 106300, 229500},
	{1792, 1344, 60, 4, 3, 83600, 204750},
	{1792, 1344, 75, 4, 3, 106300, 261000},
	{1856, 1392, 60, 4, 3, 86300, 218250},
	{1856, 1392, 75, 4, 3, 112500, 288000},
	{1920, 1200, 60, 16, 10, 74000, 154000, 1},
	{1920, 1200, 60, 16, 10, 74600, 193250},
	/* 0x0b bit 7 - 4 */
	{1920, 1200, 75, 16, 10, 94000, 245250},
	{1920, 1200, 85, 16, 10, 107200, 281250},
	{1920, 1440, 60, 4, 3, 90000, 234000},
	{1920, 1440, 75, 4, 3, 112500, 297000},
};

static void print_standard_timing(uint8_t b1, uint8_t b2)
{
	int ratio_w, ratio_h;
	unsigned int x, y, refresh;
	int pixclk_khz = 0, hor_freq_hz = 0;
	int i;

	if (b1 == 0x01 && b2 == 0x01)
		return;

	if (b1 == 0) {
		printf("non-conformant standard timing (0 horiz)\n");
		return;
	}
	x = (b1 + 31) * 8;
	switch ((b2 >> 6) & 0x3) {
	case 0x00:
		if (claims_one_point_three) {
			y = x * 10 / 16;
			ratio_w = 16;
			ratio_h = 10;
		} else {
			y = x;
			ratio_w = 1;
			ratio_h = 1;
		}
		break;
	case 0x01:
		y = x * 3 / 4;
		ratio_w = 4;
		ratio_h = 3;
		break;
	case 0x02:
		y = x * 4 / 5;
		ratio_w = 5;
		ratio_h = 4;
		break;
	case 0x03:
		y = x * 9 / 16;
		ratio_w = 16;
		ratio_h = 9;
		break;
	}
	refresh = 60 + (b2 & 0x3f);

	min_vert_freq_hz = min(min_vert_freq_hz, refresh);
	max_vert_freq_hz = max(max_vert_freq_hz, refresh);
	for (i = 0; i < ARRAY_SIZE(established_timings); i++) {
		if (established_timings[i].x == x &&
		    established_timings[i].y == y &&
		    established_timings[i].refresh == refresh &&
		    established_timings[i].ratio_w == ratio_w &&
		    established_timings[i].ratio_h == ratio_h) {
			pixclk_khz = established_timings[i].pixclk_khz;
			hor_freq_hz = established_timings[i].hor_freq_hz;
			break;
		}
	}
	if (pixclk_khz == 0) {
		for (i = 0; i < ARRAY_SIZE(established_timings3); i++) {
			if (established_timings3[i].x == x &&
			    established_timings3[i].y == y &&
			    established_timings3[i].refresh == refresh &&
			    established_timings3[i].ratio_w == ratio_w &&
			    established_timings3[i].ratio_h == ratio_h) {
				pixclk_khz = established_timings3[i].pixclk_khz;
				hor_freq_hz = established_timings3[i].hor_freq_hz;
				break;
			}
		}
	}
	/* TODO: this should also check DMT timings and GTF/CVT */
	if (pixclk_khz) {
		min_hor_freq_hz = min(min_hor_freq_hz, hor_freq_hz);
		max_hor_freq_hz = max(max_hor_freq_hz, hor_freq_hz);
		max_pixclk_khz = max(max_pixclk_khz, pixclk_khz);
		printf("  %dx%d@%dHz %d:%d HorFreq: %d Hz Clock: %.3f MHz\n",
		       x, y, refresh, ratio_w, ratio_h,
		       hor_freq_hz, pixclk_khz / 1000.0);
	} else {
		printf("  %dx%d@%dHz %d:%d\n",
		       x, y, refresh, ratio_w, ratio_h);
	}
}

/* 1 means valid data */
static int detailed_block(const unsigned char *x, int in_extension)
{
	int ha, hbl, hso, hspw, hborder, va, vbl, vso, vspw, vborder;
	int refresh, pixclk_khz;
	int i;
	char phsync, pvsync, *syncmethod, *stereo;

#if 0
	printf("Hex of detail: ");
	for (i = 0; i < 18; i++)
		printf("%02x", x[i]);
	printf("\n");
#endif

	if (x[0] == 0 && x[1] == 0) {
		/* Monitor descriptor block, not detailed timing descriptor. */
		if (x[2] != 0) {
			/* 1.3, 3.10.3 */
			printf("Monitor descriptor block has byte 2 nonzero (0x%02x)\n",
			       x[2]);
			has_valid_descriptor_pad = 0;
		}
		if (x[3] != 0xfd && x[4] != 0x00) {
			/* 1.3, 3.10.3 */
			printf("Monitor descriptor block has byte 4 nonzero (0x%02x)\n",
			       x[4]);
			has_valid_descriptor_pad = 0;
		}

		seen_non_detailed_descriptor = 1;
		if (x[3] <= 0xF) {
			/*
			 * in principle we can decode these, if we know what they are.
			 * 0x0f seems to be common in laptop panels.
			 * 0x0e is used by EPI: http://www.epi-standard.org/
			 */
			printf("Manufacturer-specified data, tag %d\n", x[3]);
			return 1;
		}
		switch (x[3]) {
		case 0x10:
			printf("Dummy block\n");
			for (i = 5; i < 18; i++)
				if (x[i] != 0x00)
					has_valid_dummy_block = 0;
			return 1;
		case 0xF7:
			printf("Established timings III:\n");
			for (i = 0; i < 44; i++) {
				if (x[6 + i / 8] & (1 << (7 - i % 8))) {
					printf("  %dx%d@%dHz %s%u:%u HorFreq: %d Hz Clock: %.3f MHz\n",
					       established_timings3[i].x,
					       established_timings3[i].y, established_timings3[i].refresh,
					       established_timings3[i].rb ? "RB " : "",
					       established_timings3[i].ratio_w, established_timings3[i].ratio_h,
					       established_timings3[i].hor_freq_hz,
					       established_timings3[i].pixclk_khz / 1000.0);
					min_vert_freq_hz = min(min_vert_freq_hz, established_timings3[i].refresh);
					max_vert_freq_hz = max(max_vert_freq_hz, established_timings3[i].refresh);
					min_hor_freq_hz = min(min_hor_freq_hz, established_timings3[i].hor_freq_hz);
					max_hor_freq_hz = max(max_hor_freq_hz, established_timings3[i].hor_freq_hz);
					max_pixclk_khz = max(max_pixclk_khz, established_timings3[i].pixclk_khz);
				}
			}
			return 1;
		case 0xF8: {
			int valid_cvt = 1; /* just this block */
			printf("CVT 3-byte code descriptor:\n");
			if (x[5] != 0x01) {
				has_valid_cvt = 0;
				return 0;
			}
			for (i = 0; i < 4; i++)
				valid_cvt &= detailed_cvt_descriptor(x + 6 + (i * 3), (i == 0));
			has_valid_cvt &= valid_cvt;
			return valid_cvt;
		}
		case 0xF9:
			printf("Color management data:\n");
			printf("  Version:  %d\n", x[5]);
			printf("  Red a3:   %.2f\n", (short)(x[6] | (x[7] << 8)) / 100.0);
			printf("  Red a2:   %.2f\n", (short)(x[8] | (x[9] << 8)) / 100.0);
			printf("  Green a3: %.2f\n", (short)(x[10] | (x[11] << 8)) / 100.0);
			printf("  Green a2: %.2f\n", (short)(x[12] | (x[13] << 8)) / 100.0);
			printf("  Blue a3:  %.2f\n", (short)(x[14] | (x[15] << 8)) / 100.0);
			printf("  Blue a2:  %.2f\n", (short)(x[16] | (x[17] << 8)) / 100.0);
			return 1;
		case 0xFA:
			printf("More standard timings:\n");
			for (i = 0; i < 6; i++)
				print_standard_timing(x[5 + i * 2], x[5 + i * 2 + 1]);
			return 1;
		case 0xFB: {
			unsigned w_x, w_y;
			unsigned gamma;

			printf("Color point:\n");
			w_x = (x[7] << 2) | ((x[6] >> 2) & 3);
			w_y = (x[8] << 2) | (x[6] & 3);
			gamma = x[9];
			printf("  Index: %u White: 0.%04u, 0.%04u", x[5],
			       (w_x * 10000) / 1024, (w_y * 10000) / 1024);
			if (gamma == 0xff)
				printf(" Gamma: is defined in an extension block");
			else
				printf(" Gamma: %.2f", ((gamma + 100.0) / 100.0));
			printf("\n");
			if (x[10] == 0)
				return 1;
			w_x = (x[12] << 2) | ((x[11] >> 2) & 3);
			w_y = (x[13] << 2) | (x[11] & 3);
			gamma = x[14];
			printf("  Index: %u White: 0.%04u, 0.%04u", x[10],
			       (w_x * 10000) / 1024, (w_y * 10000) / 1024);
			if (gamma == 0xff)
				printf(" Gamma: is defined in an extension block");
			else
				printf(" Gamma: %.2f", ((gamma + 100.0) / 100.0));
			printf("\n");
			return 1;
		}
		case 0xFC:
			has_name_descriptor = 1;
			printf("Monitor name: %s\n",
			       extract_string("Display Product Name", x + 5, &has_valid_name_descriptor, 13));
			return 1;
		case 0xFD: {
			int h_max_offset = 0, h_min_offset = 0;
			int v_max_offset = 0, v_min_offset = 0;
			int is_cvt = 0;
			has_range_descriptor = 1;
			char *range_class = "";
			/* 
			 * XXX todo: implement feature flags, vtd blocks
			 * XXX check: ranges are well-formed; block termination if no vtd
			 */
			if (claims_one_point_four) {
				if (x[4] & 0x02) {
					v_max_offset = 255;
					if (x[4] & 0x01) {
						v_min_offset = 255;
					}
				}
				if (x[4] & 0x04) {
					h_max_offset = 255;
					if (x[4] & 0x03) {
						h_min_offset = 255;
					}
				}
			} else if (x[4]) {
				has_valid_range_descriptor = 0;
			}

			/*
			 * despite the values, this is not a bitfield.
			 */
			switch (x[10]) {
			case 0x00: /* default gtf */
				range_class = "GTF";
				break;
			case 0x01: /* range limits only */
				range_class = "bare limits";
				if (!claims_one_point_four)
					has_valid_range_descriptor = 0;
				break;
			case 0x02: /* secondary gtf curve */
				range_class = "GTF with icing";
				break;
			case 0x04: /* cvt */
				range_class = "CVT";
				is_cvt = 1;
				if (!claims_one_point_four)
					has_valid_range_descriptor = 0;
				break;
			default: /* invalid */
				has_valid_range_descriptor = 0;
				range_class = "invalid";
				break;
			}

			if (x[5] + v_min_offset > x[6] + v_max_offset)
				has_valid_range_descriptor = 0;
			mon_min_vert_freq_hz = x[5] + v_min_offset;
			mon_max_vert_freq_hz = x[6] + v_max_offset;
			if (x[7] + h_min_offset > x[8] + h_max_offset)
				has_valid_range_descriptor = 0;
			mon_min_hor_freq_hz = (x[7] + h_min_offset) * 1000;
			mon_max_hor_freq_hz = (x[8] + h_max_offset) * 1000;
			printf("Monitor ranges (%s): %d-%dHz V, %d-%dkHz H",
			       range_class,
			       x[5] + v_min_offset, x[6] + v_max_offset,
			       x[7] + h_min_offset, x[8] + h_max_offset);
			if (x[9]) {
				mon_max_pixclk_khz = x[9] * 10000;
				printf(", max dotclock %dMHz\n", x[9] * 10);
			} else {
				if (claims_one_point_four)
					has_valid_max_dotclock = 0;
				printf("\n");
			}

			if (is_cvt) {
				int max_h_pixels = 0;

				printf("CVT version %d.%d\n", (x[11] & 0xf0) >> 4, x[11] & 0x0f);

				if (x[12] & 0xfc) {
					int raw_offset = (x[12] & 0xfc) >> 2;
					printf("Real max dotclock: %.2fMHz\n",
					       (x[9] * 10) - (raw_offset * 0.25));
					if (raw_offset >= 40)
						warning_excessive_dotclock_correction = 1;
				}

				max_h_pixels = x[12] & 0x03;
				max_h_pixels <<= 8;
				max_h_pixels |= x[13];
				max_h_pixels *= 8;
				if (max_h_pixels)
					printf("Max active pixels per line: %d\n", max_h_pixels);

				printf("Supported aspect ratios: %s %s %s %s %s\n",
				       x[14] & 0x80 ? "4:3" : "",
				       x[14] & 0x40 ? "16:9" : "",
				       x[14] & 0x20 ? "16:10" : "",
				       x[14] & 0x10 ? "5:4" : "",
				       x[14] & 0x08 ? "15:9" : "");
				if (x[14] & 0x07)
					has_valid_range_descriptor = 0;

				printf("Preferred aspect ratio: ");
				switch((x[15] & 0xe0) >> 5) {
				case 0x00: printf("4:3"); break;
				case 0x01: printf("16:9"); break;
				case 0x02: printf("16:10"); break;
				case 0x03: printf("5:4"); break;
				case 0x04: printf("15:9"); break;
				default: printf("(broken)"); break;
				}
				printf("\n");

				if (x[15] & 0x08)
					printf("Supports CVT standard blanking\n");
				if (x[15] & 0x10)
					printf("Supports CVT reduced blanking\n");

				if (x[15] & 0x07)
					has_valid_range_descriptor = 0;

				if (x[16] & 0xf0) {
					printf("Supported display scaling:\n");
					if (x[16] & 0x80)
						printf("    Horizontal shrink\n");
					if (x[16] & 0x40)
						printf("    Horizontal stretch\n");
					if (x[16] & 0x20)
						printf("    Vertical shrink\n");
					if (x[16] & 0x10)
						printf("    Vertical stretch\n");
				}

				if (x[16] & 0x0f)
					has_valid_range_descriptor = 0;

				if (x[17])
					printf("Preferred vertical refresh: %d Hz\n", x[17]);
				else
					warning_zero_preferred_refresh = 1;
			}

			/*
			 * Slightly weird to return a global, but I've never seen any
			 * EDID block wth two range descriptors, so it's harmless.
			 */
			return has_valid_range_descriptor;
		}
		case 0xFE:
			/*
			 * TODO: Two of these in a row, in the third and fourth slots,
			 * seems to be specified by SPWG: http://www.spwg.org/
			 */
			has_ascii_string = 1;
			printf("ASCII string: %s\n",
			       extract_string("Alphanumeric Data String", x + 5, &has_valid_ascii_string, 13));
			return 1;
		case 0xFF:
			has_serial_string = 1;
			printf("Serial number: %s\n",
			       extract_string("Display Product Serial Number", x + 5, &has_valid_serial_string, 13));
			return 1;
		default:
			printf("Unknown monitor description type %d\n", x[3]);
			return 0;
		}
	}

	if (seen_non_detailed_descriptor && !in_extension) {
		has_valid_descriptor_ordering = 0;
	}

	did_detailed_timing = 1;
	ha = (x[2] + ((x[4] & 0xF0) << 4));
	hbl = (x[3] + ((x[4] & 0x0F) << 8));
	hso = (x[8] + ((x[11] & 0xC0) << 2));
	hspw = (x[9] + ((x[11] & 0x30) << 4));
	hborder = x[15];
	va = (x[5] + ((x[7] & 0xF0) << 4));
	vbl = (x[6] + ((x[7] & 0x0F) << 8));
	vso = ((x[10] >> 4) + ((x[11] & 0x0C) << 2));
	vspw = ((x[10] & 0x0F) + ((x[11] & 0x03) << 4));
	vborder = x[16];
	switch ((x[17] & 0x18) >> 3) {
	case 0x00:
		syncmethod = " analog composite";
		break;
	case 0x01:
		syncmethod = " bipolar analog composite";
		break;
	case 0x02:
		syncmethod = " digital composite";
		break;
	case 0x03:
		syncmethod = "";
		break;
	}
	pvsync = (x[17] & (1 << 2)) ? '+' : '-';
	phsync = (x[17] & (1 << 1)) ? '+' : '-';
	switch (x[17] & 0x61) {
	case 0x20:
		stereo = "field sequential L/R";
		break;
	case 0x40:
		stereo = "field sequential R/L";
		break;
	case 0x21:
		stereo = "interleaved right even";
		break;
	case 0x41:
		stereo = "interleaved left even";
		break;
	case 0x60:
		stereo = "four way interleaved";
		break;
	case 0x61:
		stereo = "side by side interleaved";
		break;
	default:
		stereo = "";
		break;
	}

	if (!ha || !hbl || !va || !vbl) {
		printf("Invalid Detailed Timings:\n"
		       "  Horizontal Active/Blanking %d/%d\n"
		       "  Vertical Active/Blanking %d/%d\n",
		       ha, hbl, va, vbl);
		return 0;
	}

	pixclk_khz = (x[0] + (x[1] << 8)) * 10;
	refresh = (pixclk_khz * 1000) / ((ha + hbl) * (va + vbl));
	printf("Detailed mode: Clock %.3f MHz, %d mm x %d mm\n"
	       "               %4d %4d %4d %4d hborder %d\n"
	       "               %4d %4d %4d %4d vborder %d\n"
	       "               %chsync %cvsync%s%s %s\n"
	       "               VertFreq: %d Hz, HorFreq: %d Hz\n",
	       pixclk_khz / 1000.0,
	       (x[12] + ((x[14] & 0xF0) << 4)),
	       (x[13] + ((x[14] & 0x0F) << 8)),
	       ha, ha + hso, ha + hso + hspw, ha + hbl, hborder,
	       va, va + vso, va + vso + vspw, va + vbl, vborder,
	       phsync, pvsync, syncmethod, x[17] & 0x80 ? " interlaced" : "",
	       stereo, refresh, (pixclk_khz * 1000) / (ha + hbl)
	      );
	min_vert_freq_hz = min(min_vert_freq_hz, refresh);
	max_vert_freq_hz = max(max_vert_freq_hz, refresh);
	min_hor_freq_hz = min(min_hor_freq_hz, (pixclk_khz * 1000) / (ha + hbl));
	max_hor_freq_hz = max(max_hor_freq_hz, (pixclk_khz * 1000) / (ha + hbl));
	max_pixclk_khz = max(max_pixclk_khz, pixclk_khz);
	/* XXX flag decode */

	return 1;
}

static int do_checksum(const unsigned char *x, size_t len)
{
	unsigned char check = x[len - 1];
	unsigned char sum = 0;
	int i;

	printf("Checksum: 0x%hx", check);

	for (i = 0; i < len-1; i++)
		sum += x[i];

	if ((unsigned char)(check + sum) != 0) {
		printf(" (should be 0x%hx)\n", -sum & 0xff);
		return 0;
	}

	printf(" (valid)\n");
	return 1;
}

/* CTA extension */

static const char *audio_ext_format(unsigned char x)
{
	switch (x) {
	case 4: return "MPEG-4 HE AAC";
	case 5: return "MPEG-4 HE AAC v2";
	case 6: return "MPEG-4 AAC LC";
	case 7: return "DRA";
	case 8: return "MPEG-4 HE AAC + MPEG Surround";
	case 10: return "MPEG-4 AAC LC + MPEG Surround";
	case 11: return "MPEG-H 3D Audio";
	case 12: return "AC-4";
	case 13: return "L-PCM 3D Audio";
	default: return "RESERVED";
	}
	return "BROKEN"; /* can't happen */
}

static const char *audio_format(unsigned char x)
{
	switch (x) {
	case 0: return "RESERVED";
	case 1: return "Linear PCM";
	case 2: return "AC-3";
	case 3: return "MPEG 1 (Layers 1 & 2)";
	case 4: return "MPEG 1 Layer 3 (MP3)";
	case 5: return "MPEG2 (multichannel)";
	case 6: return "AAC";
	case 7: return "DTS";
	case 8: return "ATRAC";
	case 9: return "One Bit Audio";
	case 10: return "Dolby Digital+";
	case 11: return "DTS-HD";
	case 12: return "MAT (MLP)";
	case 13: return "DST";
	case 14: return "WMA Pro";
	case 15: return "RESERVED";
	}
	return "BROKEN"; /* can't happen */
}

static const char *mpeg_h_3d_audio_level(unsigned char x)
{
	switch (x) {
	case 0: return "Unspecified";
	case 1: return "Level 1";
	case 2: return "Level 2";
	case 3: return "Level 3";
	case 4: return "Level 4";
	case 5: return "Level 5";
	case 6: return "Reserved";
	case 7: return "Reserved";
	}
	return "BROKEN"; /* can't happen */
}

static void cta_audio_block(const unsigned char *x, unsigned int length)
{
	int i, format, ext_format = 0;

	if (length % 3) {
		printf("Broken CTA audio block length %d\n", length);
		/* XXX non-conformant */
		return;
	}

	for (i = 0; i < length; i += 3) {
		format = (x[i] & 0x78) >> 3;
		ext_format = (x[i + 2] & 0xf8) >> 3;
		if (format != 15)
			printf("    %s, max channels %d\n", audio_format(format),
			       (x[i] & 0x07)+1);
		else if (ext_format == 11)
			printf("    %s, MPEG-H 3D Audio Level: %s\n", audio_ext_format(ext_format),
			       mpeg_h_3d_audio_level(x[i] & 0x07));
		else if (ext_format == 13)
			printf("    %s, max channels %d\n", audio_ext_format(ext_format),
			       (((x[i + 1] & 0x80) >> 3) | ((x[i] & 0x80) >> 4) |
				(x[i] & 0x07))+1);
		else
			printf("    %s, max channels %d\n", audio_ext_format(ext_format),
			       (x[i] & 0x07)+1);
		printf("      Supported sample rates (kHz):%s%s%s%s%s%s%s\n",
		       (x[i+1] & 0x40) ? " 192" : "",
		       (x[i+1] & 0x20) ? " 176.4" : "",
		       (x[i+1] & 0x10) ? " 96" : "",
		       (x[i+1] & 0x08) ? " 88.2" : "",
		       (x[i+1] & 0x04) ? " 48" : "",
		       (x[i+1] & 0x02) ? " 44.1" : "",
		       (x[i+1] & 0x01) ? " 32" : "");
		if (format == 1 || ext_format == 13) {
			printf("      Supported sample sizes (bits):%s%s%s\n",
			       (x[i+2] & 0x04) ? " 24" : "",
			       (x[i+2] & 0x02) ? " 20" : "",
			       (x[i+2] & 0x01) ? " 16" : "");
		} else if (format <= 8) {
			printf("      Maximum bit rate: %d kb/s\n", x[i+2] * 8);
		} else if (format == 10) {
			// As specified by the "Dolby Audio and Dolby Atmos over HDMI"
			// specification (v1.0).
			if(x[i+2] & 1)
				printf("      Supports Joint Object Coding\n");
			if(x[i+2] & 2)
				printf("      Supports Joint Object Coding with ACMOD28\n");
		} else if (format == 14) {
			printf("      Profile: %d\n", x[i+2] & 7);
		} else if (ext_format == 11 && (x[i+2] & 1)) {
			printf("      Supports MPEG-H 3D Audio Low Complexity Profile\n");
		} else if ((ext_format >= 4 && ext_format <= 6) ||
			   ext_format == 8 || ext_format == 10) {
			printf("      AAC audio frame lengths:%s%s\n",
			       (x[i+2] & 4) ? " 1024_TL" : "",
			       (x[i+2] & 2) ? " 960_TL" : "");
			if (ext_format >= 8 && (x[i+2] & 1))
				printf("      Supports %s signaled MPEG Surround data\n",
				       (x[i+2] & 1) ? "implicitly and explicitly" : "only implicitly");
			if (ext_format == 6 && (x[i+2] & 1))
				printf("      Supports 22.2ch System H\n");
		}
	}
}

struct edid_cta_mode {
	const char *name;
	int refresh, hor_freq_hz, pixclk_khz;
};

static struct edid_cta_mode edid_cta_modes1[] = {
	/* VIC 1 */
	{"640x480@60Hz 4:3", 60, 31469, 25175},
	{"720x480@60Hz 4:3", 60, 31469, 27000},
	{"720x480@60Hz 16:9", 60, 31469, 27000},
	{"1280x720@60Hz 16:9", 60, 45000, 74250},
	{"1920x1080i@60Hz 16:9", 60, 33750, 74250},
	{"1440x480i@60Hz 4:3", 60, 15734, 27000},
	{"1440x480i@60Hz 16:9", 60, 15734, 27000},
	{"1440x240@60Hz 4:3", 60, 15734, 27000},
	{"1440x240@60Hz 16:9", 60, 15734, 27000},
	{"2880x480i@60Hz 4:3", 60, 15734, 54000},
	/* VIC 11 */
	{"2880x480i@60Hz 16:9", 60, 15734, 54000},
	{"2880x240@60Hz 4:3", 60, 15734, 54000},
	{"2880x240@60Hz 16:9", 60, 15734, 54000},
	{"1440x480@60Hz 4:3", 60, 31469, 54000},
	{"1440x480@60Hz 16:9", 60, 31469, 54000},
	{"1920x1080@60Hz 16:9", 60, 67500, 148500},
	{"720x576@50Hz 4:3", 50, 31250, 27000},
	{"720x576@50Hz 16:9", 50, 31250, 27000},
	{"1280x720@50Hz 16:9", 50, 37500, 74250},
	{"1920x1080i@50Hz 16:9", 50, 28125, 74250},
	/* VIC 21 */
	{"1440x576i@50Hz 4:3", 50, 15625, 27000},
	{"1440x576i@50Hz 16:9", 50, 15625, 27000},
	{"1440x288@50Hz 4:3", 50, 15625, 27000},
	{"1440x288@50Hz 16:9", 50, 15625, 27000},
	{"2880x576i@50Hz 4:3", 50, 15625, 54000},
	{"2880x576i@50Hz 16:9", 50, 15625, 54000},
	{"2880x288@50Hz 4:3", 50, 15625, 54000},
	{"2880x288@50Hz 16:9", 50, 15625, 54000},
	{"1440x576@50Hz 4:3", 50, 31250, 54000},
	{"1440x576@50Hz 16:9", 50, 31250, 54000},
	/* VIC 31 */
	{"1920x1080@50Hz 16:9", 50, 56250, 148500},
	{"1920x1080@24Hz 16:9", 24, 27000, 74250},
	{"1920x1080@25Hz 16:9", 25, 28125, 74250},
	{"1920x1080@30Hz 16:9", 30, 33750, 74250},
	{"2880x480@60Hz 4:3", 60, 31469, 108000},
	{"2880x480@60Hz 16:9", 60, 31469, 108000},
	{"2880x576@50Hz 4:3", 50, 31250, 108000},
	{"2880x576@50Hz 16:9", 50, 31250, 108000},
	{"1920x1080i@50Hz 16:9", 50, 31250, 72000},
	{"1920x1080i@100Hz 16:9", 100, 56250, 148500},
	/* VIC 41 */
	{"1280x720@100Hz 16:9", 100, 75000, 148500},
	{"720x576@100Hz 4:3", 100, 62500, 54000},
	{"720x576@100Hz 16:9", 100, 62500, 54000},
	{"1440x576@100Hz 4:3", 100, 31250, 54000},
	{"1440x576@100Hz 16:9", 100, 31250, 54000},
	{"1920x1080i@120Hz 16:9", 120, 67500, 148500},
	{"1280x720@120Hz 16:9", 120, 90000, 148500},
	{"720x480@120Hz 4:3", 120, 62937, 54000},
	{"720x480@120Hz 16:9", 120, 62937, 54000},
	{"1440x480i@120Hz 4:3", 120, 31469, 54000},
	/* VIC 51 */
	{"1440x480i@120Hz 16:9", 120, 31469, 54000},
	{"720x576@200Hz 4:3", 200, 125000, 108000},
	{"720x576@200Hz 16:9", 200, 125000, 108000},
	{"1440x576i@200Hz 4:3", 200, 62500, 108000},
	{"1440x576i@200Hz 16:9", 200, 62500, 108000},
	{"720x480@240Hz 4:3", 240, 125874, 108000},
	{"720x480@240Hz 16:9", 240, 125874, 108000},
	{"1440x480i@240Hz 4:3", 240, 62937, 108000},
	{"1440x480i@240Hz 16:9", 240, 62937, 108000},
	{"1280x720@24Hz 16:9", 24, 18000, 59400},
	/* VIC 61 */
	{"1280x720@25Hz 16:9", 25, 18750, 74250},
	{"1280x720@30Hz 16:9", 30, 22500, 74250},
	{"1920x1080@120Hz 16:9", 120, 135000, 297000},
	{"1920x1080@100Hz 16:9", 100, 112500, 297000},
	{"1280x720@24Hz 64:27", 24, 18000, 59400},
	{"1280x720@25Hz 64:27", 25, 18750, 74250},
	{"1280x720@30Hz 64:27", 30, 22500, 74250},
	{"1280x720@50Hz 64:27", 50, 37500, 74250},
	{"1280x720@60Hz 64:27", 60, 45000, 74250},
	{"1280x720@100Hz 64:27", 100, 75000, 148500},
	/* VIC 71 */
	{"1280x720@120Hz 64:27", 120, 91000, 148500},
	{"1920x1080@24Hz 64:27", 24, 27000, 74250},
	{"1920x1080@25Hz 64:27", 25, 28125, 74250},
	{"1920x1080@30Hz 64:27", 30, 33750, 74250},
	{"1920x1080@50Hz 64:27", 50, 56250, 148500},
	{"1920x1080@60Hz 64:27", 60, 67500, 148500},
	{"1920x1080@100Hz 64:27", 100, 112500, 297000},
	{"1920x1080@120Hz 64:27", 120, 135000, 297000},
	{"1680x720@24Hz 64:27", 24, 18000, 59400},
	{"1680x720@25Hz 64:27", 25, 18750, 59400},
	/* VIC 81 */
	{"1680x720@30Hz 64:27", 30, 22500, 59400},
	{"1680x720@50Hz 64:27", 50, 37500, 82500},
	{"1680x720@60Hz 64:27", 60, 45000, 99000},
	{"1680x720@100Hz 64:27", 100, 82500, 165000},
	{"1680x720@120Hz 64:27", 120, 99000, 198000},
	{"2560x1080@24Hz 64:27", 24, 26400, 99000},
	{"2560x1080@25Hz 64:27", 25, 28125, 90000},
	{"2560x1080@30Hz 64:27", 30, 33750, 118800},
	{"2560x1080@50Hz 64:27", 50, 56250, 185625},
	{"2560x1080@60Hz 64:27", 60, 66000, 198000},
	/* VIC 91 */
	{"2560x1080@100Hz 64:27", 100, 125000, 371250},
	{"2560x1080@120Hz 64:27", 120, 150000, 495000},
	{"3840x2160@24Hz 16:9", 24, 54000, 297000},
	{"3840x2160@25Hz 16:9", 25, 56250, 297000},
	{"3840x2160@30Hz 16:9", 30, 67500, 297000},
	{"3840x2160@50Hz 16:9", 50, 112500, 594000},
	{"3840x2160@60Hz 16:9", 60, 135000, 594000},
	{"4096x2160@24Hz 256:135", 24, 54000, 297000},
	{"4096x2160@25Hz 256:135", 25, 56250, 297000},
	{"4096x2160@30Hz 256:135", 30, 67500, 297000},
	/* VIC 101 */
	{"4096x2160@50Hz 256:135", 50, 112500, 594000},
	{"4096x2160@60Hz 256:135", 60, 135000, 594000},
	{"3840x2160@24Hz 64:27", 24, 54000, 297000},
	{"3840x2160@25Hz 64:27", 25, 56250, 297000},
	{"3840x2160@30Hz 64:27", 30, 67500, 297000},
	{"3840x2160@50Hz 64:27", 50, 112500, 594000},
	{"3840x2160@60Hz 64:27", 60, 135000, 594000},
	{"1280x720@48Hz 16:9", 48, 36000, 90000},
	{"1280x720@48Hz 64:27", 48, 36000, 90000},
	{"1680x720@48Hz 64:27", 48, 36000, 99000},
	/* VIC 111 */
	{"1920x1080@48Hz 16:9", 48, 54000, 148500},
	{"1920x1080@48Hz 64:27", 48, 54000, 148500},
	{"2560x1080@48Hz 64:27", 48, 52800, 198000},
	{"3840x2160@48Hz 16:9", 48, 108000, 594000},
	{"4096x2160@48Hz 256:135", 48, 108000, 594000},
	{"3840x2160@48Hz 64:27", 48, 108000, 594000},
	{"3840x2160@100Hz 16:9", 100, 225000, 1188000},
	{"3840x2160@120Hz 16:9", 120, 270000, 1188000},
	{"3840x2160@100Hz 64:27", 100, 225000, 1188000},
	{"3840x2160@120Hz 64:27", 120, 270000, 1188000},
	/* VIC 121 */
	{"5120x2160@24Hz 64:27", 24, 52800, 396000},
	{"5120x2160@25Hz 64:27", 25, 55000, 396000},
	{"5120x2160@30Hz 64:27", 30, 66000, 396000},
	{"5120x2160@48Hz 64:27", 48, 118800, 742500},
	{"5120x2160@50Hz 64:27", 50, 112500, 742500},
	{"5120x2160@60Hz 64:27", 60, 135000, 742500},
	{"5120x2160@100Hz 64:27", 100, 225000, 1485000},
};

static struct edid_cta_mode edid_cta_modes2[] = {
	/* VIC 193 */
	{"5120x2160@120Hz 64:27", 120, 270000, 1485000},
	{"7680x4320@24Hz 16:9", 24, 108000, 1188000},
	{"7680x4320@25Hz 16:9", 25, 110000, 1188000},
	{"7680x4320@30Hz 16:9", 30, 132000, 1188000},
	{"7680x4320@48Hz 16:9", 48, 216000, 2376000},
	{"7680x4320@50Hz 16:9", 50, 220000, 2376000},
	{"7680x4320@60Hz 16:9", 60, 264000, 2376000},
	{"7680x4320@100Hz 16:9", 100, 450000, 4752000},
	/* VIC 201 */
	{"7680x4320@120Hz 16:9", 120, 540000, 4752000},
	{"7680x4320@24Hz 64:27", 24, 108000, 1188000},
	{"7680x4320@25Hz 64:27", 25, 110000, 1188000},
	{"7680x4320@30Hz 64:27", 30, 132000, 1188000},
	{"7680x4320@48Hz 64:27", 48, 216000, 2376000},
	{"7680x4320@50Hz 64:27", 50, 220000, 2376000},
	{"7680x4320@60Hz 64:27", 60, 264000, 2376000},
	{"7680x4320@100Hz 64:27", 100, 450000, 4752000},
	{"7680x4320@120Hz 64:27", 120, 540000, 4752000},
	{"10240x4320@24Hz 64:27", 24, 118800, 1485000},
	/* VIC 211 */
	{"10240x4320@25Hz 64:27", 25, 110000, 1485000},
	{"10240x4320@30Hz 64:27", 30, 135000, 1485000},
	{"10240x4320@48Hz 64:27", 48, 237600, 2970000},
	{"10240x4320@50Hz 64:27", 50, 220000, 2970000},
	{"10240x4320@60Hz 64:27", 60, 270000, 2970000},
	{"10240x4320@100Hz 64:27", 100, 450000, 5940000},
	{"10240x4320@120Hz 64:27", 120, 540000, 5940000},
	{"4096x2160@100Hz 256:135", 100, 225000, 1188000},
	{"4096x2160@120Hz 256:135", 120, 270000, 1188000},
};

static const struct edid_cta_mode *vic_to_mode(unsigned char vic)
{
	if (vic > 0 && vic <= ARRAY_SIZE(edid_cta_modes1))
		return edid_cta_modes1 + vic - 1;
	if (vic >= 193 && vic <= ARRAY_SIZE(edid_cta_modes2) + 193)
		return edid_cta_modes2 + vic - 193;
	return NULL;
}

static void cta_svd(const unsigned char *x, int n, int for_ycbcr420)
{
	int i;

	for (i = 0; i < n; i++)  {
		const struct edid_cta_mode *vicmode = NULL;
		unsigned char svd = x[i];
		unsigned char native;
		unsigned char vic;
		const char *mode;
		unsigned hfreq = 0;
		unsigned clock_khz = 0;

		if ((svd & 0x7f) == 0)
			continue;

		if ((svd - 1) & 0x40) {
			vic = svd;
			native = 0;
		} else {
			vic = svd & 0x7f;
			native = svd & 0x80;
		}

		vicmode = vic_to_mode(vic);
		if (vicmode) {
			switch (vic) {
			case 95:
				supported_hdmi_vic_vsb_codes |= 1 << 0;
				break;
			case 94:
				supported_hdmi_vic_vsb_codes |= 1 << 1;
				break;
			case 93:
				supported_hdmi_vic_vsb_codes |= 1 << 2;
				break;
			case 98:
				supported_hdmi_vic_vsb_codes |= 1 << 3;
				break;
			}
			mode = vicmode->name;
			min_vert_freq_hz = min(min_vert_freq_hz, vicmode->refresh);
			max_vert_freq_hz = max(max_vert_freq_hz, vicmode->refresh);
			hfreq = vicmode->hor_freq_hz;
			min_hor_freq_hz = min(min_hor_freq_hz, hfreq);
			max_hor_freq_hz = max(max_hor_freq_hz, hfreq);
			clock_khz = vicmode->pixclk_khz / (for_ycbcr420 ? 2 : 1);
			max_pixclk_khz = max(max_pixclk_khz, clock_khz);
		} else {
			mode = "Unknown mode";
		}

		printf("    VIC %3d %s %s HorFreq: %d Hz Clock: %.3f MHz\n",
		       vic, mode, native ? "(native)" : "", hfreq, clock_khz / 1000.0);
		if (vic == 1)
			has_cta861_vic_1 = 1;
	}
}

static void cta_video_block(const unsigned char *x, unsigned int length)
{
	cta_svd(x, length, 0);
}

static void cta_y420vdb(const unsigned char *x, unsigned int length)
{
	cta_svd(x, length, 1);
}

static void cta_y420cmdb(const unsigned char *x, unsigned int length)
{
	int i;

	for (i = 0; i < length; i++) {
		uint8_t v = x[0 + i];
		int j;

		for (j = 0; j < 8; j++)
			if (v & (1 << j))
				printf("    VSD Index %d\n", i * 8 + j);
	}
}

static void cta_vfpdb(const unsigned char *x, unsigned int length)
{
	int i;

	for (i = 0; i < length; i++)  {
		unsigned char svr = x[i];

		if ((svr > 0 && svr < 128) || (svr > 192 && svr < 254)) {
			const struct edid_cta_mode *vicmode;
			unsigned char vic;
			const char *mode;

			vic = svr;

			vicmode = vic_to_mode(vic);
			if (vicmode)
				mode = vicmode->name;
			else
				mode = "Unknown mode";

			printf("    VIC %02d %s\n", vic, mode);

		} else if (svr > 128 && svr < 145) {
			printf("    DTD number %02d\n", svr - 128);
		}
	}
}

static struct {
	const char *name;
	int refresh, hor_freq_hz, pixclk_khz;
} edid_hdmi_modes[] = {
	{"3840x2160@30Hz 16:9", 30, 67500, 297000},
	{"3840x2160@25Hz 16:9", 25, 56250, 297000},
	{"3840x2160@24Hz 16:9", 24, 54000, 297000},
	{"4096x2160@24Hz 256:135", 24, 54000, 297000},
};

static void cta_hdmi_block(const unsigned char *x, unsigned int length)
{
	int mask = 0, formats = 0;
	int len_vic, len_3d;
	int b = 0;

	printf(" (HDMI)\n");
	printf("    Source physical address %d.%d.%d.%d\n", x[3] >> 4, x[3] & 0x0f,
	       x[4] >> 4, x[4] & 0x0f);

	if (length < 6)
		return;

	if (x[5] & 0x80)
		printf("    Supports_AI\n");
	if (x[5] & 0x40)
		printf("    DC_48bit\n");
	if (x[5] & 0x20)
		printf("    DC_36bit\n");
	if (x[5] & 0x10)
		printf("    DC_30bit\n");
	if (x[5] & 0x08)
		printf("    DC_Y444\n");
	/* two reserved */
	if (x[5] & 0x01)
		printf("    DVI_Dual\n");

	if (length < 7)
		return;

	printf("    Maximum TMDS clock: %dMHz\n", x[6] * 5);
	if (x[6] * 5 > 340)
		nonconformant_hdmi_vsdb_tmds_rate = 1;

	/* XXX the walk here is really ugly, and needs to be length-checked */
	if (length < 8)
		return;

	if (x[7] & 0x0f) {
		printf("    Supported Content Types:\n");
		if (x[7] & 0x01)
			printf("      Graphics\n");
		if (x[7] & 0x02)
			printf("      Photo\n");
		if (x[7] & 0x04)
			printf("      Cinema\n");
		if (x[7] & 0x08)
			printf("      Game\n");
	}

	if (x[7] & 0x80) {
		printf("    Video latency: %d\n", x[8 + b]);
		printf("    Audio latency: %d\n", x[9 + b]);
		b += 2;

		if (x[7] & 0x40) {
			printf("    Interlaced video latency: %d\n", x[8 + b]);
			printf("    Interlaced audio latency: %d\n", x[9 + b]);
			b += 2;
		}
	}

	if (!(x[7] & 0x20))
		return;

	printf("    Extended HDMI video details:\n");
	if (x[8 + b] & 0x80)
		printf("      3D present\n");
	if ((x[8 + b] & 0x60) == 0x20) {
		printf("      All advertised VICs are 3D-capable\n");
		formats = 1;
	}
	if ((x[8 + b] & 0x60) == 0x40) {
		printf("      3D-capable-VIC mask present\n");
		formats = 1;
		mask = 1;
	}
	switch (x[8 + b] & 0x18) {
	case 0x00: break;
	case 0x08:
		   printf("      Base EDID image size is aspect ratio\n");
		   break;
	case 0x10:
		   printf("      Base EDID image size is in units of 1cm\n");
		   break;
	case 0x18:
		   printf("      Base EDID image size is in units of 5cm\n");
		   break;
	}
	len_vic = (x[9 + b] & 0xe0) >> 5;
	len_3d = (x[9 + b] & 0x1f) >> 0;
	b += 2;

	if (len_vic) {
		unsigned hfreq = 0;
		unsigned clock_khz = 0;
		int i;

		for (i = 0; i < len_vic; i++) {
			unsigned char vic = x[8 + b + i];
			const char *mode;

			if (vic && vic <= ARRAY_SIZE(edid_hdmi_modes)) {
				supported_hdmi_vic_codes |= 1 << (vic - 1);
				mode = edid_hdmi_modes[vic - 1].name;
				min_vert_freq_hz = min(min_vert_freq_hz, edid_hdmi_modes[vic - 1].refresh);
				max_vert_freq_hz = max(max_vert_freq_hz, edid_hdmi_modes[vic - 1].refresh);
				hfreq = edid_hdmi_modes[vic - 1].hor_freq_hz;
				min_hor_freq_hz = min(min_hor_freq_hz, hfreq);
				max_hor_freq_hz = max(max_hor_freq_hz, hfreq);
				clock_khz = edid_hdmi_modes[vic - 1].pixclk_khz;
				max_pixclk_khz = max(max_pixclk_khz, clock_khz);
			} else {
				mode = "Unknown mode";
			}

			printf("      HDMI VIC %d %s HorFreq: %d Hz Clock: %.3f MHz\n",
			       vic, mode, hfreq, clock_khz / 1000.0);
		}

		b += len_vic;
	}

	if (len_3d) {
		if (formats) {
			/* 3D_Structure_ALL_15..8 */
			if (x[8 + b] & 0x80)
				printf("      3D: Side-by-side (half, quincunx)\n");
			if (x[8 + b] & 0x01)
				printf("      3D: Side-by-side (half, horizontal)\n");
			/* 3D_Structure_ALL_7..0 */
			if (x[9 + b] & 0x40)
				printf("      3D: Top-and-bottom\n");
			if (x[9 + b] & 0x20)
				printf("      3D: L + depth + gfx + gfx-depth\n");
			if (x[9 + b] & 0x10)
				printf("      3D: L + depth\n");
			if (x[9 + b] & 0x08)
				printf("      3D: Side-by-side (full)\n");
			if (x[9 + b] & 0x04)
				printf("      3D: Line-alternative\n");
			if (x[9 + b] & 0x02)
				printf("      3D: Field-alternative\n");
			if (x[9 + b] & 0x01)
				printf("      3D: Frame-packing\n");
			b += 2;
			len_3d -= 2;
		}
		if (mask) {
			int i;
			printf("      3D VIC indices:");
			/* worst bit ordering ever */
			for (i = 0; i < 8; i++)
				if (x[9 + b] & (1 << i))
					printf(" %d", i);
			for (i = 0; i < 8; i++)
				if (x[8 + b] & (1 << i))
					printf(" %d", i + 8);
			printf("\n");
			b += 2;
			len_3d -= 2;
		}

		/*
		 * list of nibbles:
		 * 2D_VIC_Order_X
		 * 3D_Structure_X
		 * (optionally: 3D_Detail_X and reserved)
		 */
		if (len_3d > 0) {
			int end = b + len_3d;

			while (b < end) {
				printf("      VIC index %d supports ", x[8 + b] >> 4);
				switch (x[8 + b] & 0x0f) {
				case 0: printf("frame packing"); break;
				case 6: printf("top-and-bottom"); break;
				case 8:
					if ((x[9 + b] >> 4) == 1) {
						printf("side-by-side (half, horizontal)");
						break;
					}
				default: printf("unknown");
				}
				printf("\n");

				if ((x[8 + b] & 0x0f) > 7) {
					/* Optional 3D_Detail_X and reserved */
					b++;
				}
				b++;
			}
		}
	}
}

static const char *max_frl_rates[] = {
	"Not Supported",
	"3 Gbps per lane on 3 lanes",
	"3 and 6 Gbps per lane on 3 lanes",
	"3 and 6 Gbps per lane on 3 lanes, 6 Gbps on 4 lanes",
	"3 and 6 Gbps per lane on 3 lanes, 6 and 8 Gbps on 4 lanes",
	"3 and 6 Gbps per lane on 3 lanes, 6, 8 and 10 Gbps on 4 lanes",
	"3 and 6 Gbps per lane on 3 lanes, 6, 8, 10 and 12 Gbps on 4 lanes",
};

static const char *dsc_max_slices[] = {
	"Not Supported",
	"up to 1 slice and up to (340 MHz/Ksliceadjust) pixel clock per slice",
	"up to 2 slices and up to (340 MHz/Ksliceadjust) pixel clock per slice",
	"up to 4 slices and up to (340 MHz/Ksliceadjust) pixel clock per slice",
	"up to 8 slices and up to (340 MHz/Ksliceadjust) pixel clock per slice",
	"up to 8 slices and up to (400 MHz/Ksliceadjust) pixel clock per slice",
	"up to 12 slices and up to (400 MHz/Ksliceadjust) pixel clock per slice",
	"up to 16 slices and up to (400 MHz/Ksliceadjust) pixel clock per slice",
};

static void cta_hf_eeodb(const unsigned char *x, unsigned int length)
{
	printf("    EDID Extension Block Count: %u\n", x[0]);
	if (length != 1 || x[0] == 0)
		nonconformant_hf_eeodb = 1;
}

static void cta_hf_scdb(const unsigned char *x, unsigned int length)
{
	unsigned rate = x[1] * 5;

	printf("    Version: %u\n", x[0]);
	if (rate) {
		printf("    Maximum TMDS Character Rate: %uMHz\n", rate);
		if ((rate && rate <= 340) || rate > 600)
			nonconformant_hf_vsdb_tmds_rate = 1;
	}
	if (x[2] & 0x80)
		printf("    SCDC Present\n");
	if (x[2] & 0x40)
		printf("    SCDC Read Request Capable\n");
	if (x[2] & 0x10)
		printf("    Supports Color Content Bits Per Component Indication\n");
	if (x[2] & 0x08)
		printf("    Supports scrambling for <= 340 Mcsc\n");
	if (x[2] & 0x04)
		printf("    Supports 3D Independent View signaling\n");
	if (x[2] & 0x02)
		printf("    Supports 3D Dual View signaling\n");
	if (x[2] & 0x01)
		printf("    Supports 3D OSD Disparity signaling\n");
	if (x[3] & 0xf0) {
		unsigned max_frl_rate = x[3] >> 4;

		printf("    Max Fixed Rate Link: ");
		if (max_frl_rate >= ARRAY_SIZE(max_frl_rates))
			printf("Reserved\n");
		else
			printf("%s\n", max_frl_rates[max_frl_rate]);
		if (max_frl_rate == 1 && rate < 300)
			nonconformant_hf_vsdb_tmds_rate = 1;
		else if (max_frl_rate >= 2 && rate < 600)
			nonconformant_hf_vsdb_tmds_rate = 1;
	}
	if (x[3] & 0x08)
		printf("    Supports UHD VIC\n");
	if (x[3] & 0x04)
		printf("    Supports 16-bits/component Deep Color 4:2:0 Pixel Encoding\n");
	if (x[3] & 0x02)
		printf("    Supports 12-bits/component Deep Color 4:2:0 Pixel Encoding\n");
	if (x[3] & 0x01)
		printf("    Supports 10-bits/component Deep Color 4:2:0 Pixel Encoding\n");

	if (length <= 7)
		return;

	if (x[4] & 0x20)
		printf("    Supports Mdelta\n");
	if (x[4] & 0x10)
		printf("    Supports media rates below VRRmin (CinemaVRR)\n");
	if (x[4] & 0x08)
		printf("    Supports negative Mvrr values\n");
	if (x[4] & 0x04)
		printf("    Supports Fast Vactive\n");
	if (x[4] & 0x02)
		printf("    Supports Auto Low-Latency Mode\n");
	if (x[4] & 0x01)
		printf("    Supports a FAPA in blanking after first active video line\n");

	if (length <= 8)
		return;

	printf("    VRRmin: %d Hz\n", x[8] & 0x3f);
	printf("    VRRmax: %d Hz\n", (x[8] & 0xc0) << 2 | x[9]);

	if (length <= 10)
		return;

	if (x[7] & 0x80)
		printf("    Supports VESA DSC 1.2a compression\n");
	if (x[7] & 0x40)
		printf("    Supports Compressed Video Transport for 4:2:0 Pixel Encoding\n");
	if (x[7] & 0x08)
		printf("    Supports Compressed Video Transport at any valid 1/16th bit bpp\n");
	if (x[7] & 0x04)
		printf("    Supports 16 bpc Compressed Video Transport\n");
	if (x[7] & 0x02)
		printf("    Supports 12 bpc Compressed Video Transport\n");
	if (x[7] & 0x01)
		printf("    Supports 10 bpc Compressed Video Transport\n");
	if (x[8] & 0xf) {
		unsigned max_slices = x[8] & 0xf;

		if (max_slices < ARRAY_SIZE(dsc_max_slices))
			printf("    Supports %s\n", dsc_max_slices[max_slices]);
	}
	if (x[8] & 0xf0) {
		unsigned max_frl_rate = x[8] >> 4;

		printf("    DSC Max Fixed Rate Link: ");
		if (max_frl_rate >= ARRAY_SIZE(max_frl_rates))
			printf("Reserved\n");
		else
			printf("%s\n", max_frl_rates[max_frl_rate]);
	}
	if (x[9] & 0x3f)
		printf("    Maximum number of bytes in a line of chunks: %u\n",
		       1024 * (1 + (x[9] & 0x3f)));
}

static void cta_hdr10plus(const unsigned char *x, unsigned int length)
{
	printf("    Application Version: %u\n", x[0]);
}

DEFINE_FIELD("YCbCr quantization", YCbCr_quantization, 7, 7,
	     { 0, "No Data" },
	     { 1, "Selectable (via AVI YQ)" });
DEFINE_FIELD("RGB quantization", RGB_quantization, 6, 6,
	     { 0, "No Data" },
	     { 1, "Selectable (via AVI Q)" });
DEFINE_FIELD("PT scan behaviour", PT_scan, 4, 5,
	     { 0, "No Data" },
	     { 1, "Always Overscannned" },
	     { 2, "Always Underscanned" },
	     { 3, "Support both over- and underscan" });
DEFINE_FIELD("IT scan behaviour", IT_scan, 2, 3,
	     { 0, "IT video formats not supported" },
	     { 1, "Always Overscannned" },
	     { 2, "Always Underscanned" },
	     { 3, "Support both over- and underscan" });
DEFINE_FIELD("CE scan behaviour", CE_scan, 0, 1,
	     { 0, "CE video formats not supported" },
	     { 1, "Always Overscannned" },
	     { 2, "Always Underscanned" },
	     { 3, "Support both over- and underscan" });

static struct field *vcdb_fields[] = {
	&YCbCr_quantization,
	&RGB_quantization,
	&PT_scan,
	&IT_scan,
	&CE_scan,
};

static const char *speaker_map[] = {
	"FL/FR - Front Left/Right",
	"LFE1 - Low Frequency Effects 1",
	"FC - Front Center",
	"BL/BR - Back Left/Right",
	"BC - Back Center",
	"FLc/FRc - Front Left/Right of Center",
	"RLC/RRC - Rear Left/Right of Center (Deprecated)",
	"FLw/FRw - Front Left/Right Wide",
	"TpFL/TpFR - Top Front Left/Right",
	"TpC - Top Center",
	"TpFC - Top Front Center",
	"LS/RS - Left/Right Surround",
	"LFE2 - Low Frequency Effects 2",
	"TpBC - Top Back Center",
	"SiL/SiR - Side Left/Right",
	"TpSiL/TpSiR - Top Side Left/Right",
	"TpBL/TpBR - Top Back Left/Right",
	"BtFC - Bottom Front Center",
	"BtFL/BtFR - Bottom Front Left/Right",
	"TpLS/TpRS - Top Left/Right Surround (Deprecated for CTA-861)",
	"LSd/RSd - Left/Right Surround Direct (HDMI only)",
};

static void cta_sadb(const unsigned char *x, unsigned int length)
{
	uint32_t sad;
	int i;

	if (length < 3)
		return;

	sad = ((x[2] << 16) | (x[1] << 8) | x[0]);

	printf("    Speaker map:\n");

	for (i = 0; i < ARRAY_SIZE(speaker_map); i++) {
		if ((sad >> i) & 1)
			printf("      %s\n", speaker_map[i]);
	}
}

static float decode_uchar_as_float(unsigned char x)
{
	signed char s = (signed char)x;

	return s / 64.0;
}

static void cta_rcdb(const unsigned char *x, unsigned int length)
{
	uint32_t spm = ((x[3] << 16) | (x[2] << 8) | x[1]);
	int i;

	if (length < 4)
		return;

	if (x[0] & 0x40)
		printf("    Speaker count: %d\n", (x[0] & 0x1f) + 1);

	printf("    Speaker Presence Mask:\n");
	for (i = 0; i < ARRAY_SIZE(speaker_map); i++) {
		if ((spm >> i) & 1)
			printf("      %s\n", speaker_map[i]);
	}
	if ((x[0] & 0x20) && length >= 7) {
		printf("    Xmax: %d dm\n", x[4]);
		printf("    Ymax: %d dm\n", x[5]);
		printf("    Zmax: %d dm\n", x[6]);
	}
	if ((x[0] & 0x80) && length >= 10) {
		printf("    DisplayX: %.3f * Xmax\n", decode_uchar_as_float(x[7]));
		printf("    DisplayY: %.3f * Ymax\n", decode_uchar_as_float(x[8]));
		printf("    DisplayZ: %.3f * Zmax\n", decode_uchar_as_float(x[9]));
	}
}

static const char *speaker_location[] = {
	"FL - Front Left",
	"FR - Front Right",
	"FC - Front Center",
	"LFE1 - Low Frequency Effects 1",
	"BL - Back Left",
	"BR - Back Right",
	"FLC - Front Left of Center",
	"FRC - Front Right of Center",
	"BC - Back Center",
	"LFE2 - Low Frequency Effects 2",
	"SiL - Side Left",
	"SiR - Side Right",
	"TpFL - Top Front Left",
	"TpFR - Top Front Right",
	"TpFC - Top Front Center",
	"TpC - Top Center",
	"TpBL - Top Back Left",
	"TpBR - Top Back Right",
	"TpSiL - Top Side Left",
	"TpSiR - Top Side Right",
	"TpBC - Top Back Center",
	"BtFC - Bottom Front Center",
	"BtFL - Bottom Front Left",
	"BtFR - Bottom Front Right",
	"FLW - Front Left Wide",
	"FRW - Front Right Wide",
	"LS - Left Surround",
	"RS - Right Surround",
};

static void cta_sldb(const unsigned char *x, unsigned int length)
{
	while (length >= 2) {
		printf("    Channel: %d (%sactive)\n", x[0] & 0x1f,
		       (x[0] & 0x20) ? "" : "not ");
		if ((x[1] & 0x1f) < ARRAY_SIZE(speaker_location))
			printf("      Speaker: %s\n", speaker_location[x[1] & 0x1f]);
		if (length >= 5 && (x[0] & 0x40)) {
			printf("      X: %.3f * Xmax\n", decode_uchar_as_float(x[2]));
			printf("      Y: %.3f * Ymax\n", decode_uchar_as_float(x[3]));
			printf("      Z: %.3f * Zmax\n", decode_uchar_as_float(x[4]));
			length -= 3;
			x += 3;
		}

		length -= 2;
		x += 2;
	}
}

static void cta_vcdb(const unsigned char *x, unsigned int length)
{
	unsigned char d = x[0];

	decode(vcdb_fields, d, "    ");
}

static const char *colorimetry_map[] = {
	"xvYCC601",
	"xvYCC709",
	"sYCC601",
	"opYCC601",
	"opRGB",
	"BT2020cYCC",
	"BT2020YCC",
	"BT2020RGB",
};

static void cta_colorimetry_block(const unsigned char *x, unsigned int length)
{
	int i;

	if (length >= 2) {
		for (i = 0; i < ARRAY_SIZE(colorimetry_map); i++) {
			if (x[0] & (1 << i))
				printf("    %s\n", colorimetry_map[i]);
		}
		if (x[1] & 0x80)
			printf("    DCI-P3\n");
		if (x[1] & 0x40)
			printf("    ICtCp\n");
	}
}

static const char *eotf_map[] = {
	"Traditional gamma - SDR luminance range",
	"Traditional gamma - HDR luminance range",
	"SMPTE ST2084",
	"Hybrid Log-Gamma",
};

static void cta_hdr_static_metadata_block(const unsigned char *x, unsigned int length)
{
	int i;

	if (length >= 2) {
		printf("    Electro optical transfer functions:\n");
		for (i = 0; i < 6; i++) {
			if (x[0] & (1 << i)) {
				printf("      %s\n", i < ARRAY_SIZE(eotf_map) ?
				       eotf_map[i] : "Unknown");
			}
		}
		printf("    Supported static metadata descriptors:\n");
		for (i = 0; i < 8; i++) {
			if (x[1] & (1 << i))
				printf("      Static metadata type %d\n", i + 1);
		}
	}

	if (length >= 3)
		printf("    Desired content max luminance: %d (%.3f cd/m^2)\n",
		       x[2], 50.0 * pow(2, x[2] / 32.0));

	if (length >= 4)
		printf("    Desired content max frame-average luminance: %d (%.3f cd/m^2)\n",
		       x[3], 50.0 * pow(2, x[3] / 32.0));

	if (length >= 5)
		printf("    Desired content min luminance: %d (%.3f cd/m^2)\n",
		       x[4], (50.0 * pow(2, x[2] / 32.0)) * pow(x[4] / 255.0, 2) / 100.0);
}

static void cta_hdr_dyn_metadata_block(const unsigned char *x, unsigned int length)
{
	while (length >= 3) {
		int type_len = x[0];
		int type = x[1] | (x[2] << 8);

		if (length < type_len + 1)
			return;
		printf("    HDR Dynamic Metadata Type %d\n", type);
		switch (type) {
		case 1:
		case 2:
		case 4:
			if (type_len > 2)
				printf("      Version: %d\n", x[3] & 0xf);
			break;
		default:
			break;
		}
		length -= type_len + 1;
		x += type_len + 1;
	}
}

static void cta_ifdb(const unsigned char *x, unsigned int length)
{
	int len_hdr = x[0] >> 5;

	if (length < 2)
		return;
	printf("    VSIFs: %d\n", x[1]);
	if (length < len_hdr + 2)
		return;
	length -= len_hdr + 2;
	x += len_hdr + 2;
	while (length > 0) {
		int payload_len = x[0] >> 5;

		if ((x[0] & 0x1f) == 1 && length >= 4) {
			printf("    InfoFrame Type Code %d IEEE OUI: %02x%02x%02x\n",
			       x[0] & 0x1f, x[3], x[2], x[1]);
			x += 4;
			length -= 4;
		} else {
			printf("    InfoFrame Type Code %d\n", x[0] & 0x1f);
			x++;
			length--;
		}
		x += payload_len;
		length -= payload_len;
	}
}

static void cta_hdmi_audio_block(const unsigned char *x, unsigned int length)
{
	int num_descs;

	if (length < 2)
		return;
	if (x[0] & 3)
		printf("    Max Stream Count: %d\n", (x[0] & 3) + 1);
	if (x[0] & 4)
		printf("    Supports MS NonMixed\n");

	num_descs = x[1] & 7;
	if (num_descs == 0)
		return;
	length -= 2;
	x += 2;
	while (length >= 4) {
		if (length > 4) {
			int format = x[0] & 0xf;

			printf("    %s, max channels %d\n", audio_format(format),
			       (x[1] & 0x1f)+1);
			printf("      Supported sample rates (kHz):%s%s%s%s%s%s%s\n",
			       (x[2] & 0x40) ? " 192" : "",
			       (x[2] & 0x20) ? " 176.4" : "",
			       (x[2] & 0x10) ? " 96" : "",
			       (x[2] & 0x08) ? " 88.2" : "",
			       (x[2] & 0x04) ? " 48" : "",
			       (x[2] & 0x02) ? " 44.1" : "",
			       (x[2] & 0x01) ? " 32" : "");
			if (format == 1)
				printf("      Supported sample sizes (bits):%s%s%s\n",
				       (x[3] & 0x04) ? " 24" : "",
				       (x[3] & 0x02) ? " 20" : "",
				       (x[3] & 0x01) ? " 16" : "");
		} else {
			uint32_t sad = ((x[2] << 16) | (x[1] << 8) | x[0]);
			int i;

			switch (x[3] >> 4) {
			case 1:
				printf("    Speaker Allocation for 10.2 channels:\n");
				break;
			case 2:
				printf("    Speaker Allocation for 22.2 channels:\n");
				break;
			case 3:
				printf("    Speaker Allocation for 30.2 channels:\n");
				break;
			default:
				printf("    Unknown Speaker Allocation (%d)\n", x[3] >> 4);
				return;
			}

			for (i = 0; i < ARRAY_SIZE(speaker_map); i++) {
				if ((sad >> i) & 1)
					printf("      %s\n", speaker_map[i]);
			}
		}
		length -= 4;
		x += 4;
	}
}

static void cta_block(const unsigned char *x)
{
	static int last_block_was_hdmi_vsdb;
	static int have_hf_vsdb, have_hf_scdb;
	static int first_block = 1;
	unsigned int length = x[0] & 0x1f;
	unsigned int oui;

	switch ((x[0] & 0xe0) >> 5) {
	case 0x01:
		cur_block = "Audio Data Block";
		printf("  Audio Data Block\n");
		cta_audio_block(x + 1, length);
		break;
	case 0x02:
		cur_block = "Video Data Block";
		printf("  Video Data Block\n");
		cta_video_block(x + 1, length);
		break;
	case 0x03:
		oui = (x[3] << 16) + (x[2] << 8) + x[1];
		printf("  Vendor-Specific Data Block, OUI %06x", oui);
		if (oui == 0x000c03) {
			cur_block = "Vendor-Specific Data Block (HDMI)";
			cta_hdmi_block(x + 1, length);
			last_block_was_hdmi_vsdb = 1;
			first_block = 0;
			return;
		}
		if (oui == 0xc45dd8) {
			cur_block = "Vendor-Specific Data Block (HDMI Forum)";
			if (!last_block_was_hdmi_vsdb)
				nonconformant_hf_vsdb_position = 1;
			if (have_hf_scdb || have_hf_vsdb)
				duplicate_scdb = 1;
			printf(" (HDMI Forum)\n");
			cta_hf_scdb(x + 4, length - 3);
			have_hf_vsdb = 1;
		} else {
			printf("\n");
		}
		break;
	case 0x04:
		cur_block = "Speaker Allocation Data Block";
		printf("  Speaker Allocation Data Block\n");
		cta_sadb(x + 1, length);
		break;
	case 0x05:
		printf("  VESA DTC Data Block\n");
		break;
	case 0x07:
		printf("  Extended tag: ");
		switch (x[1]) {
		case 0x00:
			cur_block = "Video Capability Data Block";
			printf("Video Capability Data Block\n");
			cta_vcdb(x + 2, length - 1);
			break;
		case 0x01:
			oui = (x[4] << 16) + (x[3] << 8) + x[2];
			printf("Vendor-Specific Video Data Block, OUI %06x", oui);
			if (oui == 0x90848b) {
				cur_block = "Vendor-Specific Video Data Block (HDR10+)";
				printf(" (HDR10+)\n");
				cta_hdr10plus(x + 5, length - 4);
			} else {
				printf("\n");
			}
			break;
		case 0x02:
			printf("VESA Video Display Device Data Block\n");
			break;
		case 0x03:
			printf("VESA Video Timing Block Extension\n");
			break;
		case 0x04:
			printf("Reserved for HDMI Video Data Block\n");
			break;
		case 0x05:
			cur_block = "Colorimetry Data Block";
			printf("Colorimetry Data Block\n");
			cta_colorimetry_block(x + 2, length - 1);
			break;
		case 0x06:
			cur_block = "HDR Static Metadata Data Block";
			printf("HDR Static Metadata Data Block\n");
			cta_hdr_static_metadata_block(x + 2, length - 1);
			break;
		case 0x07:
			cur_block = "HDR Dynamic Metadata Data Block";
			printf("HDR Dynamic Metadata Data Block\n");
			cta_hdr_dyn_metadata_block(x + 2, length - 1);
			break;
		case 0x0d:
			cur_block = "Video Format Preference Data Block";
			printf("Video Format Preference Data Block\n");
			cta_vfpdb(x + 2, length - 1);
			break;
		case 0x0e:
			cur_block = "YCbCr 4:2:0 Video Data Block";
			printf("YCbCr 4:2:0 Video Data Block\n");
			cta_y420vdb(x + 2, length - 1);
			break;
		case 0x0f:
			cur_block = "YCbCr 4:2:0 Capability Map Data Block";
			printf("YCbCr 4:2:0 Capability Map Data Block\n");
			cta_y420cmdb(x + 2, length - 1);
			break;
		case 0x10:
			printf("Reserved for CTA Miscellaneous Audio Fields\n");
			break;
		case 0x11:
			printf("Vendor-Specific Audio Data Block\n");
			break;
		case 0x12:
			cur_block = "HDMI Audio Data Block";
			printf("HDMI Audio Data Block\n");
			cta_hdmi_audio_block(x + 2, length - 1);
			break;
		case 0x13:
			cur_block = "Room Configuration Data Block";
			printf("Room Configuration Data Block\n");
			cta_rcdb(x + 2, length - 1);
			break;
		case 0x14:
			cur_block = "Speaker Location Data Block";
			printf("Speaker Location Data Block\n");
			cta_sldb(x + 2, length - 1);
			break;
		case 0x20:
			printf("InfoFrame Data Block\n");
			cta_ifdb(x + 2, length - 1);
			break;
		case 0x78:
			cur_block = "HDMI Forum EDID Extension Override Data Block";
			printf("HDMI Forum EDID Extension Override Data Block\n");
			cta_hf_eeodb(x + 2, length - 1);
			// This must be the first CTA block
			if (!first_block)
				nonconformant_hf_eeodb = 1;
			break;
		case 0x79:
			cur_block = "HDMI Forum Sink Capability Data Block";
			printf("HDMI Forum Sink Capability Data Block\n");
			if (!last_block_was_hdmi_vsdb)
				nonconformant_hf_vsdb_position = 1;
			if (have_hf_scdb || have_hf_vsdb)
				duplicate_scdb = 1;
			if (x[2] || x[3])
				printf("  Non-zero SCDB reserved fields!\n");
			cta_hf_scdb(x + 4, length - 3);
			have_hf_scdb = 1;
			break;
		default:
			if (x[1] >= 6 && x[1] <= 12)
				printf("Reserved for video-related blocks (%02x)\n", x[1]);
			else if (x[1] >= 19 && x[1] <= 31)
				printf("Reserved for audio-related blocks (%02x)\n", x[1]);
			else
				printf("Reserved (%02x)\n", x[1]);
			break;
		}
		break;
	default: {
		int tag = (*x & 0xe0) >> 5;
		int length = *x & 0x1f;
		printf("  Unknown tag %d, length %d (raw %02x)\n", tag, length, *x);
		break;
	}
	}
	first_block = 0;
	last_block_was_hdmi_vsdb = 0;
}

static int parse_cta(const unsigned char *x)
{
	int ret = 0;
	int version = x[1];
	int offset = x[2];
	const unsigned char *detailed;

	cur_block = "CTA-861";

	if (version >= 1) do {
		if (version == 1 && x[3] != 0)
			ret = 1;

		if (offset < 4)
			break;

		if (version < 3) {
			printf("%d 8-byte timing descriptors\n\n", (offset - 4) / 8);
			if (offset - 4 > 0)
				/* do stuff */ ;
		}

		if (version >= 2) {    
			if (x[3] & 0x80)
				printf("Underscans PC formats by default\n");
			if (x[3] & 0x40)
				printf("Basic audio support\n");
			if (x[3] & 0x20)
				printf("Supports YCbCr 4:4:4\n");
			if (x[3] & 0x10)
				printf("Supports YCbCr 4:2:2\n");
			printf("%d native detailed modes\n\n", x[3] & 0x0f);
		}
		if (version == 3) {
			int i;

			printf("%d bytes of CTA data\n", offset - 4);
			for (i = 4; i < offset; i += (x[i] & 0x1f) + 1) {
				cta_block(x + i);
			}
			printf("\n");
		}

		for (detailed = x + offset; detailed + 18 < x + 127; detailed += 18)
			if (detailed[0])
				detailed_block(detailed, 1);
	} while (0);

	has_valid_cta_checksum = do_checksum(x, EDID_PAGE_SIZE);
	has_cta861 = 1;
	nonconformant_cta861_640x480 = !has_cta861_vic_1 && !has_640x480p60_est_timing;

	return ret;
}

static void parse_displayid_detailed_timing(const unsigned char *x)
{
	int ha, hbl, hso, hspw;
	int va, vbl, vso, vspw;
	char phsync, pvsync, *stereo;
	int pix_clock;
	char *aspect;

	switch (x[3] & 0xf) {
	case 0:
		aspect = "1:1";
		break;
	case 1:
		aspect = "5:4";
		break;
	case 2:
		aspect = "4:3";
		break;
	case 3:
		aspect = "15:9";
		break;
	case 4:
		aspect = "16:9";
		break;
	case 5:
		aspect = "16:10";
		break;
	case 6:
		aspect = "64:27";
		break;
	case 7:
		aspect = "256:135";
		break;
	default:
		aspect = "undefined";
		break;
	}
	switch ((x[3] >> 5) & 0x3) {
	case 0:
		stereo = ", no 3D stereo";
		break;
	case 1:
		stereo = ", 3D stereo";
		break;
	case 2:
		stereo = ", 3D stereo depends on user action";
		break;
	case 3:
		stereo = ", reserved";
		break;
	}
	printf("  Aspect %s%s%s\n", aspect, x[3] & 0x80 ? ", preferred" : "", stereo);
	pix_clock = x[0] + (x[1] << 8) + (x[2] << 16);
	ha = x[4] | (x[5] << 8);
	hbl = x[6] | (x[7] << 8);
	hso = x[8] | ((x[9] & 0x7f) << 8);
	phsync = ((x[9] >> 7) & 0x1) ? '+' : '-';
	hspw = x[10] | (x[11] << 8);
	va = x[12] | (x[13] << 8);
	vbl = x[14] | (x[15] << 8);
	vso = x[16] | ((x[17] & 0x7f) << 8);
	vspw = x[18] | (x[19] << 8);
	pvsync = ((x[17] >> 7) & 0x1 ) ? '+' : '-';

	printf("  Detailed mode: Clock %.3f MHz, %d mm x %d mm\n"
	       "                 %4d %4d %4d %4d\n"
	       "                 %4d %4d %4d %4d\n"
	       "                 %chsync %cvsync\n",
	       (float)pix_clock/100.0, 0, 0,
	       ha, ha + hso, ha + hso + hspw, ha + hbl,
	       va, va + vso, va + vso + vspw, va + vbl,
	       phsync, pvsync
	      );
}

static const struct {
	int x, y, refresh, ratio_w, ratio_h;
	int hor_freq_hz, pixclk_khz, rb, interlaced;
} displayid_vesa_dmt[] = {
	/* 0x03 bit 7 - 0 */
	{640, 350, 85, 64, 35, 37900, 31500},
	{640, 400, 85, 16, 10, 37900, 31500},
	{720, 400, 85, 9, 5, 37900, 35500},
	{640, 480, 60, 4, 3, 31469, 25175},
	{640, 480, 72, 4, 3, 37900, 31500},
	{640, 480, 75, 4, 3, 37500, 31500},
	{640, 480, 85, 4, 3, 43300, 36000},
	{800, 600, 56, 4, 3, 35200, 36000},
	/* 0x04 bit 7 - 0 */
	{800, 600, 60, 4, 3, 37900, 40000},
	{800, 600, 72, 4, 3, 48100, 50000},
	{800, 600, 75, 4, 3, 46900, 49500},
	{800, 600, 85, 4, 3, 53700, 56250},
	{800, 600, 120, 4, 3, 76302, 73250, 1},
	{848, 480, 60, 16, 9, 31020, 33750},
	{1024, 768, 43, 4, 3, 35522, 44900, 0, 1},
	{1024, 768, 60, 4, 3, 48400, 65000},
	/* 0x05 bit 7 - 0 */
	{1024, 768, 70, 4, 3, 56500, 75000},
	{1024, 768, 75, 4, 3, 60000, 78750},
	{1024, 768, 85, 4, 3, 68700, 94500},
	{1024, 768, 120, 4, 3, 97551, 115500, 1},
	{1152, 864, 75, 4, 3, 67500, 108000},
	{1280, 768, 60, 5, 3, 47400, 68250, 1},
	{1280, 768, 60, 5, 3, 47800, 79500},
	{1280, 768, 75, 5, 3, 60300, 102250},
	/* 0x06 bit 7 - 0 */
	{1280, 768, 85, 5, 3, 68600, 117500},
	{1280, 768, 120, 5, 3, 97396, 140250, 1},
	{1280, 800, 60, 16, 10, 49306, 710000, 1},
	{1280, 800, 60, 16, 10, 49702, 83500},
	{1280, 800, 75, 16, 10, 62795, 106500},
	{1280, 800, 85, 16, 10, 71554, 122500},
	{1280, 800, 120, 16, 10, 101563, 146250, 1},
	{1280, 960, 60, 4, 3, 60000, 108000},
	/* 0x07 bit 7 - 0 */
	{1280, 960, 85, 4, 3, 85900, 148500},
	{1280, 960, 120, 4, 3, 121875, 175500, 1},
	{1280, 1024, 60, 5, 4, 64000, 108000},
	{1280, 1024, 75, 5, 4, 80000, 135000},
	{1280, 1024, 85, 5, 4, 91100, 157500},
	{1280, 1024, 120, 5, 4, 130035, 187250, 1},
	{1360, 768, 60, 85, 48, 47700, 85500},
	{1360, 768, 120, 85, 48, 97533, 148250, 1},
	/* 0x08 bit 7 - 0 */
	{1400, 1050, 60, 4, 3, 64700, 101000, 1},
	{1400, 1050, 60, 4, 3, 65300, 121750},
	{1400, 1050, 75, 4, 3, 82300, 156000},
	{1400, 1050, 85, 4, 3, 93900, 179500},
	{1400, 1050, 120, 4, 3, 133333, 208000, 1},
	{1440, 900, 60, 16, 10, 55500, 88750, 1},
	{1440, 900, 60, 16, 10, 65300, 121750},
	{1440, 900, 75, 16, 10, 82300, 156000},
	/* 0x09 bit 7 - 0 */
	{1440, 900, 85, 16, 10, 93900, 179500},
	{1440, 900, 120, 16, 10, 114219, 182750, 1},
	{1600, 1200, 60, 4, 3, 75000, 162000},
	{1600, 1200, 65, 4, 3, 81300, 175500},
	{1600, 1200, 70, 4, 3, 87500, 189000},
	{1600, 1200, 75, 4, 3, 93800, 202500},
	{1600, 1200, 85, 4, 3, 106300, 229500},
	{1600, 1200, 120, 4, 3, 152415, 268250, 1},
	/* 0x0a bit 7 - 0 */
	{1680, 1050, 60, 16, 10, 64700, 119000, 1},
	{1680, 1050, 60, 16, 10, 65300, 146250},
	{1680, 1050, 75, 16, 10, 82300, 187000},
	{1680, 1050, 85, 16, 10, 93900, 214750},
	{1680, 1050, 120, 16, 10, 133424, 245500, 1},
	{1792, 1344, 60, 4, 3, 83600, 204750},
	{1792, 1344, 75, 4, 3, 106300, 261000},
	{1792, 1344, 120, 4, 3, 170722, 333250, 1},
	/* 0x0b bit 7 - 4 */
	{1856, 1392, 60, 4, 3, 86300, 218250},
	{1856, 1392, 75, 4, 3, 112500, 288000},
	{1856, 1392, 120, 4, 3, 176835, 356500, 1},
	{1920, 1200, 60, 16, 10, 74000, 154000, 1},
	{1920, 1200, 60, 16, 10, 74600, 193250},
	{1920, 1200, 75, 16, 10, 94000, 245250},
	{1920, 1200, 85, 16, 10, 107200, 281250},
	{1920, 1200, 120, 16, 10, 152404, 317000, 1},
	/* 0x0c bit 7 - 4 */
	{1920, 1440, 60, 4, 3, 90000, 234000},
	{1920, 1440, 75, 4, 3, 112500, 297000},
	{1920, 1440, 120, 4, 3, 182933, 380500, 1},
	{2560, 1600, 60, 16, 10, 98713, 268500, 1},
	{2560, 1600, 60, 16, 10, 99458, 348500},
	{2560, 1600, 75, 16, 10, 125354, 443250},
	{2560, 1600, 85, 16, 10, 142887, 505250},
	{2560, 1600, 120, 16, 10, 203217, 552750, 1},
};

static int parse_displayid(const unsigned char *x)
{
	const unsigned char *orig = x;
	int version = x[1];
	int length = x[2];
	int ext_count = x[4];
	int i;

	cur_block = "DisplayID";

	printf("Length %d, version %u.%u, extension count %d\n",
	       length, version >> 4, version & 0xf, ext_count);

	int offset = 5;
	while (length > 0) {
		int tag = x[offset];
		int len = x[offset + 2];

		if (len == 0)
			break;
		switch (tag) {
		case 0:
			printf("Product ID Block\n");
			break;
		case 1:
			printf("Display Parameters Block\n");
			break;
		case 2:
			printf("Color Characteristics Block\n");
			break;
		case 3: {
			printf("Type 1 Detailed Timings Block\n");
			for (i = 0; i < len / 20; i++) {
				parse_displayid_detailed_timing(&x[offset + 3 + (i * 20)]);
			}
			break;
		}
		case 4:
			printf("Type 2 Detailed Timings Block\n");
			break;
		case 5:
			printf("Type 3 Short Timings Block\n");
			break;
		case 6:
			printf("Type 4 DMT Timings Block\n");
			break;
		case 7:
			printf("Type 1 VESA DMT Timings Block\n");
			for (i = 0; i < min(len, 10) * 8; i++) {
				if (x[offset + 3 + i / 8] & (1 << (i % 8))) {
					printf("  %dx%d%s@%dHz %s%u:%u HorFreq: %d Hz Clock: %.3f MHz\n",
					       displayid_vesa_dmt[i].x,
					       displayid_vesa_dmt[i].y,
					       displayid_vesa_dmt[i].interlaced ? "i" : "",
					       displayid_vesa_dmt[i].refresh,
					       displayid_vesa_dmt[i].rb ? "RB " : "",
					       displayid_vesa_dmt[i].ratio_w, displayid_vesa_dmt[i].ratio_h,
					       displayid_vesa_dmt[i].hor_freq_hz,
					       displayid_vesa_dmt[i].pixclk_khz / 1000.0);
					min_vert_freq_hz = min(min_vert_freq_hz, displayid_vesa_dmt[i].refresh);
					max_vert_freq_hz = max(max_vert_freq_hz, displayid_vesa_dmt[i].refresh);
					min_hor_freq_hz = min(min_hor_freq_hz, displayid_vesa_dmt[i].hor_freq_hz);
					max_hor_freq_hz = max(max_hor_freq_hz, displayid_vesa_dmt[i].hor_freq_hz);
					max_pixclk_khz = max(max_pixclk_khz, displayid_vesa_dmt[i].pixclk_khz);
				}
			}
			break;
		case 8:
			printf("CTA Timings Block\n");
			break;
		case 9:
			printf("Video Timing Range Block\n");
			break;
		case 0xa:
			printf("Product Serial Number Block\n");
			break;
		case 0xb:
			printf("GP ASCII String Block\n");
			break;
		case 0xc:
			printf("Display Device Data Block\n");
			break;
		case 0xd:
			printf("Interface Power Sequencing Block\n");
			break;
		case 0xe:
			printf("Transfer Characteristics Block\n");
			break;
		case 0xf:
			printf("Display Interface Block\n");
			break;
		case 0x10:
			printf("Stereo Display Interface Block\n");
			break;
		case 0x12: {
			int capabilities = x[offset + 3];
			int num_v_tile = (x[offset + 4] & 0xf) | (x[offset + 6] & 0x30);
			int num_h_tile = (x[offset + 4] >> 4) | ((x[offset + 6] >> 2) & 0x30);
			int tile_v_location = (x[offset + 5] & 0xf) | ((x[offset + 6] & 0x3) << 4);
			int tile_h_location = (x[offset + 5] >> 4) | (((x[offset + 6] >> 2) & 0x3) << 4);
			int tile_width = x[offset + 7] | (x[offset + 8] << 8);
			int tile_height = x[offset + 9] | (x[offset + 10] << 8);
			int pix_mult = x[offset + 11];

			printf("Tiled Display Topology Block\n");
			printf("  Capabilities: 0x%08x\n", capabilities);
			printf("  Num horizontal tiles: %d Num vertical tiles: %d\n", num_h_tile + 1, num_v_tile + 1);
			printf("  Tile location: %d, %d\n", tile_h_location, tile_v_location);
			printf("  Tile resolution: %dx%d\n", tile_width + 1, tile_height + 1);
			if (capabilities & 0x40) {
				if (pix_mult) {
					printf("  Top bevel size: %d pixels\n",
					       pix_mult * x[offset + 12] / 10);
					printf("  Bottom bevel size: %d pixels\n",
					       pix_mult * x[offset + 13] / 10);
					printf("  Right bevel size: %d pixels\n",
					       pix_mult * x[offset + 14] / 10);
					printf("  Left bevel size: %d pixels\n",
					       pix_mult * x[offset + 15] / 10);
				} else {
					warn("No bevel information, but the pixel multiplier is non-zero\n");
				}
				printf("  Tile resolution: %dx%d\n", tile_width + 1, tile_height + 1);
			} else if (pix_mult) {
				warn("No bevel information, but the pixel multiplier is non-zero\n");
			}
			break;
		}
		default:
			printf("Unknown DisplayID Data Block 0x%x\n", tag);
			break;
		}
		length -= len + 3;
		offset += len + 3;
	}

	/* DisplayID length field is number of following bytes
	 * but checksum is calculated over the entire structure
	 * (excluding DisplayID-in-EDID magic byte)
	 */
	has_valid_displayid_checksum = do_checksum(orig+1, orig[2] + 5);
	return 0;
}

int decode_edid(unsigned char* edid)
{
	unsigned char *x;
	time_t the_time;
	struct tm *ptm;
	int analog, i;
	unsigned col_x, col_y;

	if (!edid) {
		return -1;
	}

	if (!edid || memcmp(edid, "\x00\xFF\xFF\xFF\xFF\xFF\xFF\x00", 8)) {
		fprintf(stderr, "No header found\n");
		return -1;
	}

	cur_block = "EDID Structure Version & Revision";
	printf("EDID version: %hd.%hd\n", edid[0x12], edid[0x13]);
	if (edid[0x12] == 1) {
		if (edid[0x13] > 4) {
			printf("Claims > 1.4, assuming 1.4 conformance\n");
			edid[0x13] = 4;
		}
		edid_minor = edid[0x13];
		switch (edid[0x13]) {
		case 4:
			claims_one_point_four = 1;
		case 3:
			claims_one_point_three = 1;
		case 2:
			claims_one_point_two = 1;
		default:
			break;
		}
		claims_one_point_oh = 1;
	}

	cur_block = "Vendor & Product Identification";
	printf("Manufacturer: %s Model %x Serial Number %u\n",
	       manufacturer_name(edid + 0x08),
	       (unsigned short)(edid[0x0A] + (edid[0x0B] << 8)),
	       (unsigned int)(edid[0x0C] + (edid[0x0D] << 8)
			      + (edid[0x0E] << 16) + (edid[0x0F] << 24)));
	has_valid_serial_number = edid[0x0C] || edid[0x0D] || edid[0x0E] || edid[0x0F];
	/* XXX need manufacturer ID table */

	time(&the_time);
	ptm = localtime(&the_time);
	if (edid[0x10] < 55 || (edid[0x10] == 0xff && claims_one_point_four)) {
		has_valid_week = 1;
		if (edid[0x11] > 0x0f) {
			if (edid[0x10] == 0xff) {
				has_valid_year = 1;
				printf("Model year %hd\n", edid[0x11] + 1990);
			} else if (edid[0x11] + 90 <= ptm->tm_year + 1) {
				has_valid_year = 1;
				if (edid[0x10])
					printf("Made in week %hd of %hd\n", edid[0x10], edid[0x11] + 1990);
				else
					printf("Made in year %hd\n", edid[0x11] + 1990);
			}
		}
	}
		if (!has_valid_year)
			warn("Invalid year\n");

	/* display section */

	cur_block = "Basic Display Parameters & Features";
	if (edid[0x14] & 0x80) {
		int conformance_mask;
		analog = 0;
		printf("Digital display\n");
		if (claims_one_point_four) {
			conformance_mask = 0;
			if ((edid[0x14] & 0x70) == 0x00)
				printf("Color depth is undefined\n");
			else if ((edid[0x14] & 0x70) == 0x70)
				nonconformant_digital_display = 1;
			else
				printf("%d bits per primary color channel\n",
				       ((edid[0x14] & 0x70) >> 3) + 4);

			switch (edid[0x14] & 0x0f) {
			case 0x00: printf("Digital interface is not defined\n"); break;
			case 0x01: printf("DVI interface\n"); break;
			case 0x02: printf("HDMI-a interface\n"); break;
			case 0x03: printf("HDMI-b interface\n"); break;
			case 0x04: printf("MDDI interface\n"); break;
			case 0x05: printf("DisplayPort interface\n"); break;
			default:
				   nonconformant_digital_display = 1;
			}
		} else if (claims_one_point_two) {
			conformance_mask = 0x7E;
			if (edid[0x14] & 0x01) {
				printf("DFP 1.x compatible TMDS\n");
			}
		} else conformance_mask = 0x7F;
		if (!nonconformant_digital_display)
			nonconformant_digital_display = edid[0x14] & conformance_mask;
	} else {
		analog = 1;
		int voltage = (edid[0x14] & 0x60) >> 5;
		int sync = (edid[0x14] & 0x0F);
		printf("Analog display, Input voltage level: %s V\n",
		       voltage == 3 ? "0.7/0.7" :
		       voltage == 2 ? "1.0/0.4" :
		       voltage == 1 ? "0.714/0.286" :
		       "0.7/0.3");

		if (claims_one_point_four) {
			if (edid[0x14] & 0x10)
				printf("Blank-to-black setup/pedestal\n");
			else
				printf("Blank level equals black level\n");
		} else if (edid[0x14] & 0x10) {
			/*
			 * XXX this is just the X text.  1.3 says "if set, display expects
			 * a blank-to-black setup or pedestal per appropriate Signal
			 * Level Standard".  Whatever _that_ means.
			 */
			printf("Configurable signal levels\n");
		}

		printf("Sync: %s%s%s%s\n", sync & 0x08 ? "Separate " : "",
		       sync & 0x04 ? "Composite " : "",
		       sync & 0x02 ? "SyncOnGreen " : "",
		       sync & 0x01 ? "Serration " : "");
	}

	if (edid[0x15] && edid[0x16])
		printf("Maximum image size: %d cm x %d cm\n", edid[0x15], edid[0x16]);
	else if (claims_one_point_four && (edid[0x15] || edid[0x16])) {
		if (edid[0x15])
			printf("Aspect ratio is %f (landscape)\n", 100.0/(edid[0x16] + 99));
		else
			printf("Aspect ratio is %f (portrait)\n", 100.0/(edid[0x15] + 99));
	} else {
		/* Either or both can be zero for 1.3 and before */
		printf("Image size is variable\n");
	}

	if (edid[0x17] == 0xff) {
		if (claims_one_point_four)
			printf("Gamma is defined in an extension block\n");
		else
			/* XXX Technically 1.3 doesn't say this... */
			printf("Gamma: 1.0\n");
	} else printf("Gamma: %.2f\n", ((edid[0x17] + 100.0) / 100.0));

	if (edid[0x18] & 0xE0) {
		printf("DPMS levels:");
		if (edid[0x18] & 0x80) printf(" Standby");
		if (edid[0x18] & 0x40) printf(" Suspend");
		if (edid[0x18] & 0x20) printf(" Off");
		printf("\n");
	}

	if (analog || !claims_one_point_four) {
		switch (edid[0x18] & 0x18) {
		case 0x00: printf("Monochrome or grayscale display\n"); break;
		case 0x08: printf("RGB color display\n"); break;
		case 0x10: printf("Non-RGB color display\n"); break;
		case 0x18: printf("Undefined display color type\n");
		}
	} else {
		printf("Supported color formats: RGB 4:4:4");
		if (edid[0x18] & 0x08)
			printf(", YCrCb 4:4:4");
		if (edid[0x18] & 0x10)
			printf(", YCrCb 4:2:2");
		printf("\n");
	}

	if (edid[0x18] & 0x04) {
		/*
		 * The sRGB chromaticities are (x, y):
		 * red:   0.640,  0.330
		 * green: 0.300,  0.600
		 * blue:  0.150,  0.060
		 * white: 0.3127, 0.3290
		 */
		static const unsigned char srgb_chromaticity[10] = {
			0xee, 0x91, 0xa3, 0x54, 0x4c, 0x99, 0x26, 0x0f, 0x50, 0x54
		};
		printf("Default (sRGB) color space is primary color space\n");
		nonconformant_srgb_chromaticity =
			memcmp(edid + 0x19, srgb_chromaticity, sizeof(srgb_chromaticity));
	}
	if (edid[0x18] & 0x02) {
		if (claims_one_point_four)
			printf("First detailed timing includes the native pixel format and preferred refresh rate\n");
		else
			printf("First detailed timing is preferred timing\n");
		has_preferred_timing = 1;
	} else if (claims_one_point_four) {
		/* 1.4 always has a preferred timing and this bit means something else. */
		has_preferred_timing = 1;
	}

	if (edid[0x18] & 0x01) {
		if (claims_one_point_four)
			printf("Display is continuous frequency\n");
		else
			printf("Supports GTF timings within operating range\n");
	}

	cur_block = "Color Characteristics";
	printf("Display x,y Chromaticity:\n");
	col_x = (edid[0x1b] << 2) | (edid[0x19] >> 6);
	col_y = (edid[0x1c] << 2) | ((edid[0x19] >> 4) & 3);
	printf("  Red:   0.%04u, 0.%04u\n",
	       (col_x * 10000) / 1024, (col_y * 10000) / 1024);
	col_x = (edid[0x1d] << 2) | ((edid[0x19] >> 2) & 3);
	col_y = (edid[0x1e] << 2) | (edid[0x19] & 3);
	printf("  Green: 0.%04u, 0.%04u\n",
	       (col_x * 10000) / 1024, (col_y * 10000) / 1024);
	col_x = (edid[0x1f] << 2) | (edid[0x1a] >> 6);
	col_y = (edid[0x20] << 2) | ((edid[0x1a] >> 4) & 3);
	printf("  Blue:  0.%04u, 0.%04u\n",
	       (col_x * 10000) / 1024, (col_y * 10000) / 1024);
	col_x = (edid[0x21] << 2) | ((edid[0x1a] >> 2) & 3);
	col_y = (edid[0x22] << 2) | (edid[0x1a] & 3);
	printf("  White: 0.%04u, 0.%04u\n",
	       (col_x * 10000) / 1024, (col_y * 10000) / 1024);

	cur_block = "Established Timings";
	printf("Established timings supported:\n");
	for (i = 0; i < 17; i++) {
		if (edid[0x23 + i / 8] & (1 << (7 - i % 8))) {
			min_vert_freq_hz = min(min_vert_freq_hz, established_timings[i].refresh);
			max_vert_freq_hz = max(max_vert_freq_hz, established_timings[i].refresh);
			min_hor_freq_hz = min(min_hor_freq_hz, established_timings[i].hor_freq_hz);
			max_hor_freq_hz = max(max_hor_freq_hz, established_timings[i].hor_freq_hz);
			max_pixclk_khz = max(max_pixclk_khz, established_timings[i].pixclk_khz);
			printf("  %dx%d%s@%dHz %u:%u HorFreq: %d Hz Clock: %.3f MHz\n",
			       established_timings[i].x, established_timings[i].y,
			       established_timings[i].interlaced ? "i" : "",
			       established_timings[i].refresh,
			       established_timings[i].ratio_w, established_timings[i].ratio_h,
			       established_timings[i].hor_freq_hz,
			       established_timings[i].pixclk_khz / 1000.0);
		}
	}
	has_640x480p60_est_timing = edid[0x23] & 0x20;

	cur_block = "Standard Timings";
	printf("Standard timings supported:\n");
	for (i = 0; i < 8; i++)
		print_standard_timing(edid[0x26 + i * 2], edid[0x26 + i * 2 + 1]);

	/* detailed timings */
	cur_block = "Detailed Timings";
	has_valid_detailed_blocks = detailed_block(edid + 0x36, 0);
	if (has_preferred_timing && !did_detailed_timing)
		has_preferred_timing = 0; /* not really accurate... */
	has_valid_detailed_blocks &= detailed_block(edid + 0x48, 0);
	has_valid_detailed_blocks &= detailed_block(edid + 0x5A, 0);
	has_valid_detailed_blocks &= detailed_block(edid + 0x6C, 0);

	if (edid[0x7e])
		printf("Has %d extension blocks\n", edid[0x7e]);

	has_valid_checksum = do_checksum(edid, EDID_PAGE_SIZE);
}

/*
 * Notes on panel extensions: (TODO, implement me in the code)
 *
 * EPI: http://www.epi-standard.org/fileadmin/spec/EPI_Specification1.0.pdf
 * at offset 0x6c (fourth detailed block): (all other bits reserved)
 * 0x6c: 00 00 00 0e 00
 * 0x71: bit 6-5: data color mapping (00 conventional/fpdi/vesa, 01 openldi)
 *       bit 4-3: pixels per clock (00 1, 01 2, 10 4, 11 reserved)
 *       bit 2-0: bits per pixel (000 18, 001 24, 010 30, else reserved)
 * 0x72: bit 5: FPSCLK polarity (0 normal 1 inverted)
 *       bit 4: DE polarity (0 high active 1 low active)
 *       bit 3-0: interface (0000 LVDS TFT
 *                           0001 mono STN 4/8bit
 *                           0010 color STN 8/16 bit
 *                           0011 18 bit tft
 *                           0100 24 bit tft
 *                           0101 tmds
 *                           else reserved)
 * 0x73: bit 1: horizontal display mode (0 normal 1 right/left reverse)
 *       bit 0: vertical display mode (0 normal 1 up/down reverse)
 * 0x74: bit 7-4: total poweroff seq delay (0000 vga controller default
 *                                          else time in 10ms (10ms to 150ms))
 *       bit 3-0: total poweron seq delay (as above)
 * 0x75: contrast power on/off seq delay, same as 0x74
 * 0x76: bit 7: backlight control enable (1 means this field is valid)
 *       bit 6: backlight enabled at boot (0 on 1 off)
 *       bit 5-0: backlight brightness control steps (0..63)
 * 0x77: bit 7: contrast control, same bit pattern as 0x76 except bit 6 resvd
 * 0x78 - 0x7c: reserved
 * 0x7d: bit 7-4: EPI descriptor major version (1)
 *       bit 3-0: EPI descriptor minor version (0)
 *
 * ----
 *
 * SPWG: http://www.spwg.org/spwg_spec_version3.8_3-14-2007.pdf
 *
 * Since these are "dummy" blocks, terminate with 0a 20 20 20 ... as usual
 *
 * detailed descriptor 3:
 * 0x5a - 0x5e: 00 00 00 fe 00
 * 0x5f - 0x63: PC maker part number
 * 0x64: LCD supplier revision #
 * 0x65 - 0x6b: manufacturer part number
 *
 * detailed descriptor 4:
 * 0x6c - 0x70: 00 00 00 fe 00
 * 0x71 - 0x78: smbus nits values (whut)
 * 0x79: number of lvds channels (1 or 2)
 * 0x7A: panel self test (1 if present)
 * and then dummy terminator
 *
 * SPWG also says something strange about the LSB of detailed descriptor 1:
 * "LSB is set to "1" if panel is DE-timing only. H/V can be ignored."
 */
