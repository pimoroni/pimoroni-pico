
#include "af-file-io.h"
#include "af-memory.h"

#define AF_FILE void*
#define AF_FREAD(p, size, nmemb, stream) fileio_read(stream, p, nmemb)
#define AF_FGETC(stream) fileio_getc(stream)

#define AF_MALLOC(size)         af_malloc(size)
#define AF_REALLOC(p, size)     af_realloc(p, size)
#define AF_FREE(p)              af_free(p)

#define PP_MALLOC(size)         af_malloc(size)
#define PP_REALLOC(p, size)     af_realloc(p, size)
#define PP_FREE(p)              af_free(p)

#define AF_DEBUG(...)          af_debug(__VA_ARGS__)

#include "pretty-poly.h"
#include "alright-fonts.h"
#include "pico_graphics.hpp"

pp_rect_t pp_contour_bounds(const pp_path_t *c);

namespace pimoroni {

    // Integer point types cause compound error in transformations
    typedef float picovector_point_type;

    class PicoVector {
        private:
            static PicoGraphics *graphics;
            af_text_metrics_t text_metrics;
            static constexpr uint8_t alpha_map_x4[4] {0, 128, 192, 255};
            static constexpr uint8_t alpha_map_x16[16] {0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 255};
            static uint8_t max_alpha;
            static const uint8_t *alpha_map;

        public:
            PicoVector(PicoGraphics *graphics, void *mem = nullptr) {
                PicoVector::graphics = graphics;

                pp_tile_callback(PicoVector::tile_callback);

                set_antialiasing(graphics->supports_alpha_blend() ? PP_AA_X4 : PP_AA_NONE);

                pp_clip(graphics->clip.x, graphics->clip.y, graphics->clip.w, graphics->clip.h);

                text_metrics.align = AF_H_ALIGN_LEFT;
                text_metrics.line_height = 110;
                text_metrics.letter_spacing = 95;
                text_metrics.word_spacing = 200;
                text_metrics.size = 48;
                // Shoud be set before rendering chars
                //text_metrics.transform = (pp_mat3_t *)af_malloc(sizeof(pp_mat3_t));
                //*text_metrics.transform = pp_mat3_identity();
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
                            if (alpha >= max_alpha) {
                                PicoVector::graphics->pixel({x + tile->x, y + tile->y});
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
                                PicoVector::graphics->pixel({x + tile->x, y + tile->y});
                            }
                        }
                        tile_data += tile->stride - tile->w;
                    }
                }
            }

            void set_antialiasing(pp_antialias_t antialias) {
                pp_antialias(antialias);
                if(antialias == PP_AA_X16) {
                    alpha_map = alpha_map_x16;
                    max_alpha = 16;
                } else {
                    alpha_map = alpha_map_x4;
                    max_alpha = 4;
                }
            }

            void set_font_size(unsigned int font_size) {
                text_metrics.size = font_size;
            }

            bool set_font(std::string_view font_path, unsigned int font_size) {
                if(text_metrics.face) {
                    af_free(text_metrics.face->glyphs);
                    af_free(text_metrics.face);
                }
                text_metrics.face = (af_face_t *)af_malloc(sizeof(af_face_t));
                //bool result = text_metrics.face.load(font_path);
                void* font = fileio_open(font_path.data());
                bool result = af_load_font_file(font, text_metrics.face);

                set_font_size(font_size);

                return result;
            }

            bool set_font(void* font,  unsigned int font_size) {
                if(text_metrics.face) {
                    af_free(text_metrics.face->glyphs);
                    af_free(text_metrics.face);
                }
                text_metrics.face = (af_face_t *)af_malloc(sizeof(af_face_t));
                bool result = af_load_font_file(font, text_metrics.face);

                set_font_size(font_size);

                return result;
            }

            pp_point_t text(std::string_view text, pp_mat3_t *t=nullptr);

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