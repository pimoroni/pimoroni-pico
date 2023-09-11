#pragma once
#include <cstdint>
#include <algorithm>
#include <optional>
#include <cstring>
#include <new>
#include <filesystem>
#include <fstream>
#include <functional>

#include "pretty_poly_types.hpp"

namespace pretty_poly {
 
  class file_io {
    private:
      void *state;
      size_t filesize = 0;
  
    public:
      file_io(std::string_view path);
      ~file_io();
      size_t seek(size_t pos);
      size_t read(void *buf, size_t len);
      size_t tell();
      bool fail();
  };

  // buffer that each tile is rendered into before callback
  constexpr unsigned tile_buffer_size = 1024;

  // polygon node buffer handles at most 16 line intersections per scanline
  // is this enough for cjk/emoji? (requires a 2kB buffer)
  constexpr unsigned node_buffer_size = 32;

  typedef std::function<void(const tile_t &tile)> tile_callback_t;

  // user settings
  namespace settings {
    extern rect_t clip;
    extern tile_callback_t callback;
    extern antialias_t antialias;
  }

  constexpr size_t buffer_size() {
    return tile_buffer_size + (node_buffer_size * sizeof(unsigned)) + (node_buffer_size * 32 * sizeof(int));
  }

  constexpr size_t buffer_size();

  void init(void *memory);

  void set_options(tile_callback_t callback, antialias_t antialias, rect_t clip);

  // dy step (returns 1, 0, or -1 if the supplied value is > 0, == 0, < 0)
  inline constexpr int sign(int v);

  // write out the tile bits
  void debug_tile(const tile_t &tile);

  void add_line_segment_to_nodes(const point_t<int> &start, const point_t<int> &end);

  template<typename T>
  void build_nodes(const contour_t<T> &contour, const tile_t &tile, point_t<int> origin = point_t<int>(0, 0), int scale = 65536);

  void render_nodes(const tile_t &tile, rect_t &bounds);

  template<typename T>
  void draw_polygon(T *points, unsigned count);

  template<typename T>
  void draw_polygon(const std::vector<contour_t<T>>& contours, point_t<int> origin = point_t<int>(0, 0), int scale = 65536);
}