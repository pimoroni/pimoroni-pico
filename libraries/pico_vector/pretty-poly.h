/*

  Pretty Poly 🦜 - super-sampling polygon renderer for low resource platforms.

  Jonathan Williamson, August 2022
  Examples, source, and more: https://github.com/lowfatcode/pretty-poly
  MIT License https://github.com/lowfatcode/pretty-poly/blob/main/LICENSE

  An easy way to render high quality graphics in embedded applications running 
  on resource constrained microcontrollers such as the Cortex M0 and up.         

    - Renders polygons: concave, self-intersecting, multi contour, holes, etc.
    - C11 header only library: simply copy the header file into your project
    - Tile based renderer: low memory footprint, cache coherency
    - Low memory usage: ~4kB of heap memory required
    - High speed on low resource platforms: optionally no floating point
    - Antialiasing modes: X1 (none), X4 and X16 super sampling
    - Bounds clipping: all results clipped to supplied clip rectangle
    - Pixel format agnostic: renders a "tile" to blend into your framebuffer
    - Support for hardware interpolators on rp2040 (thanks @MichaelBell!)

  Contributor bwaaaaaarks! 🦜

    @MichaelBell - lots of bug fixes, performance boosts, and suggestions. 
    @gadgetoid - integrating into the PicoVector library and testing.
    
*/

#ifndef PP_INCLUDE_H
#define PP_INCLUDE_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#ifndef PP_MALLOC
#define PP_MALLOC(size)         malloc(size)
#define PP_REALLOC(p, size)     realloc(p, size)
#define PP_FREE(p)              free(p)
#endif

#ifndef PP_COORD_TYPE
#define PP_COORD_TYPE float
#endif

#ifndef PP_NODE_BUFFER_HEIGHT
#define PP_NODE_BUFFER_HEIGHT 16
#endif

#ifndef PP_MAX_NODES_PER_SCANLINE
#define PP_MAX_NODES_PER_SCANLINE 16
#endif

#ifndef PP_TILE_BUFFER_SIZE
#define PP_TILE_BUFFER_SIZE 4096
#endif

#if defined(PICO_ON_DEVICE) && PICO_ON_DEVICE
#define USE_RP2040_INTERP
#include "hardware/interp.h"
#endif

#ifdef PP_DEBUG
#define debug(...) printf(__VA_ARGS__)
#else
#define debug(...)
#endif

#ifdef __cplusplus
extern "C" {
#endif

// 3x3 matrix type allows for optional transformation of polygon during render
typedef struct {
  float v00, v10, v20, v01, v11, v21, v02, v12, v22;
} pp_mat3_t;
pp_mat3_t pp_mat3_identity();
void pp_mat3_rotate(pp_mat3_t *m, float a);
void pp_mat3_rotate_rad(pp_mat3_t *m, float a);
void pp_mat3_translate(pp_mat3_t *m, float x, float y);
void pp_mat3_scale(pp_mat3_t *m, float x, float y);
void pp_mat3_mul(pp_mat3_t *m1, pp_mat3_t *m2);

// point type used to hold polygon vertex coordinates
typedef struct __attribute__((__packed__)) pp_point_t {
  PP_COORD_TYPE x, y;
} pp_point_t;
pp_point_t pp_point_add(pp_point_t *p1, pp_point_t *p2);
pp_point_t pp_point_sub(pp_point_t *p1, pp_point_t *p2);
pp_point_t pp_point_mul(pp_point_t *p1, pp_point_t *p2);
pp_point_t pp_point_div(pp_point_t *p1, pp_point_t *p2);
pp_point_t pp_point_transform(pp_point_t *p, pp_mat3_t *m);

// rect type
typedef struct {
  int32_t x, y, w, h;    
} pp_rect_t;
bool pp_rect_empty(pp_rect_t *r);
pp_rect_t pp_rect_intersection(pp_rect_t *r1, pp_rect_t *r2);
pp_rect_t pp_rect_merge(pp_rect_t *r1, pp_rect_t *r2);
pp_rect_t pp_rect_transform(pp_rect_t *r, pp_mat3_t *m);

// antialias levels
typedef enum {PP_AA_NONE = 0, PP_AA_X4 = 1, PP_AA_X16 = 2} pp_antialias_t;

typedef struct {
  int32_t x, y, w, h;
  uint32_t stride;
  uint8_t *data;
} pp_tile_t;

typedef struct {
  pp_point_t *points;
  uint32_t count;
} pp_path_t;

typedef struct {
  pp_path_t *paths;
  uint32_t count;
} pp_poly_t;

// user settings
typedef void (*pp_tile_callback_t)(const pp_tile_t *tile);

extern pp_rect_t           _pp_clip;
extern pp_tile_callback_t  _pp_tile_callback;
extern pp_antialias_t      _pp_antialias;
extern pp_mat3_t          *_pp_transform;

void pp_clip(int32_t x, int32_t y, int32_t w, int32_t h);
void pp_tile_callback(pp_tile_callback_t callback);
void pp_antialias(pp_antialias_t antialias);
pp_mat3_t *pp_transform(pp_mat3_t *transform);
void pp_render(pp_poly_t *polygon);

pp_rect_t pp_contour_bounds(const pp_path_t *c);
pp_rect_t pp_polygon_bounds(pp_poly_t *p);

#ifdef __cplusplus
}
#endif

