#define PP_IMPLEMENTATION
#define AF_IMPLEMENTATION
#define PPP_IMPLEMENTATION
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
    for (auto j = 0; j < path->count; j++) {
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
    pp_path_t *path = poly->paths;
    while(path) {
      transform(path, t);
      path = path->next;
    }
  }

  pp_point_t PicoVector::text(std::string_view text, int max_width, int max_height, pp_mat3_t *t) {
    pp_point_t caret = {0, 0};

    text_metrics.transform = t;

    af_render(text_metrics.face, text.data(), text.size(), max_width, max_height, &text_metrics);

    return caret;
  }
}