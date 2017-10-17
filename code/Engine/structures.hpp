#ifndef engine_structures_h
#define engine_structures_h

#include <stdint.h>
#include <assert.h>

#ifndef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif
#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif

#define LERP(t, x, y) ((t)*(x)+(1.f-t)*(y))

#define ABS_LESS(x, y)    ((x) >  -(y) && (x) <  (y))
#define ABS_LEQ(x, y)     ((x) >= -(y) && (x) <= (y))
#define ABS_GREATER(x, y) ((x) >  (y) || (x) <  -(y))
#define ABS_GEQ(x, y)     ((x) >= (y) || (x) <= -(y))

#ifndef M_PI
#define M_PI 3.1415926535f
#endif
#ifndef M_PI_2
#define M_PI_2 1.5708f
#endif

#define SQRT_3_2 0.86602540378f

typedef unsigned int uint;

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8  i8;
typedef int16 i16;
typedef int32 i32;
typedef int64 i64;

typedef uint8  u8;
typedef uint16 u16;
typedef uint32 u32;
typedef uint64 u64;

typedef i8  s8;
typedef i16 s16;
typedef i32 s32;
typedef i64 s64;

typedef float  float32;
typedef double float64;

typedef float32 f32;
typedef float64 f64;

#define MAX_U8  0xFF
#define MAX_U16 0xFFFF
#define MAX_U32 0xFFFFFFFF
#define MAX_U64 0xFFFFFFFFFFFFFFFF

#define LOG_MALLOCS 0
#if LOG_MALLOCS
inline void *log_malloc(int s, int line, const char *file){
    void *p = malloc(s);
    printf("malloc: %p [size=%i] @ %s:%i\n", p, s, file, line);
    return p;
}
#define malloc(s) log_malloc(s, __LINE__, __FILE__);
#endif

struct RgbColor {
    unsigned char r, g, b;
};
struct RgbaColor {
    unsigned char r, g, b, a;
};

#if ENGINE_ENABLE_VEC2
// Vectors of dimension 2
#if ENGINE_ENABLE_ANGLE
struct Angle;
#endif

inline int goodMod(int a, int m){
    a = a%m;
    if(a < 0)
        return a+m;
    return a;
}
inline int aproxGoodMod(int a, int m){
    if(a < 0)
        return a+m;
    if(a >= m)
        return a-m;
    return a;
}

template <typename T> struct _Vec2{
    T x, y;
    // arithmetic operators with _Vec2r
    inline _Vec2 operator+(const _Vec2& b) const { return {x+b.x, y+b.y}; }
    inline _Vec2 operator-(const _Vec2& b) const { return {x-b.x, y-b.y}; }
    inline _Vec2 operator*(const _Vec2& b) const { return {x*b.x, y*b.y}; }
    inline _Vec2 operator/(const _Vec2& b) const { return {x/b.x, y/b.y}; }
    
    // op= operators
    inline _Vec2& operator+=(const _Vec2& b) { *this = *this+b; return *this; }
    inline _Vec2& operator-=(const _Vec2& b) { *this = *this-b; return *this; }
    inline _Vec2& operator*=(const _Vec2& b) { *this = *this*b; return *this; }
    inline _Vec2& operator/=(const _Vec2& b) { *this = *this/b; return *this; }
    
    // arithmetic operators with real
    inline _Vec2 operator+(T b) const { return {x+b, y+b}; }
    inline _Vec2 operator-(T b) const { return {x-b, y-b}; }
    inline _Vec2 operator*(T b) const { return {x*b, y*b}; }
    inline _Vec2 operator/(T b) const { return {x/b, y/b}; }
    
    // op= operators with real
    inline _Vec2& operator+=(T b) { *this = *this+b; return *this; }
    inline _Vec2& operator-=(T b) { *this = *this-b; return *this; }
    inline _Vec2& operator*=(T b) { *this = *this*b; return *this; }
    inline _Vec2& operator/=(T b) { *this = *this/b; return *this; }
    
    inline bool operator==(_Vec2 v) { return this->x == v.x && this->y == v.y; }
    inline bool operator!=(_Vec2 v) { return this->x != v.x || this->y != v.y; }
    
    inline _Vec2 operator-(void) const{
        return {-x, -y};
    }
    
    inline void normalize();
    
    inline _Vec2(T ox, T oy) : x(ox), y(oy) {};
    inline _Vec2(T o) : x(o), y(o) {};
    inline _Vec2() : x(0), y(0) {};
    
#if ENGINE_ENABLE_ANGLE
    inline _Vec2(Angle a);
#endif
};

template <typename T> inline T dot(_Vec2<T> u, _Vec2<T> v){
    return u.x*v.x+u.y*v.y;
}
template <typename T> inline T cross(_Vec2<T> a, _Vec2<T> b){
    return a.x*b.y-a.y*b.x;
}
template <typename T> inline T lenSqr(_Vec2<T> u){
    return dot(u, u);
}
template <typename T> inline float len(_Vec2<T> u){
    return sqrtf(lenSqr(u));
}
template <typename T> inline _Vec2<T> normalize(_Vec2<T> u){
    return u/len(u);
}
template <typename T> inline void _Vec2<T>::normalize(){
    *this /= len(*this);
}
template <typename T> inline _Vec2<T> operator*(T s, const _Vec2<T>& v){
    return {s*v.x, s*v.y};
}

typedef _Vec2<float> Vec2;
typedef _Vec2<int>   Vec2i;

inline Vec2 operator*(float k, Vec2i v){
    return {k*v.x, k*v.y};
}
inline Vec2 operator*(Vec2i v, float k){
    return {k*v.x, k*v.y};
}

inline float project(Vec2 axis, Vec2 p){
    return dot(axis, p)/lenSqr(axis);
}
inline float norm1(Vec2 v){
    return fabs(v.x)+fabs(v.y);
}
inline float normInf(Vec2 v){
    float x = fabs(v.x);
    float y = fabs(v.y);
    return MAX(x, y);
}
inline Vec2 orthogonal(Vec2 v){
    return Vec2(-v.y, v.x);
}

inline Vec2i goodMod(Vec2i a, int m){
    return Vec2i(goodMod(a.x, m), goodMod(a.y, m));
}
inline Vec2i aproxGoodMod(Vec2i a, int m){
    return Vec2i(aproxGoodMod(a.x, m), aproxGoodMod(a.y, m));
}

inline Vec2 floor(Vec2 v){
    return {floorf(v.x), floorf(v.y)};
}
inline Vec2 round(Vec2 v){
    return {roundf(v.x), roundf(v.y)};
}
inline Vec2 ceil(Vec2 v){
    return {ceilf(v.x), ceilf(v.y)};
}

#endif

#if ENGINE_ENABLE_ANGLE
// Angles
struct Angle{
    float c, s;
    // arithmetic operators with Vec2
    inline Angle operator+(const Angle& b) const { return {c*b.c-s*b.s, c*b.s+s*b.c}; }
    inline Angle operator-(const Angle& b) const { return {c*b.c+s*b.s, -c*b.s+s*b.c}; }
    
#if ENGINE_ENABLE_VEC2
    inline Vec2 operator*(const float& a) const { return {a*c, a*s}; }
#endif
    
    inline Angle operator*(const float b) const {
        float ro = atan2f(s, c);
        return {cosf(b*ro), sinf(b*ro)};
    }
    
    // op= operators
    inline Angle& operator+=(const Angle& b) { *this = *this+b; return *this; }
    inline Angle& operator-=(const Angle& b) { *this = *this-b; return *this; }
    
    inline bool operator==(Angle v) { return fabs(this->c-v.c)+fabs(this->s-v.s) < 0.1f; }
    inline bool operator!=(Angle v) { return fabs(this->c-v.c)+fabs(this->s-v.s) > 0.1f; }
    
    inline Angle operator-(void) const{
        return {c, -s};
    }
    
    inline void correct();
    
    inline Angle(float ox, float oy){
        c = ox;
        s = oy;
    };
    inline Angle(float ro){
        c = cosf(ro);
        s = sinf(ro);
    };
    Angle(Vec2 v);
    inline Angle(){};
    inline float toFloat(){
        return atan2f(s, c);
    }
};
inline bool isAngleBetween(Angle ro, float a, float b){
    float p = ro.toFloat();
    if(a < b){
        return a <= p && p <= b;
    }else{
        return a <= p || p <= b;
    }
}

extern const Angle zeroAngle;

#if ENGINE_ENABLE_VEC2


inline Vec2 operator*(const float& b, const Angle &a){ return {b*a.c, b*a.s}; };

