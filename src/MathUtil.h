#pragma once

#include <cmath>

namespace math {

constexpr float kPi = 3.14159265358979323846f;

inline float degToRad(float deg) { return deg * (kPi / 180.0f); }

struct Vec3 {
    float x = 0, y = 0, z = 0;

    Vec3() = default;
    Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    Vec3 operator+(const Vec3& r) const { return {x + r.x, y + r.y, z + r.z}; }
    Vec3 operator-(const Vec3& r) const { return {x - r.x, y - r.y, z - r.z}; }
    Vec3 operator*(float s) const { return {x * s, y * s, z * s}; }
};

inline float dot(const Vec3& a, const Vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline float lengthSq(const Vec3& v) { return dot(v, v); }
inline float length(const Vec3& v) { return std::sqrt(lengthSq(v)); }

struct Mat4 {
    // Column-major, OpenGL style.
    float m[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    static Mat4 identity();
    static Mat4 translation(const Vec3& t);
    static Mat4 rotationX(float deg);
    static Mat4 rotationY(float deg);
    static Mat4 rotationZ(float deg);

    Mat4 operator*(const Mat4& rhs) const;
    Vec3 transformPoint(const Vec3& p) const;

    static Mat4 inverseRigidBody(const Mat4& t);
};

inline float clamp(float v, float lo, float hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
}

} // namespace math
