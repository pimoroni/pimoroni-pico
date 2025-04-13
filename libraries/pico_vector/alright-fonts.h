/*

  Alright Fonts 🖍 - a font format for embedded and low resource platforms.

  Jonathan Williamson, August 2022
  Examples, source, and more: https://github.com/lowfatcode/pretty-poly
  MIT License https://github.com/lowfatcode/pretty-poly/blob/main/LICENSE

  An easy way to render high quality text in embedded applications running 
  on resource constrained microcontrollers such as the Cortex M0 and up.         

    - OTF and TTF support: generate efficient packed fonts easily
    - Minimal data: ~4kB (40 bytes per char) for printable ASCII set (Roboto)
    - Tunable: trade off file size, contour complexity, and visual quality
    - Metrics: advance and bounding box for fast layout
    - UTF-8 or ASCII: support for non ASCII like Kanji or Cyrillic
    - Fixed scale: coords scaled to ^2 bounds for fast scaling (no divide)
    - C17 header only library: simply copy the header file into your project
    - Customised font packs: include only the characters you need
    - Simple outlines: all paths are simply polylines for easy rendering
    - Easy antialiasing: combine with Pretty Poly for quick results!

*/

#ifndef AF_INCLUDE_H
#define AF_INCLUDE_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <wchar.h>
#include <float.h>

#define FLAG_16BIT_POINT_COUNT 0b00000001

#ifdef AF_MALLOC
  #ifndef PP_MALLOC
    #define PP_MALLOC(size)         AF_MALLOC(size)
    #define PP_REALLOC(p, size)     AF_REALLOC(p, size)
    #define PP_FREE(p)              AF_FREE(p)
  #endif // PP_MALLOC
#endif // AF_MALLOC

#ifndef AF_MALLOC
  #define AF_MALLOC(size)         malloc(size)
  #define AF_REALLOC(p, size)     realloc(p, size)
  #define AF_FREE(p)              free(p)
#endif // AF_MALLOC

#ifndef AF_FILE
  #define AF_FILE FILE*
  #define AF_FREAD(p, size, nmemb, stream) fread(p, size, nmemb, stream)
  #define AF_FGETC(stream) fgetc(stream)
#endif

#ifndef AF_DEBUG
  #define AF_DEBUG(...)
#endif

#include "pretty-poly.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int8_t x, y;
} af_point_t;
pp_point_t af_point_transform(pp_point_t *p, pp_mat3_t *m);

typedef struct {
  uint16_t point_count;
  af_point_t *points;
} af_path_t;

typedef struct {
  int16_t codepoint;
  int8_t x, y, w, h;
  int8_t advance;
  uint8_t path_count;
  af_path_t *paths;    
} af_glyph_t;

typedef struct {
  uint16_t flags;
  uint16_t glyph_count;
  af_glyph_t *glyphs;
} af_face_t;

typedef enum {
  AF_H_ALIGN_LEFT = 0, AF_H_ALIGN_CENTER = 1, AF_H_ALIGN_RIGHT = 2, 
  AF_H_ALIGN_JUSTIFY = 4,
  AF_V_ALIGN_TOP = 8, AF_V_ALIGN_MIDDLE = 16, AF_V_ALIGN_BOTTOM = 32
} af_align_t;  
 
typedef struct  {
  af_face_t *face;                  // font
  float size;                       // text size in pixels
  float line_height;                // spacing between lines (%)
  float letter_spacing;             // spacing between characters (%)   
  float word_spacing;               // spacing between words (%)
  unsigned int align;               // horizontal and vertical alignment
  pp_mat3_t *transform;             // arbitrary transformation
} af_text_metrics_t;

bool af_load_font_file(AF_FILE file, af_face_t *face);
void af_render_character(af_face_t *face, uint16_t codepoint, af_text_metrics_t *tm);
void af_render(af_face_t *face, const char *text, size_t tlen, float max_line_width, float max_height, af_text_metrics_t *tm);
pp_rect_t af_measure(af_face_t *face, const char *text, size_t tlen, float max_line_width, af_text_metrics_t *tm);

#ifdef AF_USE_PRETTY_POLY
#endif

#ifdef __cplusplus
}
#endif

#ifdef AF_IMPLEMENTATION


/*
  helper functions
*/