inline Vec2 rotate(Vec2 u, float c, float s){
    return {c*u.x-s*u.y, s*u.x+c*u.y};
}
inline Vec2 rotate(float x, float y, float c, float s){
    return {c*x-s*y, s*x+c*y};
}
inline Vec2 rotate(Vec2 u, Angle a){
    return {a.c*u.x-a.s*u.y, a.s*u.x+a.c*u.y};
}
inline Vec2 rotate(float x, float y, Angle a){
    return {a.c*x-a.s*y, a.s*x+a.c*y};
}
inline Vec2 flipRotate(Vec2 u, float c, float s){
    return {-c*u.x-s*u.y, -s*u.x+c*u.y};
}
inline Vec2 flipRotate(float x, float y, float c, float s){
    return {-c*x-s*y, -s*x+c*y};
}
inline Vec2 flipRotate(Vec2 u, Angle a){
    return {-a.c*u.x-a.s*u.y, -a.s*u.x+a.c*u.y};
}
inline Vec2 flipRotate(float x, float y, Angle a){
    return {-a.c*x-a.s*y, -a.s*x+a.c*y};
}
inline Vec2 rotate(float x, Angle a){
    return {x*a.c, x*a.s};
}
inline Vec2 flipRotate(float x, Angle a){
    return {-x*a.c, x*a.s};
}
inline Angle::Angle(Vec2 v){
    v.normalize();
    c = v.x;
    s = v.y;
};
template <typename T> inline _Vec2<T>::_Vec2(Angle a){
    x = a.c;
    y = a.s;
};
inline float dot(Vec2 v, Angle a){
    return v.x*a.c+v.y*a.s;
};
inline float dot(Angle a, Vec2 v){
    return v.x*a.c+v.y*a.s;
};
#endif

#endif


#if ENGINE_ENABLE_VEC3
/* Vectors of dimension 3 */
template <typename T> struct _Vec3{
    T x, y, z;
    // arithmetic operators with _Vec3r
    inline _Vec3 operator+(const _Vec3& b) const { return {x+b.x, y+b.y, z+b.z}; }
    inline _Vec3 operator-(const _Vec3& b) const { return {x-b.x, y-b.y, z-b.z}; }
    inline _Vec3 operator*(const _Vec3& b) const { return {x*b.x, y*b.y, z*b.z}; }
    inline _Vec3 operator/(const _Vec3& b) const { return {x/b.x, y/b.y, z/b.z}; }
    
    // op= operators
    inline _Vec3& operator+=(const _Vec3& b) { *this = *this+b; return *this; }
    inline _Vec3& operator-=(const _Vec3& b) { *this = *this-b; return *this; }
    inline _Vec3& operator*=(const _Vec3& b) { *this = *this*b; return *this; }
    inline _Vec3& operator/=(const _Vec3& b) { *this = *this/b; return *this; }
    
    // arithmetic operators with real
    inline _Vec3 operator+(T b) const { return {x+b, y+b, z+b}; }
    inline _Vec3 operator-(T b) const { return {x-b, y-b, z-b}; }
    inline _Vec3 operator*(T b) const { return {x*b, y*b, z*b}; }
    inline _Vec3 operator/(T b) const { return {x/b, y/b, z/b}; }
    
    // op= operators with real
    inline _Vec3& operator+=(T b) { *this = *this+b; return *this; }
    inline _Vec3& operator-=(T b) { *this = *this-b; return *this; }
    inline _Vec3& operator*=(T b) { *this = *this*b; return *this; }
    inline _Vec3& operator/=(T b) { *this = *this/b; return *this; }
    
    inline bool operator==(_Vec3 v) { return this->x == v.x && this->y == v.y && this->z == v.z; }
    inline bool operator!=(_Vec3 v) { return this->x != v.x || this->y != v.y || this->z != v.z; }
    
    inline _Vec3 operator-(void) const{
        return {-x, -y, -z};
    }
    
    inline void normalize();
    
    inline _Vec3(T ox, T oy, T oz){
        x = ox;
        y = oy;
        z = oz;
    };
    inline _Vec3(_Vec2<T> u, T oz){
        x = u.x;
        y = u.y;
        z = oz;
    };
    inline _Vec3(_Vec2<T> u){
        x = u.x;
        y = u.y;
        z = 0.f;
    };
    inline _Vec3(){};
};
template <typename T> inline _Vec3<T> operator*(const T b, const _Vec3<T> v) {
    return {v.x*b, v.y*b, v.z*b};
}
template <typename T> inline _Vec3<T> operator/(const T b, const _Vec3<T> v) {
    return {v.x/b, v.y/b, v.z/b};
}

template <typename T> inline T dot(_Vec3<T> u, _Vec3<T> v){
    return u.x*v.x+u.y*v.y+u.z*v.z;
}
template <typename T> inline float lenSqr(_Vec3<T> u){
    return dot(u, u);
}
template <typename T> inline float len(_Vec3<T> u){
    return sqrtf(lenSqr(u));
}
template <typename T> inline _Vec3<T> normalize(_Vec3<T> u){
    return u/len(u);
}
template <typename T> inline void _Vec3<T>::normalize(){
    *this /= len(*this);
}
template <typename T> inline _Vec3<T> cross(_Vec3<T> u, _Vec3<T> v){
    return {
        u.y*v.z-u.z*v.y,
        u.z*v.x-u.x*v.z,
        u.x*v.y-u.y*v.x
    };
}
template <typename T> inline _Vec3<T> lerp(float t, _Vec3<T> u, _Vec3<T> v){
    return u*(1.f-t)+v*t;
}

typedef _Vec3<float> Vec3;
typedef _Vec3<int>   Vec3i;

#if ENGINE_ENABLE_ANGLE

inline Vec3 rotate(Vec3 u, float c, float s){
    return {c*u.x-s*u.y, s*u.x+c*u.y, u.z};
}
inline Vec3 rotate(float x, float y, float z, float c, float s){
    return {c*x-s*y, s*x+c*y, z};
}
inline Vec3 rotate(Vec3 u, Angle a){
    return {a.c*u.x-a.s*u.y, a.s*u.x+a.c*u.y, u.z};
}
inline Vec3 rotate(float x, float y, float z, Angle a){
    return {a.c*x-a.s*y, a.s*x+a.c*y, z};
}
inline Vec3 flipVector(Vec3 u){
    return {-u.x, u.y, u.z};
}
inline Vec3 flipRotate(Vec3 u, float c, float s){
    return {-c*u.x-s*u.y, -s*u.x+c*u.y, u.z};
}
inline Vec3 flipRotate(Vec3 u, Angle a){
    return {-a.c*u.x-a.s*u.y, -a.s*u.x+a.c*u.y, u.z};
}
inline Vec3 flipRotate(float x, float y, float z, Angle a){
    return {-a.c*x-a.s*y, -a.s*x+a.c*y, z};
}

#endif

#endif


#if ENGINE_ENABLE_VEC4
/* Vectors of dimension 4 */
template <typename T> struct _Vec4{
    T x, y, z, w;
    // arithmetic operators with _Vec3r
    inline _Vec4 operator+(const _Vec4& b) const { return {x+b.x, y+b.y, z+b.z, w+b.w}; }
    inline _Vec4 operator-(const _Vec4& b) const { return {x-b.x, y-b.y, z-b.z, w-b.w}; }
    inline _Vec4 operator*(const _Vec4& b) const { return {x*b.x, y*b.y, z*b.z, w*b.w}; }
    inline _Vec4 operator/(const _Vec4& b) const { return {x/b.x, y/b.y, z/b.z, w/b.w}; }
    
    // op= operators
    inline _Vec4& operator+=(const _Vec4& b) { *this = *this+b; return *this; }
    inline _Vec4& operator-=(const _Vec4& b) { *this = *this-b; return *this; }
    inline _Vec4& operator*=(const _Vec4& b) { *this = *this*b; return *this; }
    inline _Vec4& operator/=(const _Vec4& b) { *this = *this/b; return *this; }
    
    // arithmetic operators with real
    inline _Vec4 operator+(T b) const { return {x+b, y+b, z+b, w+b}; }
    inline _Vec4 operator-(T b) const { return {x-b, y-b, z-b, w-b}; }
    inline _Vec4 operator*(T b) const { return {x*b, y*b, z*b, w*b}; }
    inline _Vec4 operator/(T b) const { return {x/b, y/b, z/b, w/b}; }
    
    // op= operators with real
    inline _Vec4& operator+=(T b) { *this = *this+b; return *this; }
    inline _Vec4& operator-=(T b) { *this = *this-b; return *this; }
    inline _Vec4& operator*=(T b) { *this = *this*b; return *this; }
    inline _Vec4& operator/=(T b) { *this = *this/b; return *this; }
    
    inline bool operator==(_Vec4 v) { return this->x == v.x && this->y == v.y && this->z == v.z; }
    inline bool operator!=(_Vec4 v) { return this->x != v.x || this->y != v.y || this->z != v.z; }
    
    inline _Vec4 operator-(void) const{
        return {-x, -y, -z, -w};
    }
    
    inline void normalize();
    
    inline _Vec4(T ox, T oy, T oz, T ow){
        x = ox;
        y = oy;
        z = oz;
        w = ow;
    };
    inline _Vec4(_Vec2<T> u, _Vec2<T> v){
        x = u.x;
        y = u.y;
        z = v.x;
        w = v.y;
    };
    inline _Vec4(_Vec2<T> u){
        x = u.x;
        y = u.y;
        z = 0.f;
        w = 0.f;
    };
    inline _Vec4(){};
};
template <typename T> inline _Vec4<T> operator*(const T b, const _Vec4<T> v) {
    return {v.x*b, v.y*b, v.z*b, v.w*b};
}
template <typename T> inline _Vec4<T> operator/(const T b, const _Vec4<T> v) {
    return {v.x/b, v.y/b, v.z/b};
}

