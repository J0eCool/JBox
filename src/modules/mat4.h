#pragma once

#include <string.h>

class Mat4 {
    float data[16];
    Buffer<float> buffer;
public:
    // default: identity matrix
    Mat4() : data {
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0,
    }, buffer(16, data) {}

    Mat4(float const _data[16]) : buffer(16, data) {
        memcpy(data, _data, 16*sizeof(float));
    }

    static Mat4 identity() {
        return Mat4();
    }

    Mat4 operator*(float scale) const {
        auto ret = Mat4();
        for (int i = 0; i < 16; ++i) {
            ret.data[i] = data[i] * scale;
        }
        return ret;
    }
    Mat4 operator*(Mat4 const& other) const {
        auto ret = Mat4();
        for (int i = 0; i < 16; ++i) {
            int x = i % 4;
            int y = i / 4;
            ret.data[i] = 0.0;
            for (int j = 0; j < 4; ++j) {
                ret.data[i] += data[x + j * 4] * other.data[j + y * 4];
            }
        }
        return ret;
    }

    static Mat4 translate(float x, float y, float z) {
        return Mat4((float[]){
            1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            x, y, z, 1.0,
        });
    }
    static Mat4 scale(float x, float y, float z) {
        return Mat4((float[]){
            x, 0.0, 0.0, 0.0,
            0.0, y, 0.0, 0.0,
            0.0, 0.0, z, 0.0,
            0.0, 0.0, 0.0, 1.0,
        });
    }
    static Mat4 scale(float s) {
        return Mat4::scale(s, s, s);
    }
    static Mat4 rotateX(float angle) {
        auto c = cos(angle);
        auto s = sin(angle);
        return Mat4((float[]){
            1.0, 0.0, 0.0, 0.0,
            0.0, c, s, 0.0,
            0.0, -s, c, 0.0,
            0.0, 0.0, 0.0, 1.0,
        });
    }
    static Mat4 rotateY(float angle) {
        auto c = cos(angle);
        auto s = sin(angle);
        return Mat4((float[]){
            c, 0.0, -s, 0.0,
            0.0, 1.0, 0.0, 0.0,
            s, 0.0, c, 0.0,
            0.0, 0.0, 0.0, 1.0,
        });
    }
    static Mat4 rotateZ(float angle) {
        auto c = cos(angle);
        auto s = sin(angle);
        return Mat4((float[]){
            c, s, 0.0, 0.0,
            -s, c, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0,
        });
    }

    ITBuffer* toITBuffer() {
        return &buffer;
    }
};