// big endian file reading helpers
uint16_t  ru16(AF_FILE file) {uint8_t  w[2]; AF_FREAD((char *) w, 1, 2, file); return  (uint16_t)w[0] <<  8 |  w[1];}
int16_t   rs16(AF_FILE file) {uint8_t  w[2]; AF_FREAD((char *) w, 1, 2, file); return  (uint16_t)w[0] <<  8 |  w[1];}
uint32_t  ru32(AF_FILE file) {uint8_t dw[4]; AF_FREAD((char *)dw, 1, 4, file); return (uint32_t)dw[0] << 24 | (uint32_t)dw[1] << 16 | (uint32_t)dw[2] << 8 | dw[3];}
uint8_t    ru8(AF_FILE file) {return AF_FGETC(file);}
int8_t     rs8(AF_FILE file) {return AF_FGETC(file);}

bool af_load_font_file(AF_FILE file, af_face_t *face) {
  bool font16 = false;
  // check header magic bytes are present
  char marker[4]; AF_FREAD(marker, 1, 4, file);
  if(memcmp(marker, "af!?", 4) != 0) {    
    return false; // doesn't start with magic marker
  }

  // extract flags and ensure none set
  face->flags = ru16(file);
  if(face->flags == FLAG_16BIT_POINT_COUNT) {
    font16 = true;
  } else if(face->flags != 0) {    
    return false; // unknown flags set
  }

  // number of glyphs, paths, and points in font
  uint16_t glyph_count = ru16(file);
  uint16_t path_count  = ru16(file);
  uint16_t point_count = ru16(file);

  size_t glyph_buffer_size = sizeof(af_glyph_t) * glyph_count;
  size_t path_buffer_size = sizeof(af_path_t) * path_count;
  size_t point_buffer_size = sizeof(af_point_t) * point_count;

  // allocate buffer to store font glyph, path, and point data
  uint8_t *buffer = (uint8_t *)AF_MALLOC(glyph_buffer_size + path_buffer_size + point_buffer_size);

  if(!buffer) {
    return false; // failed memory allocation
  }

  af_glyph_t *glyphs = (af_glyph_t *) buffer;
  af_path_t   *paths = ( af_path_t *)(buffer + glyph_buffer_size);
  af_point_t *points = (af_point_t *)(buffer + glyph_buffer_size + path_buffer_size);

  // load glyph dictionary
  face->glyph_count = glyph_count;
  face->glyphs      = glyphs;
  for(int i = 0; i < glyph_count; i++) {
    af_glyph_t *glyph = &face->glyphs[i];
    glyph->codepoint  = ru16(file);
    glyph->x          =  rs8(file);
    glyph->y          =  rs8(file);
    glyph->w          =  ru8(file);
    glyph->h          =  ru8(file);
    glyph->advance    =  ru8(file);
    glyph->path_count =  ru8(file);
    glyph->paths      =      paths;
    paths += glyph->path_count;
  }

  // load the glyph paths
  for(int i = 0; i < glyph_count; i++) {
    af_glyph_t *glyph = &face->glyphs[i];
    for(int j = 0; j < glyph->path_count; j++) {
      af_path_t *path = &glyph->paths[j];
      path->point_count = font16 ? ru16(file) : ru8(file);
      path->points = points;
      points += path->point_count;
    }
  }

  // load the glyph points
  for(int i = 0; i < glyph_count; i++) {
    af_glyph_t *glyph = &face->glyphs[i];
    for(int j = 0; j < glyph->path_count; j++) {
      af_path_t *path = &glyph->paths[j];
      for(int k = 0; k < path->point_count; k++) {
        af_point_t *point = &path->points[k];
        point->x = ru8(file);
        point->y = ru8(file);
      }
    }
  }

  return true;
}

uint16_t get_utf8_char(const char *text, const char *end)
{
  uint16_t codepoint;
  if((*text & 0x80) == 0x00)
  {
    codepoint = *text; // ASCII, codepoints U+0000...U007F
  }
  else if( ((*text & 0xE0) == 0xC0) && (text+1 <= end) && ((*(text+1) & 0xC0) == 0x80) )
  {
    codepoint = ((uint16_t)(*text & 0x1F) << 6) + (*(text+1) & 0x3F); //codepoints U+0080...U+07FF
  }
  else if( ((*text & 0xF0) == 0xE0) && (text+2 <= end) && ((*(text+1) & 0xC0) == 0x80) && ((*(text+2) & 0xC0) == 0x80) )
  {
    codepoint = ((uint16_t)(*text & 0x0F) << 12) + ((uint16_t)(*(text+1) & 0x3F) << 6) + (*(text+2) & 0x3F); // codepoints U+0800...U+FFFF
  }
  else
  {
    codepoint = 0xFFFF; // malformed UTF-8 sequences or unsupported codepoints starting at U+10000
  }
  return codepoint;

}