template <typename T> inline T dot(_Vec4<T> u, _Vec4<T> v){
    return u.x*v.x+u.y*v.y+u.z*v.z+u.w*v.w;
}
template <typename T> inline float lenSqr(_Vec4<T> u){
    return dot(u, u);
}
template <typename T> inline float len(_Vec4<T> u){
    return sqrtf(lenSqr(u));
}
template <typename T> inline _Vec4<T> normalize(_Vec4<T> u){
    return u/len(u);
}
template <typename T> inline void _Vec4<T>::normalize(){
    *this /= len(*this);
}
template <typename T> inline _Vec4<T> lerp(float t, _Vec4<T> u, _Vec4<T> v){
    return u*(1.f-t)+v*t;
}

typedef _Vec4<float> Vec4;
typedef _Vec4<int>   Vec4i;

#endif

/* 2x2 matrices */
template <typename T> struct _Mat2{
    T _00, _01, _10, _11;
    
    inline _Mat2 operator+(const _Mat2& m) const {
        return {
            _00+m._00, _01+m._01,
            _10+m._10, _11+m._11
        };
    }
    inline _Mat2 operator-(const _Mat2& m) const {
        return {
            _00-m._00, _01-m._01,
            _10-m._10, _11-m._11
        };
    }
    inline _Mat2 operator*(const _Mat2& m) const {
        return {
            _00*m._00+_01*m._10, _00*m._01+_01*m._11,
            _10*m._00+_11*m._10, _10*m._01+_11*m._11
        };
    }
    
    inline _Mat2& operator+=(const _Mat2& m) {
        *this = *this+m;
        return *this;
    }
    inline _Mat2& operator-=(const _Mat2& m) {
        *this = *this-m;
        return *this;
    }
    inline _Mat2& operator*=(const _Mat2& m) {
        *this = *this*m;
        return *this;
    }
    
    inline _Mat2(T b00, T b01, T b10, T b11){
        _00 = b00;
        _01 = b01;
        _10 = b10;
        _11 = b11;
    };
};

template <typename T> inline _Vec2<T> operator*(const _Mat2<T>& m, const _Vec2<T>& v) {
    return {
        m._00*v.x+m._01*v.y,
        m._10*v.x+m._11*v.y
    };
}
template <typename T> inline _Vec2<T> operator*(const _Vec2<T>& v, const _Mat2<T>& m) {
    return {
        m._00*v.x+m._10*v.y,
        m._01*v.x+m._11*v.y
    };
}

/* 3x3 matrices */
template <typename T> struct _Mat3{
    T _00, _01, _02, _10, _11, _12, _20, _21, _22;
    
    inline _Mat3 operator+(const _Mat3& m) const {
        return {
            _00+m._00, _01+m._01, _02+m._02,
            _10+m._10, _11+m._11, _12+m._12,
            _20+m._20, _21+m._21, _22+m._22
        };
    }
    inline _Mat3 operator-(const _Mat3& m) const {
        return {
            _00-m._00, _01-m._01, _02-m._02,
            _10-m._10, _11-m._11, _12-m._12,
            _20-m._20, _21-m._21, _22-m._22
        };
    }
    inline _Mat3 operator*(const _Mat3& m) const {
        return {
            _00*m._00+_01*m._10+_02*m._20, _00*m._01+_01*m._11+_02*m._21, _00*m._02+_01*m._12+_02*m._22,
            _10*m._00+_11*m._10+_12*m._20, _10*m._01+_11*m._11+_12*m._21, _10*m._02+_11*m._12+_12*m._22,
            _20*m._00+_21*m._10+_22*m._20, _20*m._01+_21*m._11+_22*m._21, _20*m._02+_21*m._12+_22*m._22,
        };
    }
    
    inline _Mat3& operator+=(const _Mat3& m) {
        *this = *this+m;
        return *this;
    }
    inline _Mat3& operator-=(const _Mat3& m) {
        *this = *this-m;
        return *this;
    }
    inline _Mat3& operator*=(const _Mat3& m) {
        *this = *this*m;
        return *this;
    }
    
    inline _Mat3(T b00, T b01, T b02, T b10, T b11, T b12, T b20, T b21, T b22){
        _00 = b00;
        _01 = b01;
        _02 = b02;
        _10 = b10;
        _11 = b11;
        _12 = b12;
        _20 = b20;
        _21 = b21;
        _22 = b22;
    };
};
template <typename T> inline _Vec3<T> operator*(const _Mat3<T>& m, const _Vec3<T>& v) {
    return {
        m._00*v.x+m._01*v.y+m._02*v.z,
        m._10*v.x+m._11*v.y+m._12*v.z,
        m._20*v.x+m._21*v.y+m._22*v.z
    };
}
template <typename T> inline _Vec3<T> operator*(const _Vec3<T>& v, const _Mat3<T>& m) {
    return {
        m._00*v.x+m._10*v.y+m._20*v.z,
        m._01*v.x+m._11*v.y+m._21*v.z,
        m._02*v.x+m._12*v.y+m._22*v.z
    };
}

#if ENGINE_ENABLE_MAT4
/* 4x4 matrices */
template <typename T> struct _Mat4{
    T _00, _01, _02, _03, _10, _11, _12, _13, _20, _21, _22, _23, _30, _31, _32, _33;
    
    inline _Mat4 operator+(const _Mat4& m) const {
        return {
            _00+m._00, _01+m._01, _02+m._02, _03+m._03,
            _10+m._10, _11+m._11, _12+m._12, _13+m._13,
            _20+m._20, _21+m._21, _22+m._22, _23+m._23,
            _30+m._30, _31+m._31, _32+m._32, _33+m._33,
        };
    }
    inline _Mat4 operator-(const _Mat4& m) const {
        return {
            _00-m._00, _01-m._01, _02-m._02, _03-m._03,
            _10-m._10, _11-m._11, _12-m._12, _13-m._13,
            _20-m._20, _21-m._21, _22-m._22, _23-m._23,
            _30-m._30, _31-m._31, _32-m._32, _33-m._33,
        };
    }
    inline _Mat4 operator*(const _Mat4& m) const {
        return {
            _00*m._00+_01*m._10+_02*m._20+_03*m._30, _00*m._01+_01*m._11+_02*m._21+_03*m._31, _00*m._02+_01*m._12+_02*m._22+_03*m._32, _00*m._03+_01*m._13+_02*m._23+_03*m._33,
            _10*m._00+_11*m._10+_12*m._20+_13*m._30, _10*m._01+_11*m._11+_12*m._21+_13*m._31, _10*m._02+_11*m._12+_12*m._22+_13*m._32, _10*m._03+_11*m._13+_12*m._23+_13*m._33,
            _20*m._00+_21*m._10+_22*m._20+_23*m._30, _20*m._01+_21*m._11+_22*m._21+_23*m._31, _20*m._02+_21*m._12+_22*m._22+_23*m._32, _20*m._03+_21*m._13+_22*m._23+_23*m._33,
            _30*m._00+_31*m._10+_32*m._20+_33*m._30, _30*m._01+_31*m._11+_32*m._21+_33*m._31, _30*m._02+_31*m._12+_32*m._22+_33*m._32, _30*m._03+_31*m._13+_32*m._23+_33*m._33,
        };
    }
    
    inline _Mat4& operator+=(const _Mat4& m) {
        *this = *this+m;
        return *this;
    }
    inline _Mat4& operator-=(const _Mat4& m) {
        *this = *this-m;
        return *this;
    }
    inline _Mat4& operator*=(const _Mat4& m) {
        *this = *this*m;
        return *this;
    }
    
    inline _Mat4(T b00, T b01, T b02, T b03, T b10, T b11, T b12, T b13, T b20, T b21, T b22, T b23, T b30, T b31, T b32, T b33){
        _00 = b00;
        _01 = b01;
        _02 = b02;
        _03 = b03;
        _10 = b10;
        _11 = b11;
        _12 = b12;
        _13 = b13;
        _20 = b20;
        _21 = b21;
        _22 = b22;
        _23 = b23;
        _30 = b30;
        _31 = b31;
        _32 = b32;
        _33 = b33;
    };
    
    inline _Mat4(){};
};
template <typename T> inline _Vec4<T> operator*(const _Mat4<T>& m, const _Vec4<T>& v) {
    return {
        m._00*v.x+m._01*v.y+m._02*v.z+m._03*v.w,
        m._10*v.x+m._11*v.y+m._12*v.z+m._13*v.w,
        m._20*v.x+m._21*v.y+m._22*v.z+m._23*v.w,
        m._30*v.x+m._31*v.y+m._32*v.z+m._33*v.w,
    };
}
template <typename T> inline _Vec4<T> operator*(const _Vec4<T>& v, const _Mat4<T>& m) {
    return {
        m._00*v.x+m._10*v.y+m._20*v.z+m._30*v.w,
        m._01*v.x+m._11*v.y+m._21*v.z+m._31*v.w,
        m._02*v.x+m._12*v.y+m._22*v.z+m._32*v.w,
        m._03*v.x+m._13*v.y+m._23*v.z+m._33*v.w,
    };
}
typedef _Mat4<float> Mat4;
#endif


