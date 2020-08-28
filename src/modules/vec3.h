#pragma once

struct Vec3 {
    float x, y, z;
    Vec3() : x(0.0), y(0.0), z(0.0) {}
    Vec3(float _x, float _y, float _z = 0.0f) : x(_x), y(_y), z(_z) {}

    Vec3 operator+(Vec3 const& v) const {
        return Vec3(x + v.x, y + v.y, z + v.z);
    }
};
