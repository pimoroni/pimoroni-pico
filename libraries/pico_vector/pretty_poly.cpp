#include <cstdint>
#include <algorithm>
#include <optional>
#include <cstring>
#include <new>
#include <filesystem>
#include <fstream>

#include "pretty_poly.hpp"

#include "hardware/interp.h"

#ifdef PP_DEBUG
#define debug(...) printf(__VA_ARGS__)
#else
#define debug(...)
#endif

namespace pretty_poly {

  uint8_t *tile_buffer;

  int (*nodes)[32];
  unsigned *node_counts;

  // default tile bounds to X1 antialiasing
  rect_t tile_bounds(0, 0, tile_buffer_size / node_buffer_size, node_buffer_size);

  // user settings
  namespace settings {
    rect_t clip(0, 0, 320, 240);
    tile_callback_t callback;
    antialias_t antialias = antialias_t::NONE;
  }

  void init(void *memory) {
    uintptr_t m = (uintptr_t)memory;
    tile_buffer = new(memory) uint8_t[tile_buffer_size];
    node_counts = new((void *)(m + tile_buffer_size)) unsigned[node_buffer_size];
    nodes = new((void *)(m + tile_buffer_size + (node_buffer_size * sizeof(unsigned)))) int[node_buffer_size][32];
  }

  void set_options(tile_callback_t callback, antialias_t antialias, rect_t clip) {
    settings::callback = callback;
    settings::antialias = antialias;
    settings::clip = clip;

    // recalculate the tile size for rendering based on antialiasing level
    int tile_height = node_buffer_size >> antialias;
    tile_bounds = rect_t(0, 0, tile_buffer_size / tile_height, tile_height);
  }

  // dy step (returns 1, 0, or -1 if the supplied value is > 0, == 0, < 0)
  inline constexpr int sign(int v) {
    // assumes 32-bit int/unsigned
    return ((unsigned)-v >> 31) - ((unsigned)v >> 31);
  }

  // write out the tile bits
  void debug_tile(const tile_t &tile) {
    debug("  - tile %d, %d (%d x %d)\n", tile.bounds.x, tile.bounds.y, tile.bounds.w, tile.bounds.h);
    for(auto y = 0; y < tile.bounds.h; y++) {
      debug("[%3d]: ", y);
      for(auto x = 0; x < tile.bounds.w; x++) {
        debug("%d", tile.get_value(x, y));
      }  
      debug("\n");              
    }
    debug("-----------------------\n");
  }

  void add_line_segment_to_nodes(const point_t<int> &start, const point_t<int> &end) {
    // swap endpoints if line "pointing up", we do this because we
    // alway skip the last scanline (so that polygons can but cleanly
    // up against each other without overlap)
    int sx = start.x, sy = start.y, ex = end.x, ey = end.y;

    if(ey < sy) {
      std::swap(sy, ey);
      std::swap(sx, ex);
    }

    // Early out if line is completely outside the tile, or has no lines
    if (ey < 0 || sy >= (int)node_buffer_size || sy == ey) return;

    debug("      + line segment from %d, %d to %d, %d\n", sx, sy, ex, ey);

    // Determine how many in-bounds lines to render
    int y = std::max(0, sy);
    int count = std::min((int)node_buffer_size, ey) - y;

    // Handle cases where x is completely off to one side or other
    if (std::max(sx, ex) <= 0) {
      while (count--) {
        nodes[y][node_counts[y]++] = 0;
        ++y;
      }
      return;
    }

    const int full_tile_width = (tile_bounds.w << settings::antialias);
    if (std::min(sx, ex) >= full_tile_width) {
      while (count--) {
        nodes[y][node_counts[y]++] = full_tile_width;
        ++y;
      }
      return;
    }

    // Normal case
    int x = sx;
    int e = 0;

    const int xinc = sign(ex - sx);
    const int einc = abs(ex - sx) + 1;
    const int dy = ey - sy;

    // If sy < 0 jump to the start, note this does use a divide
    // but potentially saves many wasted loops below, so is likely worth it.
    if (sy < 0) {
      e = einc * -sy;
      int xjump = e / dy;
      e -= dy * xjump;
      x += xinc * xjump;
    }

    interp1->base[1] = full_tile_width;
    interp1->accum[0] = x;

    // loop over scanlines
    while(count--) {
      // consume accumulated error
      while(e > dy) {e -= dy; interp1->add_raw[0] = xinc;}

      // clamp node x value to tile bounds
      const int nx = interp1->peek[0];
      debug("      + adding node at %d, %d\n", x, y);
      // add node to node list
      nodes[y][node_counts[y]++] = nx;

      // step to next scanline and accumulate error
      y++;
      e += einc;
    }
  }