#ifdef PP_IMPLEMENTATION

pp_rect_t           _pp_clip = (pp_rect_t){0, 0, 320, 240};
pp_tile_callback_t  _pp_tile_callback = NULL;
pp_antialias_t      _pp_antialias = PP_AA_X4;
pp_mat3_t          *_pp_transform = NULL;

int     _pp_max(int a, int b) { return a > b ? a : b; }
int     _pp_min(int a, int b) { return a < b ? a : b; }
int     _pp_sign(int v) {return (v > 0) - (v < 0);}
void    _pp_swap(int *a, int *b) {int t = *a; *a = *b; *b = t;}

// pp_mat3_t implementation
pp_mat3_t pp_mat3_identity() {
  pp_mat3_t m; memset(&m, 0, sizeof(pp_mat3_t)); m.v00 = m.v11 = m.v22 = 1.0f; return m;}
void pp_mat3_rotate(pp_mat3_t *m, float a) {
  pp_mat3_rotate_rad(m, a * M_PI / 180.0f);}
void pp_mat3_rotate_rad(pp_mat3_t *m, float a) {
  float c = cosf(a), s = sinf(a); pp_mat3_t r = pp_mat3_identity();
  r.v00 = c; r.v01 = -s; r.v10 = s; r.v11 = c; pp_mat3_mul(m, &r); }
void pp_mat3_translate(pp_mat3_t *m, float x, float y) {
  pp_mat3_t r = pp_mat3_identity(); r.v02 = x; r.v12 = y; pp_mat3_mul(m, &r);}
void pp_mat3_scale(pp_mat3_t *m, float x, float y) {
  pp_mat3_t r = pp_mat3_identity(); r.v00 = x; r.v11 = y; pp_mat3_mul(m, &r);}
void pp_mat3_mul(pp_mat3_t *m1, pp_mat3_t *m2) {
  pp_mat3_t r;
  r.v00 = m1->v00 * m2->v00 + m1->v01 * m2->v10 + m1->v02 * m2->v20;
  r.v01 = m1->v00 * m2->v01 + m1->v01 * m2->v11 + m1->v02 * m2->v21;
  r.v02 = m1->v00 * m2->v02 + m1->v01 * m2->v12 + m1->v02 * m2->v22;
  r.v10 = m1->v10 * m2->v00 + m1->v11 * m2->v10 + m1->v12 * m2->v20;
  r.v11 = m1->v10 * m2->v01 + m1->v11 * m2->v11 + m1->v12 * m2->v21;
  r.v12 = m1->v10 * m2->v02 + m1->v11 * m2->v12 + m1->v12 * m2->v22;
  r.v20 = m1->v20 * m2->v00 + m1->v21 * m2->v10 + m1->v22 * m2->v20;
  r.v21 = m1->v20 * m2->v01 + m1->v21 * m2->v11 + m1->v22 * m2->v21;
  r.v22 = m1->v20 * m2->v02 + m1->v21 * m2->v12 + m1->v22 * m2->v22;    
  *m1 = r;
}

