#include "pretty_poly.hpp"
#include "alright_fonts.hpp"
#include "pico_graphics.hpp"

namespace pimoroni {
    class PicoVector {
        private:
            PicoGraphics *graphics;
            alright_fonts::text_metrics_t text_metrics;
            const uint8_t alpha_map[4] {0, 128, 192, 255};

        public:
            PicoVector(PicoGraphics *graphics, void *mem = nullptr) : graphics(graphics) {
                pretty_poly::init(mem);

                set_options([this](const pretty_poly::tile_t &tile) -> void {
                    uint8_t *tile_data = tile.data;

                    if(this->graphics->supports_alpha_blend() && pretty_poly::settings::antialias != pretty_poly::NONE) {
                        for(auto y = 0; y < tile.bounds.h; y++) {
                            for(auto x = 0; x < (int)tile.stride; x++) {
                                uint8_t alpha = *tile_data++;
                                if(x > tile.bounds.w) continue;
                                if (alpha >= 4) {
                                    this->graphics->pixel({x + tile.bounds.x, y + tile.bounds.y});
                                } else if (alpha > 0) {
                                    alpha = alpha_map[alpha];
                                    this->graphics->set_pixel_alpha({x + tile.bounds.x, y + tile.bounds.y}, alpha);
                                }
                            }
                        }
                    } else {
                        for(auto y = 0; y < tile.bounds.h; y++) {
                            for(auto x = 0; x < (int)tile.stride; x++) {
                                uint8_t alpha =  *tile_data++;
                                if(x > tile.bounds.w) continue;
                                if (alpha) {
                                    this->graphics->pixel({x + tile.bounds.x, y + tile.bounds.y});
                                }
                            }
                        }
                    }
                }, graphics->supports_alpha_blend() ? pretty_poly::X4 : pretty_poly::NONE, {0, 0, graphics->bounds.w, graphics->bounds.h});
            }

            void set_antialiasing(pretty_poly::antialias_t antialias) {
                set_options(pretty_poly::settings::callback, antialias, pretty_poly::settings::clip);
            }

            void set_font_size(unsigned int font_size) {
                text_metrics.set_size(font_size);
            }

            bool set_font(std::string_view font_path, unsigned int font_size) {
                bool result = text_metrics.face.load(font_path);

                set_font_size(font_size);

                return result;
            }

            void rotate(std::vector<pretty_poly::contour_t<int>> &contours, Point origin, float angle);
            void translate(std::vector<pretty_poly::contour_t<int>> &contours, Point translation);

            Point text(std::string_view text, Point origin);

            void polygon(std::vector<pretty_poly::contour_t<int>> contours, Point origin = Point(0, 0), int scale=65536);

            static constexpr size_t pretty_poly_buffer_size() {
                return pretty_poly::buffer_size();
            };
    };
}