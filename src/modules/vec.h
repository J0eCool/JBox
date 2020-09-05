#pragma once

struct Vec {
    float x, y, z;
    Vec() : x(0.0), y(0.0), z(0.0) {}
    Vec(float k) : x(k), y(k), z(k) {}
    Vec(float _x, float _y, float _z = 0.0f) : x(_x), y(_y), z(_z) {}

    static Vec polar2d(float angle, float r=1.0) {
        return Vec(r * cos(angle), r * sin(angle));
    }

    #define OP(op) \
        Vec operator op(Vec const& v) const { \
            return Vec(x op v.x, y op v.y, z op v.z); \
        }
    OP(+)
    OP(-)
    OP(*)
    OP(/)
    #undef OP

    Vec operator*(float s) const {
        return Vec(s * x, s * y, s * z);
    }

    float dot(Vec const& v) const {
        return x*v.x + y*v.y + z*v.z;
    }
    Vec cross(Vec const& v) const {
        return Vec(
            y*v.z - z*v.y,
            z*v.x - x*v.z,
            x*v.y - y*v.x);
    }

    float size2() const {
        return x*x + y*y + z*z;
    }
    float size() const {
        return sqrt(size2());
    }
    Vec unit() const {
        return (*this) * (1/size());
    }
};

Vec operator*(float s, Vec const& v) {
    return v * s;
}

// where there are vecs, there may be angles; here until I find a better place
const float PI = 3.1415926535;
const float TAU = 2*PI;