  template<typename T>
  void build_nodes(const contour_t<T> &contour, const tile_t &tile, point_t<int> origin, int scale) {
    int ox = (origin.x - tile.bounds.x) << settings::antialias;
    int oy = (origin.y - tile.bounds.y) << settings::antialias;

    // start with the last point to close the loop
    point_t<int> last(
      (((int(contour.points[contour.count - 1].x) * scale) << settings::antialias) / 65536) + ox,
      (((int(contour.points[contour.count - 1].y) * scale) << settings::antialias) / 65536) + oy
    );

    for(auto i = 0u; i < contour.count; i++) {
      point_t<int> point(
        (((int(contour.points[i].x) * scale) << settings::antialias) / 65536) + ox,
        (((int(contour.points[i].y) * scale) << settings::antialias) / 65536) + oy
      );

      add_line_segment_to_nodes(last, point);
      
      last = point;
    }
  }

  void render_nodes(const tile_t &tile, rect_t &bounds) {
    int maxy = -1;
    bounds.y = 0;
    bounds.x = tile.bounds.w;
    int maxx = 0;
    int anitialias_mask = (1 << settings::antialias) - 1;

    for(auto y = 0; y < (int)node_buffer_size; y++) {
      if(node_counts[y] == 0) {
        if (y == bounds.y) ++bounds.y;
        continue;
      }

      std::sort(&nodes[y][0], &nodes[y][0] + node_counts[y]);

      uint8_t* row_data = &tile.data[(y >> settings::antialias) * tile.stride];
      bool rendered_any = false;
      for(auto i = 0u; i < node_counts[y]; i += 2) {
        int sx = nodes[y][i + 0];
        int ex = nodes[y][i + 1];

        if(sx == ex) {
          continue;
        }

        rendered_any = true;

        maxx = std::max((ex - 1) >> settings::antialias, maxx);

        debug(" - render span at %d from %d to %d\n", y, sx, ex);

        if (settings::antialias) {
          int ax = sx >> settings::antialias;
          const int aex = ex >> settings::antialias;

          bounds.x = std::min(ax, bounds.x);

          if (ax == aex) {
            row_data[ax] += ex - sx;
            continue;
          }

          row_data[ax] += (1 << settings::antialias) - (sx & anitialias_mask);
          for(ax++; ax < aex; ax++) {
            row_data[ax] += (1 << settings::antialias);
          }

          // This might add 0 to the byte after the end of the row, we pad the tile data
          // by 1 byte to ensure that is OK
          row_data[ax] += ex & anitialias_mask;
        }
        else {
          bounds.x = std::min(sx, bounds.x);
          for(int x = sx; x < ex; x++) {
            row_data[x]++;
          }       
        }
      }

      if (rendered_any) {
        debug("  - rendered line %d\n", y);
        maxy = y;
      }
      else if (y == bounds.y) {
        debug(" - render nothing on line %d\n", y);
        ++bounds.y;
      }
    }

    bounds.y >>= settings::antialias;
    maxy >>= settings::antialias;
    bounds.w = (maxx >= bounds.x) ? maxx + 1 - bounds.x : 0;
    bounds.h = (maxy >= bounds.y) ? maxy + 1 - bounds.y : 0;
    debug(" - rendered tile bounds %d, %d (%d x %d)\n", bounds.x, bounds.y, bounds.w, bounds.h);
  }