uint8_t num_of_utf8_continuation_bytes(const char *text, const char *end)
{
  uint8_t cont = 0;
  for(char c = *text; text < end; text++, c = *text)
  {
    if((c & 0xC0) == 0x80)
    {
      cont++;
    }
  }
  return cont;
}

af_glyph_t *find_glyph(af_face_t *face, char c) {
  for(int i = 0; i < face->glyph_count; i++) {
    if(face->glyphs[i].codepoint == c) {
      return &face->glyphs[i];
    }
  }
  return NULL;
}

void af_render_glyph(af_glyph_t* glyph, af_text_metrics_t *tm) {
  assert(glyph != NULL);

  pp_poly_t *poly = pp_poly_new();
  for(uint32_t i = 0; i < glyph->path_count; i++) {
    pp_path_t *path = pp_poly_add_path(poly);
    for(uint32_t j = 0; j < glyph->paths[i].point_count; j++) {
      pp_path_add_point(path, {
        glyph->paths[i].points[j].x,
        glyph->paths[i].points[j].y
      });
    }
  }

  pp_render(poly);

  pp_poly_free(poly);
}

void af_render_character(af_face_t *face, uint16_t c, af_text_metrics_t *tm) {
  af_glyph_t *glyph = find_glyph(face, c);
  if(!glyph) {
    return;
  }
  af_render_glyph(glyph, tm);
}

float get_line_width(af_face_t *face, const char *text, size_t *tlen, float max_line_width, af_text_metrics_t *tm) {
  float line_width = 0;
  const char *start = text;
  const char *end = text + *tlen;
  const char *last_space = nullptr;
  uint16_t utf8_char;
  while(text < end) {
    utf8_char = get_utf8_char(text, end);
    text++;
    if(utf8_char > 0x7F) {
      text++;
    }
    if(utf8_char > 0x7FF) {
      text++;
    }
    af_glyph_t *glyph = find_glyph(face, utf8_char);
    if(!glyph) {
      continue;
    }

    float char_width;
    if(utf8_char == L' ') {
      char_width = (glyph->advance * tm->word_spacing) / 100.0f;
      last_space = text;
    } else {
      char_width = (glyph->advance * tm->letter_spacing) / 100.0f;
    }

    if (max_line_width > 0 && line_width + char_width > max_line_width && last_space) {
      *tlen = last_space - start;
      break;
    }
    line_width += char_width;
  }
  return line_width;
}

size_t line_length(const char *text, const char *end) {
  if(text >= end) return 0;

  char *line_ending = (char *)memchr(text, '\n', end - text);

  if(line_ending == NULL || line_ending > end) {
    line_ending = (char *)end;
  }

  return line_ending - text;
}

float get_max_line_width(af_face_t *face, const char *text, size_t tlen, af_text_metrics_t *tm) {
  float max_width = 0;
  char *line = (char *)text;
  char *tend = line + tlen;

  size_t line_len = line_length(line, tend);
  while(line_len) {
    float width = get_line_width(face, line, &line_len, 0, tm);
    max_width = max_width < width ? width : max_width;
    line += line_len + 1;
    line_len = line_length(line, tend);
  }
  
  return max_width;
}

