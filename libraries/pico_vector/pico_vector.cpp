#define PP_IMPLEMENTATION
#define AF_IMPLEMENTATION
#include "pico_vector.hpp"
#include <vector>

namespace pimoroni {
  PicoGraphics *PicoVector::graphics = nullptr;

  void PicoVector::draw(pp_poly_t *poly) {
    pp_transform(NULL);
    pp_render(poly);
  }

  void PicoVector::draw(pp_poly_t *poly, pp_mat3_t *t) {
    pp_transform(t);
    pp_render(poly);
  }

  void PicoVector::rotate(pp_path_t *path, pp_point_t origin, float angle) {
    pp_mat3_t t = pp_mat3_identity();
    pp_mat3_translate(&t, origin.x, origin.y);
    pp_mat3_rotate(&t, angle);
    pp_mat3_translate(&t, -origin.x, -origin.y);
    transform(path, &t);
  }

  void PicoVector::translate(pp_path_t *path, pp_point_t translation) {
    pp_mat3_t t = pp_mat3_identity();
    pp_mat3_translate(&t, translation.x, translation.y);
    transform(path, &t);
  }

  void PicoVector::transform(pp_path_t *path, pp_mat3_t *t) {
    for (auto j = 0u; j < path->count; j++) {
      path->points[j] = pp_point_transform(&path->points[j], t);
    }
  }

  void PicoVector::rotate(pp_poly_t *poly, pp_point_t origin, float angle) {
    pp_mat3_t t = pp_mat3_identity();
    pp_mat3_translate(&t, origin.x, origin.y);
    pp_mat3_rotate(&t, angle);
    pp_mat3_translate(&t, -origin.x, -origin.y);
    transform(poly, &t);
  }

  void PicoVector::translate(pp_poly_t *poly, pp_point_t translation) {
    pp_mat3_t t = pp_mat3_identity();
    pp_mat3_translate(&t, translation.x, translation.y);
    transform(poly, &t);
  }

  void PicoVector::transform(pp_poly_t *poly, pp_mat3_t *t) {
    for (auto i = 0u; i < poly->count; i++) {
      transform(&poly->paths[i], t);
    }
  }

  pp_point_t PicoVector::text(std::wstring_view text, pp_point_t offset, pp_mat3_t *t) {
    pp_point_t caret = {0, 0};

    // Align text from the bottom left
    caret.y += (PP_COORD_TYPE)text_metrics.line_height;

    caret = pp_point_transform(&caret, t);
    caret.x += offset.x;
    caret.y += offset.y;

    pp_point_t space;
    pp_point_t carriage_return = {0, -(PP_COORD_TYPE)text_metrics.line_height};

    wchar_t spc = L' ';

    space.x = af_measure(text_metrics.face, &spc, &text_metrics).w;
    if (space.x == 0) {
      space.x = text_metrics.word_spacing;
    }

    space = pp_point_transform(&space, t);
    carriage_return = pp_point_transform(&carriage_return, t);
  
    pp_point_t initial_carriage_return = carriage_return;

    size_t i = 0;

    while(i < text.length()) {
      size_t next_space = text.find(' ', i + 1);

      if(next_space == std::string::npos) {
        next_space = text.length();
      }

      size_t next_linebreak = text.find('\n', i + 1);

      if(next_linebreak == std::string::npos) {
        next_linebreak = text.length();
      }

      size_t next_break = std::min(next_space, next_linebreak);

      uint16_t word_width = 0;
      for(size_t j = i; j < next_break; j++) {
        word_width += af_measure(text_metrics.face, &text[j], &text_metrics).w;
        word_width += text_metrics.letter_spacing;
      }

      if(caret.x != 0 && caret.x + word_width > graphics->clip.w) {
        caret = pp_point_sub(&caret, &carriage_return);
        carriage_return = initial_carriage_return;
      }

      for(size_t j = i; j < std::min(next_break + 1, text.length()); j++) {
        if (text[j] == L'\n') { // Linebreak
          caret = pp_point_sub(&caret, &carriage_return);
          carriage_return = initial_carriage_return;
        } else if (text[j] == L' ') { // Space
          caret = pp_point_add(&caret, &space);
          carriage_return = pp_point_add(&carriage_return, &space);
        } else {
          // apply the caret offset...
          pp_mat3_t pos = pp_mat3_identity();
          pp_mat3_mul(&pos, t);
          pp_mat3_translate(&pos, caret.x, caret.y);
          text_metrics.transform = &pos;
          af_render_character(text_metrics.face, text[j], &text_metrics);
        }
        pp_point_t advance = {
          (PP_COORD_TYPE)af_measure(text_metrics.face, (const wchar_t *)text[j], &text_metrics).w + text_metrics.letter_spacing,
          (PP_COORD_TYPE)0
        };
        advance = pp_point_transform(&advance, t);
        caret = pp_point_add(&caret, &advance);
        carriage_return = pp_point_add(&carriage_return, &advance);
      }

      i = next_break + 1;
    }

    return {caret.x, caret.y};
  }
}