#include <cstdint>
#include <algorithm>
#include <optional>
#include <cstring>
#include <new>



using namespace std;

#ifdef PP_DEBUG
#define debug(...) printf(__VA_ARGS__)
#else
#define debug(...)
#endif

namespace pretty_poly {

  //typedef void (*tile_callback_t)(const tile_t &tile);

  typedef std::function<void(const tile_t &tile)> tile_callback_t;

  // buffer that each tile is rendered into before callback
  constexpr unsigned tile_buffer_size = 1024;
  //uint8_t tile_buffer[tile_buffer_size];
  uint8_t *tile_buffer;

  // polygon node buffer handles at most 16 line intersections per scanline
  // is this enough for cjk/emoji? (requires a 2kB buffer)
  constexpr unsigned node_buffer_size = 32;

  //int nodes[node_buffer_size][32];
  //unsigned node_counts[node_buffer_size];

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

  size_t buffer_size() {
    return tile_buffer_size + (node_buffer_size * sizeof(unsigned)) + (node_buffer_size * 32 * sizeof(int));
  }

  void init(void *memory) {
    tile_buffer = new(memory) uint8_t[tile_buffer_size];
    node_counts = new(memory + tile_buffer_size) unsigned[node_buffer_size];
    nodes = new(memory + tile_buffer_size + (node_buffer_size * sizeof(unsigned))) int[node_buffer_size][32];
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
  __attribute__((always_inline)) int sign(int v) {
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
      swap(sy, ey);
      swap(sx, ex);
    }

    /*sx <<= settings::antialias;
    ex <<= settings::antialias;
    sy <<= settings::antialias;
    ey <<= settings::antialias;*/

    int x = sx;
    int y = sy;
    int e = 0;

    int xinc = sign(ex - sx);
    int einc = abs(ex - sx) + 1;

    // todo: preclamp sy and ey (and therefore count) no need to perform
    // that test inside the loop
    int dy = ey - sy;
    int count = dy;
    debug("      + line segment from %d, %d to %d, %d\n", sx, sy, ex, ey);
    // loop over scanlines
    while(count--) {
      // consume accumulated error
      while(e > dy) {e -= dy; x += xinc;}

      if(y >= 0 && y < node_buffer_size) {  
        // clamp node x value to tile bounds
        int nx = max(min(x, (int)(tile_bounds.w << settings::antialias)), 0);        
        debug("      + adding node at %d, %d\n", x, y);
        // add node to node list
        nodes[y][node_counts[y]++] = nx;
      }

      // step to next scanline and accumulate error
      y++;
      e += einc;
    }
  }

  template<typename T>
  void build_nodes(const contour_t<T> &contour, const tile_t &tile, point_t<int> origin = point_t<int>(0, 0), int scale = 65536) {
    int ox = (origin.x - tile.bounds.x) << settings::antialias;
    int oy = (origin.y - tile.bounds.y) << settings::antialias;

    // start with the last point to close the loop
    point_t<int> last(
      (((contour.points[contour.count - 1].x * scale) << settings::antialias) / 65536) + ox, 
      (((contour.points[contour.count - 1].y * scale) << settings::antialias) / 65536) + oy
    );

    for(int i = 0; i < contour.count; i++) {
      point_t<int> point(
        (((contour.points[i].x * scale) << settings::antialias) / 65536) + ox,
        (((contour.points[i].y * scale) << settings::antialias) / 65536) + oy
      );

      add_line_segment_to_nodes(last, point);
      
      last = point;
    }
  }

  void render_nodes(const tile_t &tile) {
    for(auto y = 0; y < node_buffer_size; y++) {
      if(node_counts[y] == 0) {
        continue;
      }

      std::sort(&nodes[y][0], &nodes[y][0] + node_counts[y]);

      for(auto i = 0; i < node_counts[y]; i += 2) {
        int sx = nodes[y][i + 0];
        int ex = nodes[y][i + 1];

        if(sx == ex) {
          continue;
        }

        debug(" - render span at %d from %d to %d\n", y, sx, ex);

        for(int x = sx; x < ex; x++) {
          tile.data[(x >> settings::antialias) + (y >> settings::antialias) * tile.stride]++;
        }       
      }
    }
  }

  template<typename T>
  void draw_polygon(T *points, unsigned count) {
    std::vector<contour_t<T>> contours;
    contour_t<T> c(points, count);
    contours.push_back(c);
    draw_polygon<T>(contours);
  }
  
  template<typename T>
  void draw_polygon(std::vector<contour_t<T>> contours, point_t<int> origin = point_t<int>(0, 0), int scale = 65536) {    

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


    memset(nodes, 0, node_buffer_size * sizeof(unsigned) * 32);

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
        for(contour_t<T> &contour : contours) {
          debug("    : build nodes for contour\n");
          build_nodes(contour, tile, origin, scale);
        }

        debug("    : render the tile\n");
        // render the tile
        render_nodes(tile);

        settings::callback(tile);
      }
    }
  }
}