// pp_point_t implementation
pp_point_t pp_point_add(pp_point_t *p1, pp_point_t *p2) {
  return (pp_point_t){.x = p1->x + p2->x, .y = p1->y + p2->y};
}
pp_point_t pp_point_sub(pp_point_t *p1, pp_point_t *p2) {
  return (pp_point_t){.x = p1->x - p2->x, .y = p1->y - p2->y};
}
pp_point_t pp_point_mul(pp_point_t *p1, pp_point_t *p2) {
  return (pp_point_t){.x = p1->x * p2->x, .y = p1->y * p2->y};
}
pp_point_t pp_point_div(pp_point_t *p1, pp_point_t *p2) {
  return (pp_point_t){.x = p1->x / p2->x, .y = p1->y / p2->y};
}
pp_point_t pp_point_transform(pp_point_t *p, pp_mat3_t *m) {
  return (pp_point_t){
    .x = (m->v00 * p->x + m->v01 * p->y + m->v02),
    .y = (m->v10 * p->x + m->v11 * p->y + m->v12)
  };
}

// pp_rect_t implementation
bool pp_rect_empty(pp_rect_t *r) {
  return r->w == 0 || r->h == 0;
}
pp_rect_t pp_rect_intersection(pp_rect_t *r1, pp_rect_t *r2) {
  return (pp_rect_t){
    .x = _pp_max(r1->x, r2->x), .y = _pp_max(r1->y, r2->y),
    .w = _pp_max(0, _pp_min(r1->x + r1->w, r2->x + r2->w) - _pp_max(r1->x, r2->x)),
    .h = _pp_max(0, _pp_min(r1->y + r1->h, r2->y + r2->h) - _pp_max(r1->y, r2->y))
  };
}
pp_rect_t pp_rect_merge(pp_rect_t *r1, pp_rect_t *r2) {
  return (pp_rect_t){
    .x = _pp_min(r1->x, r2->x), 
    .y = _pp_min(r1->y, r2->y),
    .w = _pp_max(r1->x + r1->w, r2->x + r2->w) - _pp_min(r1->x, r2->x),
    .h = _pp_max(r1->y + r1->h, r2->y + r2->h) - _pp_min(r1->y, r2->y)
  };
}
pp_rect_t pp_rect_transform(pp_rect_t *r, pp_mat3_t *m) {
  pp_point_t tl = {.x = (PP_COORD_TYPE)r->x, .y = (PP_COORD_TYPE)r->y};
  pp_point_t tr = {.x = (PP_COORD_TYPE)r->x + (PP_COORD_TYPE)r->w, .y = (PP_COORD_TYPE)r->y};
  pp_point_t bl = {.x = (PP_COORD_TYPE)r->x, .y = (PP_COORD_TYPE)r->y + (PP_COORD_TYPE)r->h};
  pp_point_t br = {.x = (PP_COORD_TYPE)r->x + (PP_COORD_TYPE)r->w, .y = (PP_COORD_TYPE)r->y + (PP_COORD_TYPE)r->h};

  tl = pp_point_transform(&tl, m);
  tr = pp_point_transform(&tr, m);
  bl = pp_point_transform(&bl, m);
  br = pp_point_transform(&br, m);

  PP_COORD_TYPE minx = _pp_min(tl.x, _pp_min(tr.x, _pp_min(bl.x, br.x)));
  PP_COORD_TYPE miny = _pp_min(tl.y, _pp_min(tr.y, _pp_min(bl.y, br.y)));
  PP_COORD_TYPE maxx = _pp_max(tl.x, _pp_max(tr.x, _pp_max(bl.x, br.x)));
  PP_COORD_TYPE maxy = _pp_max(tl.y, _pp_max(tr.y, _pp_max(bl.y, br.y)));

  return (pp_rect_t){
    .x = (int32_t)minx, 
    .y = (int32_t)miny, 
    .w = (int32_t)(maxx - minx), 
    .h = (int32_t)(maxy - miny)
  };
}

