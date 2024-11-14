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
#include <limits.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#ifndef PP_COORD_TYPE
#define PP_COORD_TYPE float
#endif

#ifndef PP_SCALE_TO_ALPHA
#define PP_SCALE_TO_ALPHA 1
#endif

#if defined(PICO_ON_DEVICE) && PICO_ON_DEVICE
#define USE_RP2040_INTERP
#include "hardware/interp.h"
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
typedef enum {PP_AA_NONE = 0, PP_AA_FAST = 1, PP_AA_X4 = 1, PP_AA_BEST = 2, PP_AA_X16 = 2} pp_antialias_t;

typedef struct {
  int32_t x, y, w, h;
  uint32_t stride;
  uint8_t *data;
} pp_tile_t;

typedef struct _pp_path_t {
  pp_point_t *points;
  int count;                  // number of points currently stored in points buffer
  int storage;                // size of *points buffer
  struct _pp_path_t *next;            // next path in the linked list
} pp_path_t;
void pp_path_add_point(pp_path_t *path, pp_point_t p);
void pp_path_add_points(pp_path_t *path, pp_point_t *p, int count);
void pp_path_add_path(pp_path_t *path, pp_path_t *other);
void pp_path_union(pp_path_t *path, pp_path_t *other);
pp_rect_t pp_path_bounds(const pp_path_t *c);

typedef struct {
  pp_path_t *paths;
} pp_poly_t;
pp_poly_t *pp_poly_new();
void pp_poly_free(pp_poly_t *poly);
pp_path_t* pp_poly_tail_path(pp_poly_t *p);
pp_path_t* pp_poly_add_path(pp_poly_t *p);
pp_rect_t pp_poly_bounds(pp_poly_t *p);
int pp_poly_path_count(pp_poly_t *p);
void pp_poly_merge(pp_poly_t *p, pp_poly_t *m);

// user settings
typedef void (*pp_tile_callback_t)(const pp_tile_t *tile);

extern uint32_t            _pp_tile_buffer_size;

extern pp_rect_t           _pp_clip;
extern pp_tile_callback_t  _pp_tile_callback;
extern pp_antialias_t      _pp_antialias;
extern pp_mat3_t          *_pp_transform;

void pp_clip(int32_t x, int32_t y, int32_t w, int32_t h);
void pp_tile_callback(pp_tile_callback_t callback);
void pp_antialias(pp_antialias_t antialias);
pp_mat3_t *pp_transform(pp_mat3_t *transform);
void pp_render(pp_poly_t *polygon);

void pp_init(uint32_t tile_buffer_size, uint32_t max_nodes_per_scanline);
void pp_deinit();


#ifdef __cplusplus
}
#endif

#ifdef PP_IMPLEMENTATION

#ifndef PP_MALLOC
#define PP_MALLOC(size)         malloc(size)
#define PP_REALLOC(p, size)     realloc(p, size)
#define PP_FREE(p)              free(p)
#endif

#ifdef PP_DEBUG
#define debug(...) printf(__VA_ARGS__)
#else
#define debug(...)
#endif

pp_rect_t           _pp_clip = (pp_rect_t){-INT_MAX, -INT_MAX, INT_MAX, INT_MAX};
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
  return tile->data[(x - tile->x) + (y - tile->y) * _pp_tile_buffer_size];
}

pp_poly_t *pp_poly_new() {
  pp_poly_t *poly = (pp_poly_t *)PP_MALLOC(sizeof(pp_poly_t));
  poly->paths = NULL;
  return poly;
}

void pp_poly_free(pp_poly_t *poly) {
  if(poly->paths) {
    pp_path_t *path = poly->paths;
    while(path) {
      PP_FREE(path->points);
      pp_path_t *free_path = path;
      path = path->next;
      PP_FREE(free_path);
    }
  }
  PP_FREE(poly);
}

// polygon and path implementation
pp_path_t* pp_poly_tail_path(pp_poly_t *poly) {
  pp_path_t *path = poly->paths;
  while(path->next) path = path->next;
  return path;
}

int pp_poly_path_count(pp_poly_t *poly) {
  if(!poly->paths) return 0;
  pp_path_t *path = poly->paths;
  int i = 0;
  while(path->next) {
    i++;
    path = path->next;
  }
  return i;
}

pp_path_t* pp_poly_add_path(pp_poly_t *poly) {
  pp_path_t *path = (pp_path_t *)PP_MALLOC(sizeof(pp_path_t));
  memset(path, 0, sizeof(pp_path_t));
  path->storage = 8;
  path->points = (pp_point_t *)PP_MALLOC(sizeof(pp_point_t) * path->storage);

  if(!poly->paths) {
    poly->paths = path;
  }else{
    pp_path_t *tail = pp_poly_tail_path(poly);
    tail->next = path;
  }

  return path;
}