typedef _Mat2<float> Mat2;
typedef _Mat3<float> Mat3;

template <typename T> struct _Quaternion{
    T s, i, j, k;
    inline _Quaternion operator+(const _Quaternion& b) const { return {s+b.s, i+b.i, j+b.j, k+b.k}; }
    inline _Quaternion operator*(const _Quaternion& q) const {
        return {
            -i * q.i - j * q.j - k * q.k + s * q.s,
            i * q.s + j * q.k - k * q.j + s * q.i,
            -i * q.k + j * q.s + k * q.i + s * q.j,
            i * q.j - j * q.i + k * q.s + s * q.k
        };
    }
    inline _Quaternion operator*(float b) const { return {s*b, i*b, j*b, k*b}; }
    inline _Quaternion operator/(float b) const { return {s/b, i/b, j/b, k/b}; }
    inline _Quaternion operator-(void) const{
        return {-s, -i, -j, -k};
    }
};

typedef _Quaternion<float> Quaternion;

inline float dot(Quaternion q1, Quaternion q2){
    return q1.s*q2.s+q1.i*q2.i+q1.j*q2.j+q1.k*q2.k;
}

inline Quaternion rotateAround(Vec3 n, float theta){
    float s = sinf(theta/2.f), c = cosf(theta/2.f);
    return {
        c,
        s*n.x,
        s*n.y,
        s*n.z
    };
};
inline Quaternion rotateTo(Vec3 o, Vec3 f){
    float c = dot(o, f);
    Vec3 n = cross(o, f);
    float s = sqrtf((1.f+c)*2.f);
    return {
        0.5f*s,
        n.x/s,
        n.y/s,
        n.z/s
    };
};
inline Vec3 rotateVector(Vec3 v, Quaternion q){
    Vec3 u = Vec3(q.i, q.j, q.k);
    return 2.f*dot(u, v)*u + (q.s*q.s-dot(u, u))*v + 2.f*q.s*cross(u, v);
}

#if ENGINE_ENABLE_MAT4
inline Mat4 vectorRotation(Quaternion q){
    float is = q.i*q.s;
    float ii = q.i*q.i;
    float ij = q.i*q.j;
    float ik = q.i*q.k;
    float js = q.j*q.s;
    float jj = q.j*q.j;
    float jk = q.j*q.k;
    float ks = q.k*q.s;
    float kk = q.k*q.k;
    return {
        1.f-2.f*(jj+kk), 2.f*(ij-ks),     2.f*(ik+js),     0.f,
        2.f*(ij+ks),     1.f-2.f*(ii+kk), 2.f*(jk-is),     0.f,
        2.f*(ik-js),     2.f*(jk+is),     1.f-2.f*(ii+jj), 0.f,
        0.f,             0.f,             0.f,             1.f
    };
}
#endif

inline Quaternion slerp(float t, Quaternion q1, Quaternion q2){
    float qdot = dot(q1, q2);
    if (qdot < 0.f){
        qdot = -qdot;
        q2 = -q2;
    }
    if(qdot < 0.95f){
        float angle = acosf(qdot);
        return (q1*sinf(angle*(1-t)) + q2*sinf(angle*t))/sinf(angle);
    }else{
        Quaternion q = q1*(1.f-t)+q2*t;
        return q/sqrtf(dot(q, q));
    }
}

#endif


#ifndef M_3_PI_4
#define M_3_PI_4 2.35619449019
#define M_5_PI_4 3.92699081699
#define M_7_PI_4 5.49778714378

#ifdef ENGINE_ENABLE_ARRAYS

#include <string.h>

#include "memory.hpp"

template <typename T> struct Array{
    u32 size, reserved;
    T *data;
    Allocator *allocator = &defaultAllocator;
    
    T& operator[](u32 index){
        if(index > size){
            printf("Array out of index!\n");
            assert(0);
        }
        return data[index];
    }
    
    void operator=(Array a){
        size = a.size;
        reserved = a.reserved;
        data = a.data;
        allocator = a.allocator;
    }
    
    void add(u32 plus){
        if(size == 0 || data == nullptr){
            data = (T *)allocator->allocate(allocator, plus*sizeof(T));
            size = plus;
            reserved = plus;
        }else{
            reserve(size+plus);
            size += plus;
        }
    }
    void copyTo(Array<T> &a){
        a.size = size;
        if(size > 0){
            assert(size*sizeof(T) < 5000000); // To detect errors (we may need to increase the number)
            a.reserved = reserved;
            a.data = (T*)a.allocator->allocate(a.allocator, a.reserved*sizeof(T));
            memcpy(a.data, data, size*sizeof(T));
        }else{
            assert(size == 0);
            a.size = 0;
            a.reserved = 0;
            a.data = nullptr;
        }
    }
    
    void remove(u32 index){
        for(u32 i=index+1; i<size; i++){
            data[i-1] = data[i];
        }
        size--;
        // TODO: free unused memory
    }
    
    void resize(u32 s){
        if(s < size){
            //data = (T*)allocator->reallocate(allocator, data, s*sizeof(T));
            // @Todo: free unused memory
        }else if(s > size){
            add(s-size);
        }
        size = s;
    }
    
    void push(T t){
        add(1);
        data[size-1] = t;
    }
    
    void reserve(u32 r){
        if(reserved > 0 || data != nullptr){
            if(reserved < r){
                T *oldData = data;
                reserved = r;
                data = (T*)allocator->allocate(allocator, reserved*sizeof(T));
                memcpy(data, oldData, size*sizeof(T));
                allocator->deallocate(allocator, oldData);
            }
        }else{
            reserved = r;
            data = (T*)allocator->allocate(allocator, reserved*sizeof(T));
        }
    }
    void reserveToFitSize(){
        reserved = size;
        data = (T*)allocator->allocate(allocator, reserved*sizeof(T));
    }
    
    Array(){
        size = 0;
        reserved = 0;
        data = nullptr;
    }
    
    Array(u32 s){
        size = s;
        reserved = s;
        data = (T *)allocator->allocate(allocator, s*sizeof(T));
    }
    
    Array(Allocator &al){
        size = 0;
        reserved = 0;
        data = nullptr;
        allocator = &al;
    }
    
    Array(u32 s, Allocator &al){
        size = s;
        reserved = s;
        allocator = &al;
        data = (T *)allocator->allocate(allocator, s*sizeof(T));
    }
};
//template <typename T> void readArray(Array<T> &a, FILE *fp);
//template <typename T> void readArray(Array<T> *a, FILE *fp);
//template <typename T> void writeArray(Array<T> &a, FILE *fp);
//template <typename T> void writeArray(Array<T> *a, FILE *fp);
//template <typename T> void free(Array<T> &a);
template <typename T> inline void readArray(Array<T> &a, FILE *fp){
    fread(&a.size, sizeof(u32), 1, fp);
    a.reserved = a.size;
    if(a.size > 0){
        a.data = (T *)a.allocator->allocate(a.allocator, a.size*sizeof(T));
        fread(a.data, sizeof(T), a.size, fp);
    }
}
template <typename T> inline void readArray(Array<T> *a, FILE *fp){
    fread(&a->size, sizeof(u32), 1, fp);
    a->reserved = a->size;
    if(a->size > 0){
        a->data = (T *)a->allocator->allocate(a->allocator, a->size*sizeof(T));
        fread(a->data, sizeof(T), a->size, fp);
    }
}
template <typename T> inline void writeArray(Array<T> &a, FILE *fp){
    fwrite(&a.size, sizeof(u32), 1, fp);
    fwrite(a.data, sizeof(T), a.size, fp);
}
template <typename T> inline void writeArray(Array<T> *a, FILE *fp){
    fwrite(&a->size, sizeof(u32), 1, fp);
    fwrite(a->data, sizeof(T), a->size, fp);
}
template <typename T> inline void free(Array<T> &a){
    if(a.size > 0 && a.data != nullptr)
        a.allocator->deallocate(a.allocator, a.data);
    a.size = 0;
    a.reserved = 0;
}