// pp_tile_t implementation
uint8_t pp_tile_get(const pp_tile_t *tile, const int32_t x, const int32_t y) {
  return tile->data[(x - tile->x) + (y - tile->y) * tile->stride] * (255 >> _pp_antialias >> _pp_antialias);
}

// pp_contour_t implementation
pp_rect_t pp_contour_bounds(const pp_path_t *c) {
  int minx = c->points[0].x, maxx = minx;
  int miny = c->points[0].y, maxy = miny;
  for(uint32_t i = 1; i < c->count; i++) {
    minx = _pp_min(minx, c->points[i].x);
    miny = _pp_min(miny, c->points[i].y);
    maxx = _pp_max(maxx, c->points[i].x); 
    maxy = _pp_max(maxy, c->points[i].y);
  }  
  return (pp_rect_t){.x = minx, .y = miny, .w = maxx - minx, .h = maxy - miny};
}

pp_rect_t pp_polygon_bounds(pp_poly_t *p) {
  if(p->count == 0) {return (pp_rect_t){};}
  pp_rect_t b = pp_contour_bounds(&p->paths[0]);
  for(uint32_t i = 1; i < p->count; i++) {
    pp_rect_t cb = pp_contour_bounds(&p->paths[i]);
    b = pp_rect_merge(&b, &cb);
  }
  return b;
}

// buffer that each tile is rendered into before callback
// allocate one extra byte to allow a small optimization in the row renderer
const uint32_t tile_buffer_size = PP_TILE_BUFFER_SIZE;
uint8_t tile_buffer[PP_TILE_BUFFER_SIZE + 1];

// polygon node buffer handles at most 16 line intersections per scanline
// is this enough for cjk/emoji? (requires a 2kB buffer)
int32_t nodes[PP_NODE_BUFFER_HEIGHT][PP_MAX_NODES_PER_SCANLINE * 2];
uint32_t node_counts[PP_NODE_BUFFER_HEIGHT];


void pp_clip(int32_t x, int32_t y, int32_t w, int32_t h) {
  _pp_clip = (pp_rect_t){.x = x, .y = y, .w = w, .h = h};
}

void pp_tile_callback(pp_tile_callback_t callback) {
  _pp_tile_callback = callback;
}

// maximum tile bounds determined by antialias level
uint32_t _pp_tile_width, _pp_tile_height;
void pp_antialias(pp_antialias_t antialias) {
  _pp_antialias = antialias;
  // recalculate the tile size for rendering based on antialiasing level
  _pp_tile_height = PP_NODE_BUFFER_HEIGHT >> _pp_antialias;
  _pp_tile_width = (int)(tile_buffer_size / _pp_tile_height);
}

pp_mat3_t *pp_transform(pp_mat3_t *transform) {
  pp_mat3_t *old = _pp_transform;
  _pp_transform = transform;
  return old;
}

// write out the tile bits
void debug_tile(const pp_tile_t *tile) {
  debug("  - tile %d, %d (%d x %d)\n", tile->x, tile->y, tile->w, tile->h);
  for(int32_t y = 0; y < tile->h; y++) {
    debug("[%3d]: ", y);
    for(int32_t x = 0; x < tile->w; x++) {
      debug("%02x", pp_tile_get(tile, x, y));
    }  
    debug("\n");              
  }
  debug("-----------------------\n");
}

