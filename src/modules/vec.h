#pragma once

struct Vec {
    float x, y, z;
    Vec() : x(0.0), y(0.0), z(0.0) {}
    Vec(float _x, float _y, float _z = 0.0f) : x(_x), y(_y), z(_z) {}

    #define OP(op) \
        Vec operator op(Vec const& v) const { \
            return Vec(x op v.x, y op v.y, z op v.z); \
        }
    OP(+)
    OP(-)
    OP(*)
    OP(/)
    #undef OP
};

// where there are vecs, there may be angles; here until I find a better place
const float PI = 3.1415926535;
const float TAU = 2*PI;