template <typename T, u32 aS> struct StaticArray{
    u32 size;
#if OS == OS_WINDOWS
	T data[aS];// = {}; // I hate c++
#else
    T data[aS] = {};
#endif
    
    T& operator[](u32 index){
        if(index > size){
            printf("Array out of index!\n");
            assert(0);
        }
        return data[index];
    }
    
    void operator=(StaticArray a){
        size = a.size;
        memcpy(a.data, data, sizeof(data));
    }
    
    void add(u32 plus){
        size += plus;
        assert(size <= aS);
    }
    
    void remove(u32 index){
        for(u32 i=index+1; i<size; i++){
            data[i-1] = data[i];
        }
        size--;
    }
    
    void slice(u32 s){
        size = s;
    }
    
    void push(T t){
        size++;
        assert(size <= aS);
        data[size-1] = t;
    }
    
    StaticArray(){
        size = 0;
    }
    
    StaticArray(u32 s){
        size = s;
    }
};
template <typename T, u32 aS> inline void readArray(StaticArray<T, aS> *a, FILE *fp){
    fread(&a->size, sizeof(u32), 1, fp);
    fread(a->data, sizeof(T), a->size, fp);
}
template <typename T, u32 aS> inline void writeArray(StaticArray<T, aS> *a, FILE *fp){
    fwrite(&a->size, sizeof(u32), 1, fp);
    fwrite(a->data, sizeof(T), a->size, fp);
}
template <typename T, u32 aS> inline void free(StaticArray<T, aS> &a){
    a.size = 0;
}

#if ENGINE_ENABLE_S_ARRAYS

// 1 bit arrays

struct Array_bool{
    u32 size, byteSize;
    uint8 *data;
    Allocator *allocator = &defaultAllocator;
    
    uint8 operator[](u32 index){
        if(index > size){
            printf("Bool array out of index!\n");
            return 0;
        }
        uint8 dig = index%8;
        return ((*(data+index/8)) & (1 << dig)) >> dig;
    }
    void set(u32 index, uint8 value){
        if(index > size){
            printf("Bool array out of index!\n");
            return;
        }
        switch(index%8){
            case 0:
                data[index/8] = (data[index/8] & 0xfe) | value;
                break;
            case 1:
                data[index/8] = (data[index/8] & 0xfd) | (value << 1);
                break;
            case 2:
                data[index/8] = (data[index/8] & 0xfb) | (value << 2);
                break;
            case 3:
                data[index/8] = (data[index/8] & 0xf7) | (value << 3);
                break;
            case 4:
                data[index/8] = (data[index/8] & 0xef) | (value << 4);
                break;
            case 5:
                data[index/8] = (data[index/8] & 0xdf) | (value << 5);
                break;
            case 6:
                data[index/8] = (data[index/8] & 0xbf) | (value << 6);
                break;
            case 7:
                data[index/8] = (data[index/8] & 0x7f) | (value << 7);
                break;
        }
    }
    
    Array_bool(){
        size = 0;
        byteSize = 0;
        data = nullptr;
    }
    
    Array_bool(u32 s){
        size = s;
        byteSize = (s+7)/8;
        data = (uint8 *)allocator->allocate(allocator, byteSize*sizeof(uint8));
    }
};
inline void readArray(Array_bool *a, FILE *fp){
    fread(&a->size, sizeof(u32), 1, fp);
    a->byteSize = (a->size+7)/8;
    a->data = (uint8 *)a->allocator->allocate(a->allocator, a->byteSize*sizeof(uint8));
    fread(a->data, sizeof(uint8), a->byteSize, fp);
}
inline void writeArray(Array_bool *a, FILE *fp){
    fwrite(&a->size, sizeof(u32), 1, fp);
    fwrite(a->data, sizeof(uint8), a->byteSize, fp);
}
inline void free(Array_bool &a){
    if(a.data != nullptr)
        free(a.data);
    a.size = 0;
    a.byteSize = 0;
}

// 2 bit arrays

struct Array_uint2{
    u32 size, byteSize;
    uint8 *data;
    
    uint8 operator[](u32 index){
        if(index > size){
            printf("Uint2 array out of index!\n");
            return 0;
        }
        uint8 dig = 2*(index%4);
        return ((*(data+index/4)) & (3 << dig)) >> dig;
    }
    void set(u32 index, uint8 value){
        if(index > size){
            printf("Uint2 array out of index!\n");
            return;
        }
        switch(index%4){
            case 0:
                data[index/4] = (data[index/4] & 0xfc) | value;
                break;
            case 1:
                data[index/4] = (data[index/4] & 0xf3) | (value << 2);
                break;
            case 2:
                data[index/4] = (data[index/4] & 0xcf) | (value << 4);
                break;
            case 3:
                data[index/4] = (data[index/4] & 0x3f) | (value << 6);
                break;
        }
    }
    
    Array_uint2(){
        size = 0;
        byteSize = 0;
        data = nullptr;
    }
    
    Array_uint2(u32 s){
        size = s;
        byteSize = (s+3)/4;
        data = (uint8 *)malloc(byteSize*sizeof(uint8));
    }
};
inline void readArray(Array_uint2 *a, FILE *fp){
    fread(&a->size, sizeof(u32), 1, fp);
    a->byteSize = (a->size+3)/4;
    a->data = (uint8 *)malloc(a->byteSize*sizeof(uint8));
    fread(a->data, sizeof(uint8), a->byteSize, fp);
}
inline void writeArray(Array_uint2 *a, FILE *fp){
    fwrite(&a->size, sizeof(u32), 1, fp);
    fwrite(a->data, sizeof(uint8), a->byteSize, fp);
}
inline void free(Array_uint2 &a){
    if(a.data != nullptr)
        free(a.data);
    a.size = 0;
    a.byteSize = 0;
}

// 4 bit array

struct Array_uint4{
    u32 size, byteSize;
    uint8 *data;
    Allocator *allocator = &defaultAllocator;
    
    uint8 operator[](u32 index){
        if(index > size){
            printf("Uint4 array out of index! at(%i/%i)\n", index, size);
            assert(0);
            return 0;
        }
        if(index & 1){
            return (data[index/2] & 0xf0) >> 4;
        }else{
            return (data[index/2] & 0x0f) ;
        }
    }
    void set(u32 index, uint8 value){
        if(index > size){
            printf("Uint4 array out of index! set(%i/%i)\n", index, size);
            assert(0);
            return;
        }
        if(index & 1){
            data[index/2] = (data[index/2] & 0x0f) | (value << 4);
        }else{
            data[index/2] = (data[index/2] & 0xf0) | value;
        }
    }
    
    Array_uint4(){
        size = 0;
        byteSize = 0;
        data = nullptr;
    }
    
    Array_uint4(u32 s){
        size = s;
        byteSize = (s+1)/2;
        data = (uint8 *)allocator->allocate(allocator, byteSize*sizeof(uint8));
    }
};
inline void readArray(Array_uint4 *a, FILE *fp){
    fread(&a->size, sizeof(u32), 1, fp);
    a->byteSize = (a->size+1)/2;
    a->data = (uint8 *)malloc(a->byteSize*sizeof(uint8));
    fread(a->data, sizeof(uint8), a->byteSize, fp);
}
inline void writeArray(Array_uint4 *a, FILE *fp){
    fwrite(&a->size, sizeof(u32), 1, fp);
    fwrite(a->data, sizeof(uint8), a->byteSize, fp);
}
inline void free(Array_uint4 &a){
    if(a.data != nullptr)
        free(a.data);
    a.size = 0;
    a.byteSize = 0;
}


// Static Arrays

// 1 bit arrays