void add_line_segment_to_nodes(const pp_point_t start, const pp_point_t end) {
  int32_t sx = start.x, sy = start.y, ex = end.x, ey = end.y;

  if(ey < sy) {
    // swap endpoints if line "pointing up", we do this because we
    // alway skip the last scanline (so that polygons can but cleanly
    // up against each other without overlap)
    int32_t ty = sy; sy = ey; ey = ty;
    int32_t tx = sx; sx = ex; ex = tx;
  }

  // Early out if line is completely outside the tile, or has no lines
  if (ey < 0 || sy >= (int)PP_NODE_BUFFER_HEIGHT || sy == ey) return;

  debug("      + line segment from %d, %d to %d, %d\n", sx, sy, ex, ey);

  // Determine how many in-bounds lines to render
  int y = _pp_max(0, sy);
  int count = _pp_min((int)PP_NODE_BUFFER_HEIGHT, ey) - y;

  // Handle cases where x is completely off to one side or other
  if (_pp_max(sx, ex) <= 0) {
    while (count--) {
      nodes[y][node_counts[y]++] = 0;
      ++y;
    }
    return;
  }

  const int full_tile_width = (_pp_tile_width << _pp_antialias);
  if (_pp_min(sx, ex) >= full_tile_width) {
    while (count--) {
      nodes[y][node_counts[y]++] = full_tile_width;
      ++y;
    }
    return;
  }

  // Normal case
  int x = sx;
  int e = 0;

  const int xinc = _pp_sign(ex - sx);
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

#ifdef USE_RP2040_INTERP
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
#else
  // loop over scanlines
  while(count--) {
    // consume accumulated error
    while(e > dy) {e -= dy; x += xinc;}

    // clamp node x value to tile bounds
    int nx = _pp_max(_pp_min(x, full_tile_width), 0);        
    debug("      + adding node at %d, %d\n", x, y);
    // add node to node list
    nodes[y][node_counts[y]++] = nx;

    // step to next scanline and accumulate error
    y++;
    e += einc;
  }
#endif
}

void build_nodes(pp_path_t *contour, pp_rect_t *bounds) {
  PP_COORD_TYPE aa_scale = (PP_COORD_TYPE)(1 << _pp_antialias);

  pp_point_t tile_origin = (pp_point_t) {
    .x = bounds->x * aa_scale,
    .y = bounds->y * aa_scale
  };

  // start with the last point to close the loop
  pp_point_t last = {
    .x = (contour->points[contour->count - 1].x),
    .y = (contour->points[contour->count - 1].y)
  };

  if(_pp_transform) {
    last = pp_point_transform(&last, _pp_transform);
  }

  last.x *= aa_scale;
  last.y *= aa_scale;

  last = pp_point_sub(&last, &tile_origin);

  for(uint32_t i = 0; i < contour->count; i++) {
    pp_point_t point = {
      .x = (contour->points[i].x),
      .y = (contour->points[i].y)
    };

    if(_pp_transform) {
      point = pp_point_transform(&point, _pp_transform);
    }

    point.x *= aa_scale;
    point.y *= aa_scale;

    point = pp_point_sub(&point, &tile_origin);

    add_line_segment_to_nodes(last, point);
    
    last = point;
  }
}

int compare_nodes(const void* a, const void* b) {
  return *((int*)a) - *((int*)b);
}

pp_rect_t render_nodes(uint8_t *buffer, pp_rect_t *tb) {
  int maxy = -1;

  pp_rect_t rb; // render bounds
  rb.y = 0;
  rb.x = tb->w;
  int maxx = 0;
  PP_COORD_TYPE aa_scale = (PP_COORD_TYPE)(1 << _pp_antialias);
  int anitialias_mask = (1 << _pp_antialias) - 1;

  for(int32_t y = 0; y < PP_NODE_BUFFER_HEIGHT; y++) {
    if(node_counts[y] == 0) {
      if (y == rb.y) ++rb.y;
      continue;
    }

    qsort(&nodes[y][0], node_counts[y], sizeof(int), compare_nodes);

    unsigned char* row_data = &buffer[(y >> _pp_antialias) * _pp_tile_width];
    bool rendered_any = false;
    for(uint32_t i = 0; i < node_counts[y]; i += 2) {
      int sx = nodes[y][i + 0];
      int ex = nodes[y][i + 1];

      if(sx == ex) {
        continue;
      }

      rendered_any = true;

      maxx = _pp_max((ex - 1) >> _pp_antialias, maxx);

      debug(" - render span at %d from %d to %d\n", y, sx, ex);

      if (_pp_antialias) {
        int ax = sx / aa_scale;
        const int aex = ex / aa_scale;

        rb.x = _pp_min(ax, rb.x);

        if (ax == aex) {
          row_data[ax] += ex - sx;
          continue;
        }

        row_data[ax] += aa_scale - (sx & anitialias_mask);
        for(ax++; ax < aex; ax++) {
          row_data[ax] += aa_scale;
        }

        // This might add 0 to the byte after the end of the row, we pad the tile data
        // by 1 byte to ensure that is OK
        row_data[ax] += ex & anitialias_mask;
      } else {
        rb.x = _pp_min(sx, rb.x);
        for(int x = sx; x < ex; x++) {
          row_data[x]++;
        }       
      }
    }

    if (rendered_any) {
      debug("  - rendered line %d\n", y);
      maxy = y;
    }
    else if (y == rb.y) {
      debug(" - render nothing on line %d\n", y);
      ++rb.y;
    }
  }

  rb.y >>= _pp_antialias;
  maxy >>= _pp_antialias;
  rb.w = (maxx >= rb.x) ? maxx + 1 - rb.x : 0;
  rb.h = (maxy >= rb.y) ? maxy + 1 - rb.y : 0;

  return rb;
}