void pp_poly_merge(pp_poly_t *p, pp_poly_t *m) {
  if(!p->paths) {
    p->paths = m->paths;
  }else{
    pp_poly_tail_path(p)->next = m->paths;
  }

  m->paths = NULL;
  pp_poly_free(m);
}

pp_point_t* pp_path_tail_point(pp_path_t *path) {
  return (path->count > 0) ? &path->points[path->count -1] : NULL;
}

void pp_path_add_point(pp_path_t *path, pp_point_t p) {
  if(path->count == path->storage) { // no storage left, double buffer size
    if(path->points) {
      path->storage *= 2;
      path->points = (pp_point_t *)PP_REALLOC(path->points, sizeof(pp_point_t) * (path->storage));
    }else{
      path->storage = 8;
      path->points = (pp_point_t *)PP_MALLOC(sizeof(pp_point_t) * (path->storage));
    }
  }
  path->points[path->count] = p;
  path->count++;
}

void pp_path_add_points(pp_path_t *path, pp_point_t *points, int count) {
  if(count + path->count > path->storage) { // not enough storage, allocate
    path->storage = path->count + count;
    path->points = (pp_point_t *)PP_REALLOC(path->points, sizeof(pp_point_t) * (path->storage));
  }
  memcpy(&path->points[path->count], points, sizeof(pp_point_t) * count);
  path->count += count;
}

// pp_contour_t implementation
pp_rect_t pp_path_bounds(const pp_path_t *path) {
  int minx = INT_MAX, maxx = -INT_MAX, miny = INT_MAX, maxy = -INT_MAX;
  for(int i = 0; i < path->count; i++) {
    minx = _pp_min(minx, path->points[i].x);
    miny = _pp_min(miny, path->points[i].y);
    maxx = _pp_max(maxx, path->points[i].x);
    maxy = _pp_max(maxy, path->points[i].y);
  }
  return (pp_rect_t){minx, miny, maxx - minx, maxy - miny};
}

void pp_path_union(pp_path_t *path, pp_path_t *other) {

}

pp_rect_t pp_poly_bounds(pp_poly_t *p) {
  pp_path_t *path = p->paths;
  if(!path) return (pp_rect_t){};
  pp_rect_t b = pp_path_bounds(path);
  path = path->next;
  while(path) {
    pp_rect_t cb = pp_path_bounds(path);
    b = pp_rect_merge(&b, &cb);
    path = path->next;
  }
  return b;
}

uint32_t _pp_tile_buffer_size = 0;
uint32_t _pp_max_nodes_per_scanline = 0;

// buffer that each tile is rendered into before callback
// allocate one extra byte to allow a small optimization in the row renderer
uint8_t *pp_tile_buffer;
//uint8_t pp_tile_buffer[PP_TILE_BUFFER_SIZE * PP_TILE_BUFFER_SIZE];

// polygon node buffer handles at most 16 line intersections per scanline
// is this enough for cjk/emoji? (requires a 2kB buffer)
int32_t *pp_nodes;
uint32_t *pp_node_counts;
//int32_t pp_nodes[PP_TILE_BUFFER_SIZE * 4][PP_MAX_NODES_PER_SCANLINE * 2];
//uint32_t pp_node_counts[PP_TILE_BUFFER_SIZE * 4];

uint8_t _pp_alpha_map_none[2] = {0, 255};
uint8_t _pp_alpha_map_x4[5] = {0, 63, 127, 190, 255};
uint8_t _pp_alpha_map_x16[17] = {0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240, 255};

void pp_init(uint32_t tile_buffer_size, uint32_t max_nodes_per_scanline) {
  _pp_tile_buffer_size = tile_buffer_size;
  _pp_max_nodes_per_scanline = max_nodes_per_scanline;
  pp_tile_buffer = (uint8_t *)PP_MALLOC(tile_buffer_size * tile_buffer_size);
  // tile_buffer_size * 4 | max_nodes_per_scanline * 2
  pp_nodes = (int32_t *)PP_MALLOC(tile_buffer_size * 4 * max_nodes_per_scanline * 2 * sizeof(int32_t));
  pp_node_counts = (uint32_t *)PP_MALLOC(tile_buffer_size * 4 * sizeof(uint32_t));
}

void pp_deinit() {
  PP_FREE(pp_tile_buffer);
  PP_FREE(pp_nodes);
  PP_FREE(pp_node_counts);
}

void pp_clip(int32_t x, int32_t y, int32_t w, int32_t h) {
  _pp_clip = (pp_rect_t){.x = x, .y = y, .w = w, .h = h};
}

