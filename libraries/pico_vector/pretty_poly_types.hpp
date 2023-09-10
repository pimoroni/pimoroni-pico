#pragma once
#include <cstdint>
#include <math.h>
#include <vector>

#ifdef PP_DEBUG
#define debug(...) printf(__VA_ARGS__)
#else
#define debug(...)
#endif

namespace pretty_poly {

  enum antialias_t {NONE = 0, X4 = 1, X16 = 2};

  // 3x3 matrix for coordinate transformations
  struct mat3_t {
    float v00 = 0.0f, v10 = 0.0f, v20 = 0.0f, v01 = 0.0f, v11 = 0.0f, v21 = 0.0f, v02 = 0.0f, v12 = 0.0f, v22 = 0.0f;
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
      r.v00 = c; r.v01 = -s; r.v10 = s; r.v11 = c; return r;}
    static mat3_t translation(float x, float y) {
      mat3_t r = mat3_t::identity(); r.v02 = x; r.v12 = y; return r;}
    static mat3_t scale(float x, float y) {
      mat3_t r = mat3_t::identity(); r.v00 = x; r.v11 = y; return r;}
  };

  // 2x2 matrix for rotations and scales
  struct mat2_t {
    float v00 = 0.0f, v10 = 0.0f, v01 = 0.0f, v11 = 0.0f;
    mat2_t() = default;
    mat2_t(const mat2_t &m) = default;
    inline mat2_t& operator*= (const mat2_t &m) {        
      float r00 = this->v00 * m.v00 + this->v01 * m.v10;
      float r01 = this->v00 * m.v01 + this->v01 * m.v11;
      float r10 = this->v10 * m.v00 + this->v11 * m.v10;
      float r11 = this->v10 * m.v01 + this->v11 * m.v11;
      this->v00 = r00; this->v01 = r01;
      this->v10 = r10; this->v11 = r11;
      return *this;
    }

    static mat2_t identity() {mat2_t m; m.v00 = m.v11 = 1.0f; return m;}
    static mat2_t rotation(float a) {
      float c = cosf(a), s = sinf(a); mat2_t r;
      r.v00 = c; r.v01 = -s; r.v10 = s; r.v11 = c; return r;}
    static mat2_t scale(float x, float y) {
      mat2_t r; r.v00 = x; r.v11 = y; return r;}
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
    inline point_t& operator*= (const mat2_t &a) {this->transform(a); return *this;}
    inline point_t& operator*= (const mat3_t &a) {this->transform(a); return *this;}
    inline point_t& operator/= (const float a) {x /= a; y /= a; return *this;}
    inline point_t& operator/= (const point_t &a) {x /= a.x; y /= a.y; return *this;}
    void transform(const mat3_t &m) {
      float tx = x, ty = y;
      this->x = (m.v00 * tx + m.v01 * ty + m.v02);
      this->y = (m.v10 * tx + m.v11 * ty + m.v12);
    }
    void transform(const mat2_t &m) {
      float tx = x, ty = y;
      this->x = (m.v00 * tx + m.v01 * ty);
      this->y = (m.v10 * tx + m.v11 * ty);
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
    bool empty() const {return this->w == 0 || this->h == 0;}
    rect_t intersection(const rect_t &c) {
      return rect_t(std::max(this->x, c.x), std::max(this->y, c.y),
        std::max(0, std::min(this->x + this->w, c.x + c.w) - std::max(this->x, c.x)),
        std::max(0, std::min(this->y + this->h, c.y + c.h) - std::max(this->y, c.y)));
    }
    rect_t merge(const rect_t &c) {
      return rect_t(std::min(this->x, c.x), std::min(this->y, c.y),
        std::max(this->x + this->w, c.x + c.w) - std::min(this->x, c.x),
        std::max(this->y + this->h, c.y + c.h) - std::min(this->y, c.y));
    }
  };

  struct tile_t {
    rect_t bounds;
    unsigned stride;
    uint8_t *data;

    tile_t() {};

    inline int get_value(int x, int y) const {
      return this->data[x + y * this->stride];
    }
  };

  template<typename T>
  struct contour_t {
    point_t<T> *points;
    unsigned count;

    contour_t() {}
    contour_t(const std::vector<point_t<T>>& v) : points(v.data()), count(v.size()) {};
    contour_t(point_t<T> *points, unsigned count) : points(points), count(count) {};

    // TODO: Make this work, it's so much nicer to use auto point : contour
    //point_t<T> *begin() const { return points; };
    //point_t<T> *end() const { return points + count * sizeof(point_t<T>); };

    rect_t bounds() const {
      T minx = this->points[0].x, maxx = minx;
      T miny = this->points[0].y, maxy = miny;
      for(auto i = 1u; i < this->count; i++) {
        minx = std::min(minx, this->points[i].x);
        miny = std::min(miny, this->points[i].y);
        maxx = std::max(maxx, this->points[i].x); 
        maxy = std::max(maxy, this->points[i].y);
      }
      return rect_t(minx, miny, maxx - minx, maxy - miny);
    }
  };
  
}
