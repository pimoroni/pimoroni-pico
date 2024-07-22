
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
                // TODO: we're using a cast here to avoid a hard dependency link between
                // PicoGraphics and PicoVector. These types might subtly mismatch, though...
                PicoVector::graphics->render_tile((pimoroni::Tile *)tile);
            }

            void set_antialiasing(pp_antialias_t antialias) {
                pp_antialias(antialias);
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
                pp_poly_t *poly = pp_poly_new();
                poly->paths = path;
                draw(poly);
                poly->paths = NULL; // Don't free our non-owned path
                pp_poly_free(poly);
            };

            void draw(pp_path_t *path, pp_mat3_t *t) {
                pp_poly_t *poly = pp_poly_new();
                poly->paths = path;
                draw(poly, t);
                poly->paths = NULL; // Don't free our non-owned path
                pp_poly_free(poly);
            };

            static constexpr size_t pretty_poly_buffer_size() {
                return 0;
            };
    };
}