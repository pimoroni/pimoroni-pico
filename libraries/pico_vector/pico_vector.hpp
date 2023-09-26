#include "pretty_poly/pretty-poly.h"
#include "alright_fonts.hpp"
#include "pico_graphics.hpp"

namespace pimoroni {

    // Integer point types cause compound error in transformations
    typedef float picovector_point_type;

    class PicoVector {
        private:
            static PicoGraphics *graphics;
            alright_fonts::text_metrics_t text_metrics;
            static constexpr uint8_t alpha_map[4] {0, 128, 192, 255};

        public:
            PicoVector(PicoGraphics *graphics, void *mem = nullptr) {
                PicoVector::graphics = graphics;

                pp_tile_callback(PicoVector::tile_callback);

                pp_antialias(graphics->supports_alpha_blend() ? PP_AA_X4 : PP_AA_NONE);

                pp_clip(graphics->clip.x, graphics->clip.y, graphics->clip.w, graphics->clip.h);
            }

            static void tile_callback(const pp_tile_t *tile) {
                uint8_t *tile_data = tile->data;

                if(PicoVector::graphics->supports_alpha_blend() && _pp_antialias != PP_AA_NONE) {
                    if (PicoVector::graphics->render_pico_vector_tile({tile->x, tile->y, tile->w, tile->h},
                                                                tile->data,
                                                                tile->stride,
                                                                (uint8_t)_pp_antialias)) {
                        return;
                    }
                    for(auto y = 0; y < tile->h; y++) {
                        for(auto x = 0; x < tile->w; x++) {
                            uint8_t alpha = *tile_data++;
                            if (alpha >= 4) {
                                PicoVector::graphics->set_pixel({x + tile->x, y + tile->y});
                            } else if (alpha > 0) {
                                alpha = alpha_map[alpha];
                                PicoVector::graphics->set_pixel_alpha({x + tile->x, y + tile->y}, alpha);
                            }
                        }
                        tile_data += tile->stride - tile->w;
                    }
                } else {
                    for(auto y = 0; y < tile->h; y++) {
                        for(auto x = 0; x < tile->w; x++) {
                            uint8_t alpha = *tile_data++;
                            if (alpha) {
                                PicoVector::graphics->set_pixel({x + tile->x, y + tile->y});
                            }
                        }
                        tile_data += tile->stride - tile->w;
                    }
                }
            }

            void set_antialiasing(pp_antialias_t antialias) {
                pp_antialias(antialias);
            }

            void set_font_size(unsigned int font_size) {
                text_metrics.set_size(font_size);
            }

            bool set_font(std::string_view font_path, unsigned int font_size) {
                bool result = text_metrics.face.load(font_path);

                set_font_size(font_size);

                return result;
            }

            pp_point_t text(std::string_view text, pp_point_t origin, pp_mat3_t *t);

            void transform(pp_path_t *path, pp_mat3_t *t);
            void transform(pp_poly_t *poly, pp_mat3_t *t);

            void rotate(pp_path_t *path, pp_point_t origin, float angle);
            void rotate(pp_poly_t *poly, pp_point_t origin, float angle);

            void translate(pp_path_t *path, pp_point_t translation);
            void translate(pp_poly_t *poly, pp_point_t translation);

            void draw(pp_poly_t *poly);
            void draw(pp_poly_t *poly, pp_mat3_t *t);

            void draw(pp_path_t *path) {
                pp_poly_t poly = {.paths = path, .count = 1};
                draw(&poly);
            };

            void draw(pp_path_t *path, pp_mat3_t *t) {
                pp_poly_t poly = {.paths = path, .count = 1};
                draw(&poly, t);
            };

            static constexpr size_t pretty_poly_buffer_size() {
                return 0;
            };
    };
}