void pp_render(pp_poly_t *polygon) {

  debug("> draw polygon with %u contours\n", polygon->count);

  if(polygon->count == 0) {
    return;
  }

  // determine extreme bounds
  pp_rect_t polygon_bounds = pp_polygon_bounds(polygon);

  if(_pp_transform) {
    polygon_bounds = pp_rect_transform(&polygon_bounds, _pp_transform);
  }

  debug("  - bounds %d, %d (%d x %d)\n", polygon_bounds.x, polygon_bounds.y, polygon_bounds.w, polygon_bounds.h);
  debug("  - clip %d, %d (%d x %d)\n", _pp_clip.x, _pp_clip.y, _pp_clip.w, _pp_clip.h);

#ifdef USE_RP2040_INTERP
  interp_hw_save_t interp1_save;
  interp_save(interp1, &interp1_save);

  interp_config cfg = interp_default_config();
  interp_config_set_clamp(&cfg, true);
  interp_config_set_signed(&cfg, true);
  interp_set_config(interp1, 0, &cfg);
  interp1->base[0] = 0;
#endif

  // iterate over tiles
  debug("  - processing tiles\n");
  for(int32_t y = polygon_bounds.y; y < polygon_bounds.y + polygon_bounds.h; y += _pp_tile_height) {
    for(int32_t x = polygon_bounds.x; x < polygon_bounds.x + polygon_bounds.w; x += _pp_tile_width) {
      pp_rect_t tb = (pp_rect_t){.x = x, .y = y, .w = _pp_tile_width, .h = _pp_tile_height};
      tb = pp_rect_intersection(&tb, &_pp_clip);
      debug("    : %d, %d (%d x %d)\n", tb.x, tb.y, tb.w, tb.h);

      // if no intersection then skip tile
      if(pp_rect_empty(&tb)) { debug("    : empty when clipped, skipping\n"); continue; }

      // clear existing tile data and nodes
      memset(node_counts, 0, sizeof(node_counts));
      memset(tile_buffer, 0, tile_buffer_size);

      // build the nodes for each pp_path_t
      for(uint32_t i = 0; i < polygon->count; i++) {
        pp_path_t pp_path_t = polygon->paths[i];
        debug("    : build nodes for path\n");
        build_nodes(&pp_path_t, &tb);
      }

      debug("    : render the tile\n");
      // render the tile

      pp_rect_t rb = render_nodes(tile_buffer, &tb);
      tb.x += rb.x; tb.y += rb.y; tb.w = rb.w; tb.h = rb.h;

      debug(" - adjusted render tile bounds %d, %d (%d x %d)\n", rb.x, rb.y, rb.w, rb.h);

      if(pp_rect_empty(&tb)) { debug("    : empty after rendering, skipping\n"); continue; }

      pp_tile_t tile = {        
        .x = tb.x, .y = tb.y, .w = tb.w, .h = tb.h,
        .stride = (uint32_t)_pp_tile_width,
        .data = tile_buffer + rb.x + _pp_tile_width * rb.y
      };

      _pp_tile_callback(&tile);
    }
  }

#ifdef USE_RP2040_INTERP
  interp_restore(interp1, &interp1_save);
#endif
}

#endif // PP_IMPLEMENTATION

#endif // PP_INCLUDE_H