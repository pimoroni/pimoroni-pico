#pragma once
#include <cstdint>
#include <math.h>

using namespace std;

#ifdef PP_DEBUG
#define debug(...) printf(__VA_ARGS__)
#else
#define debug(...)
#endif

namespace pretty_poly {

  enum antialias_t {NONE = 0, X4 = 1, X16 = 2};

  // 3x3 matrix for coordinate transformations
  struct mat3_t {
    float v00, v10, v20, v01, v11, v21, v02, v12, v22 = 0.0f;
    mat3_t() = default;
    mat3_t(const mat3_t &m) = default;
    inline mat3_t& operator*= (const mat3_t &m) {        
      float r00 = this->v00 * m.v00 + this->v01 * m.v10 + this->v02 * m.v20;
      float r01 = this->v00 * m.v01 + this->v01 * m.v11 + this->v02 * m.v21;
      float r02 = this->v00 * m.v02 + this->v01 * m.v12 + this->v02 * m.v22;
      float r10 = this->v10 * m.v00 + this->v11 * m.v10 + this->v12 * m.v20;
      float r11 = this->v10 * m.v01 + this->v11 * m.v11 + this->v12 * m.v21;
      float r12 = this->v10 * m.v02 + this->v11 * m.v12 + this->v12 * m.v22;
      float r20 = this->v20 * m.v00 + this->v21 * m.v10 + this->v22 * m.v20;
      float r21 = this->v20 * m.v01 + this->v21 * m.v11 + this->v22 * m.v21;
      float r22 = this->v20 * m.v02 + this->v21 * m.v12 + this->v22 * m.v22;    
      this->v00 = r00; this->v01 = r01; this->v02 = r02;
      this->v10 = r10; this->v11 = r11; this->v12 = r12;
      this->v20 = r20; this->v21 = r21; this->v22 = r22;
      return *this;
    }

    static mat3_t identity() {mat3_t m; m.v00 = m.v11 = m.v22 = 1.0f; return m;}
    static mat3_t rotation(float a) {
      float c = cosf(a), s = sinf(a); mat3_t r = mat3_t::identity();
      r.v00 = c; r.v01 = s; r.v10 = -s; r.v11 = c; return r;}
    static mat3_t translation(float x, float y) {
      mat3_t r = mat3_t::identity(); r.v02 = x; r.v12 = y; return r;}
    static mat3_t scale(float x, float y) {
      mat3_t r = mat3_t::identity(); r.v00 = x; r.v11 = y; return r;}
  };

  // point type for contour points
  template<typename T = int>
  struct __attribute__ ((packed)) point_t {
    T x, y;
    point_t(T x, T y) : x(x), y(y) {}
    point_t() : x(0), y(0) {}
    inline point_t& operator-= (const point_t &a) {x -= a.x; y -= a.y; return *this;}
    inline point_t& operator+= (const point_t &a) {x += a.x; y += a.y; return *this;}
    inline point_t& operator*= (const float a) {x *= a; y *= a; return *this;}
    inline point_t& operator*= (const mat3_t &a) {this->transform(a); return *this;}
    inline point_t& operator/= (const float a) {x /= a; y /= a; return *this;}
    inline point_t& operator/= (const point_t &a) {x /= a.x; y /= a.y; return *this;}
    void transform(const mat3_t &m) {
      this->x = (m.v00 * float(this->x) + m.v01 * float(this->y) + m.v02);
      this->y = (m.v10 * float(this->x) + m.v11 * float(this->y) + m.v12);
    }

  };

  template<typename T> inline point_t<T> operator-  (point_t<T> lhs, const point_t<T> &rhs) { lhs -= rhs; return lhs; }
  template<typename T> inline point_t<T> operator-  (const point_t<T> &rhs) { return point_t<T>(-rhs.x, -rhs.y); }
  template<typename T> inline point_t<T> operator+  (point_t<T> lhs, const point_t<T> &rhs) { lhs += rhs; return lhs; }
  template<typename T> inline point_t<T> operator*  (point_t<T> lhs, const float rhs) { lhs *= rhs; return lhs; }
  template<typename T> inline point_t<T> operator*  (point_t<T> lhs, const point_t<T> &rhs) { lhs *= rhs; return lhs; }
  template<typename T> inline point_t<T> operator*  (point_t<T> lhs, const mat3_t &rhs) { lhs *= rhs; return lhs; }
  template<typename T> inline point_t<T> operator/  (point_t<T> lhs, const float rhs) { lhs /= rhs; return lhs; }
  template<typename T> inline point_t<T> operator/  (point_t<T> lhs, const point_t<T> &rhs) { lhs.x /= rhs.x; lhs.y /= rhs.y; return lhs; }


  // rect type for bounds and clipping rectangles
  struct rect_t {
    int x, y, w, h;    
    rect_t() : x(0), y(0), w(0), h(0) {}
    rect_t(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {}
    bool empty() const {return this->w == 0 && this->h == 0;}
    rect_t intersection(const rect_t &c) {
      return rect_t(max(this->x, c.x), max(this->y, c.y),
        max(0, min(this->x + this->w, c.x + c.w) - max(this->x, c.x)),
        max(0, min(this->y + this->h, c.y + c.h) - max(this->y, c.y)));
    }
    rect_t merge(const rect_t &c) {
      return rect_t(min(this->x, c.x), min(this->y, c.y),
        max(this->x + this->w, c.x + c.w) - min(this->x, c.x),
        max(this->y + this->h, c.y + c.h) - min(this->y, c.y));
    }
  };

  struct tile_t {
    rect_t bounds;
    unsigned stride;
    uint8_t *data;

    tile_t() {};

    int get_value(int x, int y) const {
      return this->data[x + y * this->stride];
    }
  };

  template<typename T>
  struct contour_t {
    point_t<T> *points;
    unsigned count;

    contour_t() {}
    contour_t(vector<point_t<T>> v) : points(v.data()), count(v.size()) {};
    contour_t(point_t<T> *points, unsigned count) : points(points), count(count) {};

    rect_t bounds() {
      T minx = this->points[0].x, maxx = minx;
      T miny = this->points[0].y, maxy = miny;
      for(auto i = 1u; i < this->count; i++) {
        minx = min(minx, this->points[i].x);
        miny = min(miny, this->points[i].y);
        maxx = max(maxx, this->points[i].x); 
        maxy = max(maxy, this->points[i].y);
      }
      return rect_t(minx, miny, maxx - minx, maxy - miny);
    }
  };
  
}