template <u32 aS> struct StaticArray_bool{
    u32 actualByteSize;
    u32 size, byteSize;
    uint8 data[(aS+7)/8];
    
    uint8 operator[](u32 index){
        if(index > size){
            printf("Bool array out of index!\n");
            return 0;
        }
        uint8 dig = index%8;
        return ((*(data+index/8)) & (1 << dig)) >> dig;
    }
    void set(u32 index, uint8 value){
        if(index > size){
            printf("Bool array out of index!\n");
            return;
        }
        switch(index%8){
            case 0:
                data[index/8] = (data[index/8] & 0xfe) | value;
                break;
            case 1:
                data[index/8] = (data[index/8] & 0xfd) | (value << 1);
                break;
            case 2:
                data[index/8] = (data[index/8] & 0xfb) | (value << 2);
                break;
            case 3:
                data[index/8] = (data[index/8] & 0xf7) | (value << 3);
                break;
            case 4:
                data[index/8] = (data[index/8] & 0xef) | (value << 4);
                break;
            case 5:
                data[index/8] = (data[index/8] & 0xdf) | (value << 5);
                break;
            case 6:
                data[index/8] = (data[index/8] & 0xbf) | (value << 6);
                break;
            case 7:
                data[index/8] = (data[index/8] & 0x7f) | (value << 7);
                break;
        }
    }
    
    StaticArray_bool(){
        size = 0;
        byteSize = 0;
        actualByteSize = (aS+7)/8;
    }
    
    StaticArray_bool(u32 s){
        size = s;
        byteSize = (s+7)/8;
        actualByteSize = (aS+7)/8;
    }
};
template <u32 aS> inline void readArray(StaticArray_bool<aS> *a, FILE *fp){
    fread(&a->size, sizeof(u32), 1, fp);
    a->byteSize = (a->size+7)/8;
    a->actualByteSize = (aS+7)/8;
    assert(a->byteSize <= a->actualByteSize);
    fread(a->data, sizeof(uint8), a->byteSize, fp);
}
template <u32 aS> inline void writeArray(StaticArray_bool<aS> *a, FILE *fp){
    fwrite(&a->size, sizeof(u32), 1, fp);
    fwrite(a->data, sizeof(uint8), a->byteSize, fp);
}
template <u32 aS> inline void free(StaticArray_bool<aS> &a){
    a.size = 0;
    a.byteSize = 0;
}

// 2 bit arrays

template <u32 aS> struct StaticArray_uint2{
    u32 actualByteSize;
    u32 size, byteSize;
    uint8 data[(aS+3)/4];
    
    uint8 operator[](u32 index){
        if(index >= size){
            printf("Uint2 array out of index (%i/%i)!\n", index, size);
            assert(0);
            return 0;
        }
        uint8 dig = 2*(index%4);
        return ((*(data+index/4)) & (3 << dig)) >> dig;
    }
    void set(u32 index, uint8 value){
        if(index >= size){
            printf("Uint2 array out of index! (%i/%i)\n", index, size);
            return;
        }
        switch(index%4){
            case 0:
                data[index/4] = (data[index/4] & 0xfc) | value;
                break;
            case 1:
                data[index/4] = (data[index/4] & 0xf3) | (value << 2);
                break;
            case 2:
                data[index/4] = (data[index/4] & 0xcf) | (value << 4);
                break;
            case 3:
                data[index/4] = (data[index/4] & 0x3f) | (value << 6);
                break;
        }
    }
    
    StaticArray_uint2(){
        size = 0;
        actualByteSize = (aS+3)/4;
    }
    
    StaticArray_uint2(u32 s){
        size = s;
        byteSize = (s+3)/4;
        actualByteSize = (aS+3)/4;
    }
};
template <u32 aS> inline void readArray(StaticArray_uint2<aS> *a, FILE *fp){
    fread(&a->size, sizeof(u32), 1, fp);
    a->byteSize = (a->size+3)/4;
    a->actualByteSize = (aS+3)/4;
    assert(a->byteSize > a->actualByteSize);
    fread(a->data, sizeof(uint8), a->byteSize, fp);
}
template <u32 aS> inline void writeArray(StaticArray_uint2<aS> *a, FILE *fp){
    fwrite(&a->size, sizeof(u32), 1, fp);
    fwrite(a->data, sizeof(uint8), a->byteSize, fp);
}
template <u32 aS> inline void free(StaticArray_uint2<aS> &a){
    a.size = 0;
    a.byteSize = 0;
}

// 4 bit array

template <u32 aS> struct StaticArray_uint4{
    u32 actualByteSize;
    u32 size, byteSize;
    uint8 data[(aS+1)/2];
    
    uint8 operator[](u32 index){
        if(index > size){
            printf("Uint4 array out of index! at(%i/%i)\n", index, size);
            return 0;
        }
        if(index & 1){
            return (data[index/2] & 0xf0) >> 4;
        }else{
            return (data[index/2] & 0x0f) ;
        }
    }
    void set(u32 index, uint8 value){
        if(index > size){
            printf("Uint4 array out of index! set(%i/%i)\n", index, size);
            return;
        }
        if(index & 1){
            data[index/2] = (data[index/2] & 0x0f) | (value << 4);
        }else{
            data[index/2] = (data[index/2] & 0xf0) | value;
        }
    }
    
    StaticArray_uint4(){
        size = 0;
        actualByteSize = (aS+1)/2;
    }
    
    StaticArray_uint4(u32 s){
        size = s;
        byteSize = (s+1)/2;
        actualByteSize = (aS+1)/2;
    }
};
template <u32 aS> inline void readArray(StaticArray_uint4<aS> *a, FILE *fp){
    fread(&a->size, sizeof(u32), 1, fp);
    a->byteSize = (a->size+1)/2;
    a->actualByteSize = (aS+1)/2;
    assert(a->byteSize <= a->actualByteSize);
    fread(a->data, sizeof(uint8), a->byteSize, fp);
}
template <u32 aS> inline void writeArray(StaticArray_uint4<aS> *a, FILE *fp){
    fwrite(&a->size, sizeof(u32), 1, fp);
    fwrite(a->data, sizeof(uint8), a->byteSize, fp);
}
template <u32 aS> inline void free(StaticArray_uint4<aS> &a){
    a.size = 0;
    a.byteSize = 0;
}

#endif

#endif

#if ENGINE_ENABLE_MATRICES

template <typename T> struct Matrix{
    u32 cols, rows;
    Array<T> array;
    Allocator *allocator = &defaultAllocator;
    
    T& at(u32 i, u32 j){
        assert(i < rows);
        assert(j < cols);
        return array[i*cols+j];
    }
    void set(u32 i, u32 j, T v){
        assert(i < rows);
        assert(j < cols);
        array[i*cols+j] = v;
    }
    T* operator[](u32 i){
        assert(i < rows);
        return &array[i*cols];
    }
    
    void operator=(Matrix a){
        cols = a.cols;
        rows = a.rows;
        array = a.array;
        allocator = a.allocator;
    }
    
    Matrix(){
        rows = 0;
        cols = 0;
        array = Array<T>();
    }
    
    Matrix(u32 r, u32 c){
        rows = r;
        cols = c;
        array = Array<T>(r*c);
    }
    
    Matrix(Allocator &al){
        rows = 0;
        cols = 0;
        allocator = &al;
        array = Array<T>(al);
    }
    
    Matrix(u32 s, Allocator &al){
        rows = s;
        cols = s;
        allocator = &al;
        array = Array<T>(al);
    }
};
template <typename T> inline void readMatrix(Matrix<T> *a, FILE *fp){
    fread(&a->rows, sizeof(u32), 1, fp);
    fread(&a->cols, sizeof(u32), 1, fp);
    a->array = Array<T>(a->rows*a->cols);
    fread(a->array.data, sizeof(T), a->array.size, fp);
}
template <typename T> inline void writeMatrix(Matrix<T> *a, FILE *fp){
    fwrite(&a->rows, sizeof(u32), 1, fp);
    fwrite(&a->cols, sizeof(u32), 1, fp);
    fwrite(a->array.data, sizeof(T), a->array.size, fp);
}
template <typename T> inline void free(Matrix<T> &a){
    free(a.array);
    a.rows = 0;
    a.cols = 0;
}

// Static Matrices

template <typename T, u32 aR, u32 aC> struct StaticMatrix{
    u32 rows, cols;
    StaticArray<T, aR*aC> array; // I hate c++
    
    T* operator[](u32 i){
        assert(i <= rows);
        return array.data+(i*cols);
    }
    
    StaticMatrix() : cols(0), rows(0) {
        array = StaticArray<T, aR*aC>();
    }
    
    StaticMatrix(u32 s) : cols(s), rows(s) {
        array = StaticArray<T, aR*aC>(s*s);
    }
    
    StaticMatrix(u32 c, u32 r) : cols(c), rows(r){
        array = StaticArray<T, aR*aC>(c*r);
    }
};
template <typename T, u32 aR, u32 aC> inline void readMatrix(StaticMatrix<T, aR, aC> *a, FILE *fp){
    fread(&a->rows, sizeof(u32), 1, fp);
    fread(&a->cols, sizeof(u32), 1, fp);
    fread(a->array.data, sizeof(T), a->array.size, fp);
}
template <typename T, u32 aR, u32 aC> inline void writeMatrix(StaticMatrix<T, aR, aC> *a, FILE *fp){
    fwrite(&a->rows, sizeof(u32), 1, fp);
    fwrite(&a->cols, sizeof(u32), 1, fp);
    fwrite(a->array.data, sizeof(T), a->array.size, fp);
}
template <typename T, u32 aR, u32 aC> inline void free(StaticMatrix<T, aR, aC> &a){
    a.rows = 0;
    a.cols = 0;
    free(a.array);
}

