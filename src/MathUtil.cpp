#include "MathUtil.h"

namespace math {

Mat4 Mat4::identity() {
    return Mat4{};
}

Mat4 Mat4::translation(const Vec3& t) {
    Mat4 r = Mat4::identity();
    r.m[12] = t.x;
    r.m[13] = t.y;
    r.m[14] = t.z;
    return r;
}

Mat4 Mat4::rotationX(float deg) {
    float c = std::cos(degToRad(deg));
    float s = std::sin(degToRad(deg));

    Mat4 r = Mat4::identity();
    r.m[5] = c;
    r.m[6] = s;
    r.m[9] = -s;
    r.m[10] = c;
    return r;
}

Mat4 Mat4::rotationY(float deg) {
    float c = std::cos(degToRad(deg));
    float s = std::sin(degToRad(deg));

    Mat4 r = Mat4::identity();
    r.m[0] = c;
    r.m[2] = -s;
    r.m[8] = s;
    r.m[10] = c;
    return r;
}

Mat4 Mat4::rotationZ(float deg) {
    float c = std::cos(degToRad(deg));
    float s = std::sin(degToRad(deg));

    Mat4 r = Mat4::identity();
    r.m[0] = c;
    r.m[1] = s;
    r.m[4] = -s;
    r.m[5] = c;
    return r;
}

Mat4 Mat4::operator*(const Mat4& rhs) const {
    Mat4 out;
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            out.m[col * 4 + row] =
                m[0 * 4 + row] * rhs.m[col * 4 + 0] +
                m[1 * 4 + row] * rhs.m[col * 4 + 1] +
                m[2 * 4 + row] * rhs.m[col * 4 + 2] +
                m[3 * 4 + row] * rhs.m[col * 4 + 3];
        }
    }
    return out;
}

Vec3 Mat4::transformPoint(const Vec3& p) const {
    float x = m[0] * p.x + m[4] * p.y + m[8] * p.z + m[12];
    float y = m[1] * p.x + m[5] * p.y + m[9] * p.z + m[13];
    float z = m[2] * p.x + m[6] * p.y + m[10] * p.z + m[14];
    return {x, y, z};
}

Mat4 Mat4::inverseRigidBody(const Mat4& t) {
    // Assumes upper-left 3x3 is a pure rotation matrix and last row is [0 0 0 1].
    Mat4 inv = Mat4::identity();

    // Transpose rotation.
    inv.m[0] = t.m[0];
    inv.m[1] = t.m[4];
    inv.m[2] = t.m[8];

    inv.m[4] = t.m[1];
    inv.m[5] = t.m[5];
    inv.m[6] = t.m[9];

    inv.m[8] = t.m[2];
    inv.m[9] = t.m[6];
    inv.m[10] = t.m[10];

    // inv translation = -(R^T * t)
    Vec3 trans{t.m[12], t.m[13], t.m[14]};
    Vec3 invT = inv.transformPoint(trans * -1.0f); // using inv as rotation-only right now

    inv.m[12] = invT.x;
    inv.m[13] = invT.y;
    inv.m[14] = invT.z;

    return inv;
}

} // namespace math