void pp_tile_callback(pp_tile_callback_t callback) {
  _pp_tile_callback = callback;
}

// maximum tile bounds determined by antialias level
void pp_antialias(pp_antialias_t antialias) {
  _pp_antialias = antialias;
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

void add_line_segment_to_nodes(const pp_point_t start, const pp_point_t end, pp_rect_t *tb) {
  int32_t sx = start.x, sy = start.y, ex = end.x, ey = end.y;

  if(ey < sy) {
    // swap endpoints if line "pointing up", we do this because we
    // alway skip the last scanline (so that polygons can but cleanly
    // up against each other without overlap)
    int32_t ty = sy; sy = ey; ey = ty;
    int32_t tx = sx; sx = ex; ex = tx;
  }

  // early out if line is completely outside the tile, or has no gradient
  if (ey < 0 || sy >= (int)(tb->h << _pp_antialias) || sy == ey) return;

  debug("      + line segment from %d, %d to %d, %d\n", sx, sy, ex, ey);

  // determine how many in-bounds lines to render
  int y = _pp_max(0, sy);
  int count = _pp_min((int)(tb->h << _pp_antialias), ey) - y;

  int x = sx;
  int e = 0;

  const int xinc = _pp_sign(ex - sx);
  const int einc = abs(ex - sx) + 1;
  const int dy = ey - sy;

  // if sy < 0 jump to the start, note this does use a divide
  // but potentially saves many wasted loops below, so is likely worth it.
  if (sy < 0) {
    e = einc * -sy;
    int xjump = e / dy;
    e -= dy * xjump;
    x += xinc * xjump;
  }

// #ifdef USE_RP2040_INTERP
//   interp1->base[1] = full_tile_width;
//   interp1->accum[0] = x;

//   // loop over scanlines
//   while(count--) {
//     // consume accumulated error
//     while(e > dy) {e -= dy; interp1->add_raw[0] = xinc;}

//     // clamp node x value to tile bounds
//     const int nx = interp1->peek[0];
//     debug("      + adding node at %d, %d\n", x, y);
//     // add node to node list
//     pp_nodes[y][pp_node_counts[y]++] = nx;

//     // step to next scanline and accumulate error
//     y++;
//     e += einc;
//   }
// #else
  // loop over scanlines

  while(count--) {
    int32_t *pp_scanline_nodes = &pp_nodes[y * 4 * _pp_max_nodes_per_scanline * 2];

    // consume accumulated error
    while(e > dy) {e -= dy; x += xinc;}

    // clamp node x value to tile bounds
    int nx = _pp_max(_pp_min(x, (tb->w << _pp_antialias)), 0);
    //debug("      + adding node at %d, %d\n", x, y);
    // add node to node list
    pp_scanline_nodes[pp_node_counts[y]++] = nx;

    // step to next scanline and accumulate error
    y++;
    e += einc;
  }
//#endif
}

void build_nodes(pp_path_t *path, pp_rect_t *tb) {
  PP_COORD_TYPE aa_scale = (PP_COORD_TYPE)(1 << _pp_antialias);

  pp_point_t tile_origin = (pp_point_t){tb->x * aa_scale, tb->y * aa_scale};

  // start with the last point to close the loop, transform it, scale for antialiasing, and offset to tile origin
  pp_point_t last = path->points[path->count - 1];
  if(_pp_transform) last = pp_point_transform(&last, _pp_transform);
  last.x *= aa_scale; last.y *= aa_scale;
  last = pp_point_sub(&last, &tile_origin);

  for(int i = 0; i < path->count; i++) {
    pp_point_t next = path->points[i];
    if(_pp_transform) next = pp_point_transform(&next, _pp_transform);
    next.x *= aa_scale; next.y *= aa_scale;
    next = pp_point_sub(&next, &tile_origin);
    add_line_segment_to_nodes(last, next, tb);
    last = next;
  }
}

int compare_nodes(const void* a, const void* b) {
  return *((int*)a) - *((int*)b);
}

pp_rect_t render_nodes(pp_rect_t *tb) {
  pp_rect_t rb = {_pp_tile_buffer_size << _pp_antialias, _pp_tile_buffer_size << _pp_antialias, 0, 0}; // render bounds
  int maxx = 0, minx = _pp_tile_buffer_size << _pp_antialias;
  debug("  + render tile %d, %d - %d, %d\n", tb->x, tb->y, tb->w, tb->h);

  for(int y = 0; y < ((int)_pp_tile_buffer_size << _pp_antialias); y++) {
    int32_t *pp_scanline_nodes = &pp_nodes[y * 4 * _pp_max_nodes_per_scanline * 2];

    // debug("    : row %d node count %d\n", y, pp_node_counts[y]);

    if(pp_node_counts[y] == 0) continue; // no nodes on this raster line

    qsort(pp_scanline_nodes, pp_node_counts[y], sizeof(int), compare_nodes);

    unsigned char* row_data = &pp_tile_buffer[(y >> _pp_antialias) * _pp_tile_buffer_size];

    for(uint32_t i = 0; i < pp_node_counts[y]; i += 2) {
      int sx = *pp_scanline_nodes++;
      int ex = *pp_scanline_nodes++;

      if(sx == ex) { // empty span, nothing to do
        continue;
      }

      // update render bounds
      rb.x = _pp_min(rb.x, sx);
      rb.y = _pp_min(rb.y, y);
      minx = _pp_min(_pp_min(sx, ex), minx);
      maxx = _pp_max(_pp_max(sx, ex), maxx);
      rb.h = y - rb.y + 1;

      //debug(" - render span at %d from %d to %d\n", y, sx, ex);

      // rasterise the span into the tile buffer
      do {
        row_data[sx >> _pp_antialias]++;
      } while(++sx < ex);
    }
  }

  rb.w = maxx - minx;

  // shifting the width and height effectively "floors" the result which can
  // mean we lose a pixel off the right or bottom edge of the tile. by adding
  // either 1 (at x4) or 3 (at x16) we change that to a "ceil" instead ensuring
  // the full tile bounds are returned
  if(_pp_antialias) {
    rb.w += (_pp_antialias | 0b1);
    rb.h += (_pp_antialias | 0b1);
  }

  rb.x >>= _pp_antialias;
  rb.y >>= _pp_antialias;
  rb.w >>= _pp_antialias;
  rb.h >>= _pp_antialias;

  uint8_t *p_alpha_map = _pp_alpha_map_none;
  if(_pp_antialias == 1) p_alpha_map = _pp_alpha_map_x4;
  if(_pp_antialias == 2) p_alpha_map = _pp_alpha_map_x16;
  #if PP_SCALE_TO_ALPHA == 1
    for(int y = rb.y; y < rb.y + rb.h; y++) {
      unsigned char* row_data = &pp_tile_buffer[y * _pp_tile_buffer_size + rb.x];
      for(int x = rb.x; x < rb.x + rb.w; x++) {
        *row_data = p_alpha_map[*row_data];
        row_data++;
      }
    }
  #endif

  debug("    : rendered tile bounds %d, %d (%d x %d)\n", rb.x, rb.y, rb.w, rb.h);

  return rb;
}

void pp_render(pp_poly_t *polygon) {

  debug("> draw polygon with %u contours\n", pp_poly_path_count(polygon));

  if(!polygon->paths) return;

  // determine extreme bounds
  pp_rect_t pb = pp_poly_bounds(polygon);

  if(_pp_transform) {
    pb = pp_rect_transform(&pb, _pp_transform);
  }

  debug("  - polygon bounds %d, %d (%d x %d)\n", pb.x, pb.y, pb.w, pb.h);
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
  for(int32_t y = pb.y; y < pb.y + pb.h; y += _pp_tile_buffer_size) {
    for(int32_t x = pb.x; x < pb.x + pb.w; x += _pp_tile_buffer_size) {
      pp_rect_t tb = (pp_rect_t){.x = x, .y = y, .w = _pp_tile_buffer_size, .h = _pp_tile_buffer_size};
      tb = pp_rect_intersection(&tb, &_pp_clip);
      debug("    : %d, %d (%d x %d)\n", tb.x, tb.y, tb.w, tb.h);

      // if no intersection then skip tile
      if(pp_rect_empty(&tb)) { debug("    : empty when clipped, skipping\n"); continue; }

      // clear existing tile data and nodes
      memset(pp_node_counts, 0, _pp_tile_buffer_size * 4 * sizeof(uint32_t));
      memset(pp_tile_buffer, 0, _pp_tile_buffer_size * _pp_tile_buffer_size);

      // build the nodes for each pp_path_t
      pp_path_t *path = polygon->paths;
      if(!path) return;
      do {
        debug("    : build nodes for path (%d points)\n", path->count);
        build_nodes(path, &tb);
        path = path->next;
      } while(path);

      debug("    : render the tile\n");
      // render the tile

      pp_rect_t rb = render_nodes(&tb);
      tb.x += rb.x; tb.y += rb.y; tb.w = rb.w; tb.h = rb.h;

      if(pp_rect_empty(&tb)) { debug("    : empty after rendering, skipping\n"); continue; }

      pp_tile_t tile = {
        .x = tb.x, .y = tb.y, .w = tb.w, .h = tb.h,
        .stride = _pp_tile_buffer_size,
        .data = pp_tile_buffer + rb.x + (_pp_tile_buffer_size * rb.y)
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