#if ENGINE_ENABLE_S_MATRICES

// 1 bit matrix

struct Matrix_bool{
    u32 rows, cols;
    Array_bool array;
    
    uint8 at(u32 i, u32 j){
        return array[i*cols+j];
    }
    void set(u32 i, u32 j, uint8 value){
        array.set(i*cols+j, value);
    }
    
    Matrix_bool(){
        cols = 0;
        rows = 0;
        array = Array_bool();
    }
    
    Matrix_bool(u32 c, u32 r){
        cols = c;
        rows = r;
        array = Array_bool(c*r);
    }
};
inline void readMatrix(Matrix_bool *a, FILE *fp){
    fread(&a->cols, sizeof(u32), 1, fp);
    fread(&a->rows, sizeof(u32), 1, fp);
    a->array.size = a->cols*a->rows;
    a->array.byteSize = (a->array.size+7)/8;
    a->array.data = (uint8 *)malloc(a->array.byteSize*sizeof(uint8));
    fread(a->array.data, sizeof(uint8), a->array.byteSize, fp);
}
inline void writeMatrix(Matrix_bool *a, FILE *fp){
    fwrite(&a->cols, sizeof(u32), 1, fp);
    fwrite(&a->rows, sizeof(u32), 1, fp);
    fwrite(a->array.data, sizeof(uint8), a->array.byteSize, fp);
}
inline void free(Matrix_bool &a){
    free(a.array);
    a.rows = 0;
    a.cols = 0;
}



struct Matrix_uint4{
    u32 rows, cols;
    Array_uint4 array;
    
    uint8 at(u32 i, u32 j){
        return array[i*cols+j];
    }
    void set(u32 i, u32 j, uint8 value){
        array.set(i*cols+j, value);
    }
    
    Matrix_uint4(){
        rows = 0;
        cols = 0;
        array = Array_uint4();
    }
    
    Matrix_uint4(u32 d){
        rows = d;
        cols = d;
        array = Array_uint4(d*d);
    }
    Matrix_uint4(u32 r, u32 c){
        rows = r;
        cols = c;
        array = Array_uint4(r*c);
    }
    
    inline Matrix_uint4 copy(){
        Matrix_uint4 a;
        a.rows = rows;
        a.cols = cols;
        a.array.size = array.size;
        a.array.byteSize = array.byteSize;
        a.array.data = (uint8 *)malloc(a.array.byteSize*sizeof(uint8));
        memcpy(a.array.data, array.data, array.byteSize*sizeof(uint8));
        return a;
    }
};
inline void readMatrix(Matrix_uint4 *a, FILE *fp){
    fread(&a->rows, sizeof(u32), 1, fp);
    fread(&a->cols, sizeof(u32), 1, fp);
    a->array.size = a->cols*a->rows;
    a->array.byteSize = (a->array.size+1)/2;
    a->array.data = (uint8 *)malloc(a->array.byteSize*sizeof(uint8));
    fread(a->array.data, sizeof(uint8), a->array.byteSize, fp);
}
inline void writeMatrix(Matrix_uint4 *a, FILE *fp){
    fwrite(&a->rows, sizeof(u32), 1, fp);
    fwrite(&a->cols, sizeof(u32), 1, fp);
    fwrite(a->array.data, sizeof(uint8), a->array.byteSize, fp);
}
inline void free(Matrix_uint4 &a){
    free(a.array);
    a.rows = 0;
    a.cols = 0;
}

// Static Matrices

// 1 bit matrix

template <u32 aR, u32 aC> struct StaticMatrix_bool{
    u32 rows, cols;
    StaticArray_bool<aC*aR> array;
    
    uint8 at(u32 i, u32 j){
        return array[i*cols+j];
    }
    void set(u32 i, u32 j, uint8 value){
        array.set(i*cols+j, value);
    }
    
    StaticMatrix_bool(){
        cols = 0;
        rows = 0;
        array = StaticArray_bool<aC*aR>();
    }
    
    StaticMatrix_bool(u32 r, u32 c){
        cols = c;
        rows = r;
        array = StaticArray_bool<aC*aR>(c*r);
    }
};
template <u32 aR, u32 aC> inline void readMatrix(StaticMatrix_bool<aR, aC> *a, FILE *fp){
    fread(&a->cols, sizeof(u32), 1, fp);
    fread(&a->rows, sizeof(u32), 1, fp);
    a->array.size = a->cols*a->rows;
    a->array.byteSize = (a->array.size+7)/8;
    a->array.actualByteSize = ((aC*aR)+7)/8;
    assert(a->byteSize <= a->actualByteSize);
    fread(a->array.data, sizeof(uint8), a->array.byteSize, fp);
}
template <u32 aR, u32 aC> inline void writeMatrix(StaticMatrix_bool<aR, aC> *a, FILE *fp){
    fwrite(&a->cols, sizeof(u32), 1, fp);
    fwrite(&a->rows, sizeof(u32), 1, fp);
    fwrite(a->array.data, sizeof(uint8), a->array.byteSize, fp);
}
template <u32 aR, u32 aC> inline void free(StaticMatrix_bool<aR, aC> &a){
    free(a.array);
    a.rows = 0;
    a.cols = 0;
}

// 2 bit matrix
template <u32 aR, u32 aC> struct StaticMatrix_uint2{
    u32 rows, cols;
    StaticArray_uint2<aC*aR> array;
    
    uint8 at(u32 i, u32 j){
        return array[i*cols+j];
    }
    void set(u32 i, u32 j, uint8 value){
        array.set(i*cols+j, value);
    }
    
    StaticMatrix_uint2(){
        cols = 0;
        rows = 0;
        array = StaticArray_uint2<aC*aR>();
    }
    
    StaticMatrix_uint2(u32 r, u32 c){
        cols = c;
        rows = r;
        array = StaticArray_uint2<aC*aR>(c*r);
    }
};
template <u32 aR, u32 aC> inline void readMatrix(StaticMatrix_uint2<aR, aC> *a, FILE *fp){
    fread(&a->cols, sizeof(u32), 1, fp);
    fread(&a->rows, sizeof(u32), 1, fp);
    a->array.size = a->cols*a->rows;
    a->array.byteSize = (a->array.size+3)/4;
    a->array.actualByteSize = ((aC*aR)+3)/4;
    assert(a->array.byteSize <= a->array.actualByteSize);
    fread(a->array.data, sizeof(uint8), a->array.byteSize, fp);
}
template <u32 aR, u32 aC> inline void writeMatrix(StaticMatrix_uint2<aR, aC> *a, FILE *fp){
    fwrite(&a->cols, sizeof(u32), 1, fp);
    fwrite(&a->rows, sizeof(u32), 1, fp);
    fwrite(a->array.data, sizeof(uint8), a->array.byteSize, fp);
}
template <u32 aR, u32 aC> inline void free(StaticMatrix_uint2<aR, aC> &a){
    free(a.array);
    a.rows = 0;
    a.cols = 0;
}



template <u32 aR, u32 aC> struct StaticMatrix_uint4{
    u32 rows, cols;
    StaticArray_uint4<aC*aR> array;
    
    uint8 at(u32 i, u32 j){
        return array[i*cols+j];
    }
    void set(u32 i, u32 j, uint8 value){
        array.set(i*cols+j, value);
    }
    
    StaticMatrix_uint4(){
        rows = 0;
        cols = 0;
        array = StaticArray_uint4<aC*aR>();
    }
    
    StaticMatrix_uint4(u32 d){
        rows = d;
        cols = d;
        array = StaticArray_uint4<aC*aR>(d*d);
    }
    StaticMatrix_uint4(u32 r, u32 c){
        rows = r;
        cols = c;
        array = StaticArray_uint4<aC*aR>(r*c);
    }
    
    inline StaticMatrix_uint4 copy(){
        StaticMatrix_uint4<aR, aC> a;
        a.rows = rows;
        a.cols = cols;
        a.array.size = array.size;
        a.array.byteSize = array.byteSize;
        a.array.actualByteSize = array.actualByteSize;
        memcpy(a.array.data, array.data, array.byteSize*sizeof(uint8));
        return a;
    }
};
template <u32 aR, u32 aC> inline void readMatrix(StaticMatrix_uint4<aR, aC> *a, FILE *fp){
    fread(&a->rows, sizeof(u32), 1, fp);
    fread(&a->cols, sizeof(u32), 1, fp);
    a->array.size = a->cols*a->rows;
    a->array.byteSize = (a->array.size+1)/2;
    a->array.actualByteSize = ((aC*aR)+1)/2;
    assert(a->array.byteSize <= a->array.actualByteSize);
    fread(a->array.data, sizeof(uint8), a->array.byteSize, fp);
}
template <u32 aR, u32 aC> inline void writeMatrix(StaticMatrix_uint4<aR, aC> *a, FILE *fp){
    fwrite(&a->rows, sizeof(u32), 1, fp);
    fwrite(&a->cols, sizeof(u32), 1, fp);
    fwrite(a->array.data, sizeof(uint8), a->array.byteSize, fp);
}
template <u32 aR, u32 aC> inline void free(StaticMatrix_uint4<aR, aC> &a){
    free(a.array);
    a.rows = 0;
    a.cols = 0;
}