void af_render(af_face_t *face, const char *text, size_t tlen, float max_line_width, float max_height, af_text_metrics_t *tm) {
  char *line = (char *)text;
  char *tend = line + tlen;
  size_t line_len = 0;

  pp_mat3_t *old = pp_transform(NULL);
  
  float line_height = (tm->line_height * 128.0f) / 100.0f;
  float scale = tm->size / 128.0f;
  uint16_t utf8_char;

  struct {
    float x, y;
  } caret;

  caret.x = 0;
  caret.y = 0;

  // find maximum line length
  if (max_line_width == 0.f) {
    max_line_width = get_max_line_width(face, text, tlen, tm);
  } else {
    max_line_width /= scale;
  }
  if (max_height == 0.f) {
    max_height = FLT_MAX;
  } else {
    max_height /= scale;
  }

  line_len = line_length(line, tend);

  while(line_len && caret.y + line_height <= max_height) {
    int line_width = get_line_width(face, line, &line_len, max_line_width, tm);
    char *end = line + line_len;

    while(line < end) {
      utf8_char = get_utf8_char(line, end);
      line++;
      if(utf8_char > 0x7F) {
        line++;
      }
      if(utf8_char > 0x7FF) {
        line++;
      }
      af_glyph_t *glyph = find_glyph(face, utf8_char);
      if(!glyph) {
        continue;
      }
      
      pp_mat3_t caret_transform = *tm->transform;
      pp_mat3_scale(&caret_transform, scale, scale);
      pp_mat3_translate(&caret_transform, caret.x, caret.y);
      
      if(tm->align & AF_H_ALIGN_CENTER) {
        pp_mat3_translate(&caret_transform, (max_line_width - line_width) / 2, 0);
      }

      if(tm->align & AF_H_ALIGN_RIGHT) {
        pp_mat3_translate(&caret_transform, (max_line_width - line_width), 0);
      }

      pp_mat3_t final_transform = old ? *old : pp_mat3_identity();
      pp_mat3_mul(&final_transform, &caret_transform);
      pp_transform(&final_transform);

      af_render_glyph(glyph, tm);
      
      if(utf8_char == L' ') {
        caret.x += (glyph->advance * tm->word_spacing) / 100.0f;
      } else {
        caret.x += (glyph->advance * tm->letter_spacing) / 100.0f;
      }
      
    }

    line += 1; // Skip over \n
    line_len = line_length(line, tend);

    caret.x = 0;
    caret.y += line_height;
  }



  pp_transform(old);
}

void _af_render(af_face_t *face, const char *text, af_text_metrics_t *tm) {
  af_render(face, text, strlen(text), 0, 0, tm);
}

pp_rect_t af_measure(af_face_t *face, const char *text, size_t tlen, float max_line_width, af_text_metrics_t *tm) {
  pp_rect_t result;
  bool first = true;
  char *line = (char *)text;
  char *tend = line + tlen;
  size_t line_len = 0;

  float line_height = (tm->line_height * 128.0f) / 100.0f;
  float scale = tm->size / 128.0f;

  struct {
    float x, y;
  } caret;

  caret.x = 0;
  caret.y = 0;

  // find maximum line length
  if (max_line_width == 0.f) max_line_width = get_max_line_width(face, text, tlen, tm);

  line_len = line_length(line, tend);

  while(line_len) {
    int line_width = get_line_width(face, line, &line_len, max_line_width, tm);
    char *end = line + line_len;

    for(char c = *line; line < end; line++, c = *line) {
      af_glyph_t *glyph = find_glyph(face, c);
      if(!glyph) {
        continue;
      }

      pp_mat3_t caret_transform = *tm->transform;
      pp_mat3_scale(&caret_transform, scale, scale);
      pp_mat3_translate(&caret_transform, caret.x, caret.y);

      if(tm->align & AF_H_ALIGN_CENTER) {
        pp_mat3_translate(&caret_transform, (max_line_width - line_width) / 2, 0);
      }

      if(tm->align & AF_H_ALIGN_RIGHT) {
        pp_mat3_translate(&caret_transform, (max_line_width - line_width), 0);
      }

      pp_rect_t r = {glyph->x, glyph->y, glyph->x + glyph->w, glyph->y + glyph->h};
      //pp_rect_t r = af_glyph_bounds(glyph, tm);
      r = pp_rect_transform(&r, &caret_transform);

      if(first) {
        result = r;
        first = false;
      } else {
        result = pp_rect_merge(&result, &r);
      }

      if(c == L' ') {
        caret.x += (glyph->advance * tm->word_spacing) / 100.0f;
      } else {
        caret.x += (glyph->advance * tm->letter_spacing) / 100.0f;
      }

    }

    line += 1; // Skip over \n
    line_len = line_length(line, tend);

    caret.x = 0;
    caret.y += line_height;
  }

  return result;
}

#endif // AF_IMPLEMENTATION

#endif // AF_INCLUDE_H