  template<typename T>
  void draw_polygon(T *points, unsigned count) {
    std::vector<contour_t<T>> contours;
    contour_t<T> c(points, count);
    contours.push_back(c);
    draw_polygon<T>(contours);
  }
  
  template<typename T>
  void draw_polygon(const std::vector<contour_t<T>>& contours, point_t<int> origin, int scale) {    

    debug("> draw polygon with %lu contours\n", contours.size());

    if(contours.size() == 0) {
      return;
    }

    // determine extreme bounds
    rect_t polygon_bounds = contours[0].bounds();
    for(auto &contour : contours) {
      polygon_bounds = polygon_bounds.merge(contour.bounds());
    }

    polygon_bounds.x = ((polygon_bounds.x * scale) / 65536) + origin.x;
    polygon_bounds.y = ((polygon_bounds.y * scale) / 65536) + origin.y;
    polygon_bounds.w = ((polygon_bounds.w * scale) / 65536);
    polygon_bounds.h = ((polygon_bounds.h * scale) / 65536);

    debug("  - bounds %d, %d (%d x %d)\n", polygon_bounds.x, polygon_bounds.y, polygon_bounds.w, polygon_bounds.h);
    debug("  - clip %d, %d (%d x %d)\n", settings::clip.x, settings::clip.y, settings::clip.w, settings::clip.h);

    interp_hw_save_t interp1_save;
    interp_save(interp1, &interp1_save);

    interp_config cfg = interp_default_config();
    interp_config_set_clamp(&cfg, true);
    interp_config_set_signed(&cfg, true);
    interp_set_config(interp1, 0, &cfg);
    interp1->base[0] = 0;

    //memset(nodes, 0, node_buffer_size * sizeof(unsigned) * 32);

    // iterate over tiles
    debug("  - processing tiles\n");
    for(auto y = polygon_bounds.y; y < polygon_bounds.y + polygon_bounds.h; y += tile_bounds.h) {
      for(auto x = polygon_bounds.x; x < polygon_bounds.x + polygon_bounds.w; x += tile_bounds.w) {
        tile_t tile;
        tile.bounds = rect_t(x, y, tile_bounds.w, tile_bounds.h).intersection(settings::clip);
        tile.stride = tile_bounds.w;
        tile.data = tile_buffer;
        debug("    : %d, %d (%d x %d)\n", tile.bounds.x, tile.bounds.y, tile.bounds.w, tile.bounds.h);

        // if no intersection then skip tile
        if(tile.bounds.empty()) {
          debug("    : empty when clipped, skipping\n");
          continue;
        }

        // clear existing tile data and nodes
        memset(node_counts, 0, node_buffer_size * sizeof(unsigned));
        memset(tile.data, 0, tile_buffer_size);

        // build the nodes for each contour
        for(const contour_t<T> &contour : contours) {
          debug("    : build nodes for contour\n");
          build_nodes(contour, tile, origin, scale);
        }

        debug("    : render the tile\n");
        // render the tile
        rect_t bounds;
        render_nodes(tile, bounds);
        if (bounds.empty()) {
          continue;
        }

        tile.data += bounds.x + tile.stride * bounds.y;
        tile.bounds.x += bounds.x;
        tile.bounds.y += bounds.y;
        tile.bounds.w = bounds.w;
        tile.bounds.h = bounds.h;

        settings::callback(tile);
      }
    }

    interp_restore(interp1, &interp1_save);
  }
}

template void pretty_poly::draw_polygon<int>(const std::vector<contour_t<int>>& contours, point_t<int> origin, int scale);
template void pretty_poly::draw_polygon<float>(const std::vector<contour_t<float>>& contours, point_t<int> origin, int scale);
template void pretty_poly::draw_polygon<uint8_t>(const std::vector<contour_t<uint8_t>>& contours, point_t<int> origin, int scale);
template void pretty_poly::draw_polygon<int8_t>(const std::vector<contour_t<int8_t>>& contours, point_t<int> origin, int scale);