#endif

#endif

#if ENGINE_ENABLE_STRINGS

struct String{
    char *text;
    u32 length, reserved;
    bool initialized;
    Allocator *allocator = &defaultAllocator;
    
    void operator+=(char c);
    void operator+=(const char *s);
    void operator+=(String s);
    
    bool operator==(String s);
    bool operator==(const char *s);
    bool operator!=(String s);
    bool operator!=(const char *s);
    
    String();
    String(const char *s);
    String(u32 toReserve);
    String(Allocator &allocator);
    String(const char *s, Allocator &allocator);
    String(u32 toReserve, Allocator &allocator);
    
    void operator=(const char *s);
    void operator=(String s);
    
    void reserve(u32 toReserve);
};

void free(String &s);

#endif

#if ENGINE_ENABLE_STACKS

struct GeneralStack{
    MemoryPool arena;
    u32 half_size;
    u8 *half_start;
    
    void push(void *p, u32 size){
        u8 *ptr = (u8 *)arena.allocator.allocate(&arena.allocator, size+2*sizeof(u32));
        *((u32 *)ptr) = size;
        ptr += sizeof(u32);
        memcpy(ptr, p, size);
        *((u32 *)(ptr+size)+1) = size;
    }
    void *push(u32 size){
        u8 *ptr = (u8 *)arena.allocator.allocate(&arena.allocator, size+2*sizeof(u32));
        *((u32 *)ptr) = size;
        ptr += sizeof(u32);
        *((u32 *)(ptr+size)+1) = size;
        return ptr;
    }
    
    void *half_push(void *p, u32 size){
        u8 *ptr = (u8 *)arena.allocator.allocate(&arena.allocator, size);
        memcpy(ptr, p, size);
        half_size += size;
        return ptr;
    }
    void *half_push(u32 size){
        u8 *ptr = (u8 *)arena.allocator.allocate(&arena.allocator, size);
        half_size += size;
        return ptr;
    }
    template <typename T> T* half_push(Array<T> a){
        half_push(&a.size, sizeof(u32));
        void *ptr = arena.memory+arena.occupied;
        half_push(a.data, a.size*sizeof(T));
        return (T *)ptr;
    }
    template <typename T, unsigned int aS> T* half_push(StaticArray<T, aS> a){
        half_push(&a.size, sizeof(u32));
        void *ptr = arena.memory+arena.occupied;
        half_push(a.data, a.size*sizeof(T));
        return (T *)ptr;
    }
    void start_push(){
        half_start = (u8 *)arena.allocator.allocate(&arena.allocator, sizeof(u32));
        half_size = 0;
    };
    void end_push(){
        u8 *ptr = (u8 *)arena.allocator.allocate(&arena.allocator, sizeof(u32));
        *((u32 *)ptr) = half_size;
        *((u32 *)half_start) = half_size;
    };
    
    void *pop(){
        if(arena.occupied == 0)
            return nullptr;
        u32 size = *((u32*)(arena.memory+arena.occupied)-1);
        u8 *p = arena.memory+arena.occupied-sizeof(u32)-size;
        arena.occupied = p-arena.memory-sizeof(u32);
        return p;
    }
    
    void removeFirst(){
        if(arena.occupied == 0)
            return;
        u32 size = *((u32*)(arena.memory)-1);
        size += 2*sizeof(u32);
        /*for(int i=size; i<arena.occupied; i++){ // @Todo: use memcpy
            arena.memory[i-size] = arena.memory[i];
        }*/
        memmove(arena.memory, arena.memory+size, arena.occupied-size);
        arena.occupied -= size;
        half_start -= size;
    }
    void *peek(){
        if(arena.occupied == 0)
            return nullptr;
        u32 size = *((u32*)(arena.memory+arena.occupied)-1);
        return arena.memory+arena.occupied-sizeof(u32)-size;
    }
    GeneralStack(){
        arena = MemoryPool();
        half_size = 0;
    }
    GeneralStack(u64 s){
        arena = MemoryPool(s);
        half_size = 0;
    }
    GeneralStack(u64 s, u8 *p){
        arena = MemoryPool(s, p);
        half_size = 0;
    }
};

inline GeneralStack getGeneralStack(u64 s){
    GeneralStack gs;
    gs.arena = getMemoryPool(s);
    gs.half_size = 0;
    return gs;
}

template <u32 bytes_per_bucket, u32 buckets_num> struct BucketArray {
    u8 *memory;
    StaticArray_bool<buckets_num> used_buckets;
    u32 next_bucket;
    
    void free_buckets(){
        u8 *starting_bucket_ptr = memory+bytes_per_bucket*next_bucket;
        u32 buckets_occupied = *(u32 *)starting_bucket_ptr;
        for(int i=0; i<buckets_occupied; i++){
            used_buckets.set(next_bucket+1, false);
        }
    }

    u8 *reserve_buckets(u32 buckets, u32 *entries_removed_ptr){
        assert(buckets <= buckets_num);
        bool enough_space_at_the_end = next_bucket+buckets <= buckets_num;
        if(!enough_space_at_the_end)
            next_bucket = 0;
        
        u8 *ptr = memory+bytes_per_bucket*next_bucket;
        
        // Write a preffix that specifies how long the entry is.
        *(u32 *)ptr = buckets;
        ptr += sizeof(u32);
        
        // Write a suffix that specifies how long the entry is.
        *(u32 *) (memory+bytes_per_bucket*(next_bucket+buckets)-sizeof(u32)) = buckets;
        
        // @Performance: Optimize...
        u32 entries_removed = 0;
        while(buckets > 0){
            if(used_buckets[next_bucket]){
                entries_removed++;
                free_buckets();
            }
            next_bucket++;
            buckets--;
        };
        
        if(entries_removed_ptr != nullptr)
            *entries_removed_ptr = entries_removed;
        return ptr;
    }
    u8 *reserve_memory_in_bytes(u32 bytes, u32 *buckets_necessary_ptr, u32 *entries_removed_ptr){
        u32 buckets_necessary = (bytes+2*sizeof(u32)-1) / bytes_per_bucket + 1;
        if(buckets_necessary_ptr !=  nullptr)
            *buckets_necessary_ptr = buckets_necessary;
        return reserve_buckets(buckets_necessary, entries_removed_ptr);
    }
    
    u8 *get_and_remove_last_entry(){
        u8 *size_ptr = memory+bytes_per_bucket*next_bucket-sizeof(u32);
        u32 occupied_buckets = *(u32 *)size_ptr;
        for(int i=next_bucket-1; i>=next_bucket-occupied_buckets; i--)
            used_buckets.set(i, false);
        assert(next_bucket >= occupied_buckets);
        next_bucket = next_bucket-occupied_buckets;
        return memory+bytes_per_bucket*next_bucket + sizeof(u32);
    }
    
    u8 *get_last_entry(){
        u8 *size_ptr = memory+bytes_per_bucket*next_bucket-sizeof(u32);
        u32 occupied_buckets = *(u32 *)size_ptr;
        assert(next_bucket >= occupied_buckets);
        u32 old_bucket = next_bucket-occupied_buckets;
        return memory+bytes_per_bucket*old_bucket + sizeof(u32);
    }
    
    BucketArray(void *m){
        memory = (u8 *)m;
        next_bucket = 0;
        used_buckets = StaticArray_bool<buckets_num>(buckets_num);
        for(int i=0; i<used_buckets.byteSize; i++){
            used_buckets.data[i] = 0;
        }
    }
    BucketArray(){
        memory = nullptr;
        next_bucket = 0;
        used_buckets = StaticArray_bool<buckets_num>(buckets_num);
        for(int i=0; i<used_buckets.byteSize; i++){
            used_buckets.data[i] = 0;
        }
    }
};

template <u32 bytes_per_bucket, u32 buckets_num> void reset_bucket_array(BucketArray<bytes_per_bucket, buckets_num> *ba){
    ba->next_bucket = 0;
    for(int i=0; i<ba->used_buckets.byteSize; i++){
        ba->used_buckets.data[i] = 0;
    }
}

#endif

#endif
