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


                if(graphics->supports_alpha_blend()) {
                    set_options([this](const pretty_poly::tile_t &tile) -> void {
                        for(auto y = 0; y < tile.bounds.h; y++) {
                            for(auto x = 0; x < tile.bounds.w; x++) {
                                uint8_t alpha = tile.get_value(x, y);
                                if (alpha >= 4) {
                                    this->graphics->pixel({x + tile.bounds.x, y + tile.bounds.y});
                                } else if (alpha > 0) {
                                    alpha = alpha_map[alpha];
                                    this->graphics->set_pixel_alpha({x + tile.bounds.x, y + tile.bounds.y}, alpha);
                                }
                            }
                        }
                    }, pretty_poly::X4, {0, 0, graphics->bounds.w, graphics->bounds.h});
                } else {
                    set_options([this](const pretty_poly::tile_t &tile) -> void {
                        for(auto y = 0; y < tile.bounds.h; y++) {
                            for(auto x = 0; x < tile.bounds.w; x++) {
                                uint8_t alpha = tile.get_value(x, y);
                                if (alpha > 0) {
                                    this->graphics->pixel({x + tile.bounds.x, y + tile.bounds.y});
                                }
                            }
                        }
                    }, pretty_poly::NONE, {0, 0, graphics->bounds.w, graphics->bounds.h});
                }
            };

            bool set_font(std::string_view font_path, unsigned int font_size) {
                bool result = text_metrics.face.load(font_path);

                text_metrics.set_size(font_size);

                return result;
            }

            Point text(std::string_view text, Point origin);

            void polygon(std::vector<pretty_poly::contour_t<int>> contours, Point origin = Point(0, 0), int scale=65536);

            static constexpr size_t pretty_poly_buffer_size() {
                return pretty_poly::buffer_size();
            };
    };
}