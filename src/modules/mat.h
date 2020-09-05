#pragma once

#include "vec.h"

// 4x4 matrix
class Mat : public FixedBuffer<float, 16> {
public:
    // default: identity matrix
    Mat() : FixedBuffer<float, 16>((float[16]){
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0,
    }) {}

    Mat(float const _data[16]) : FixedBuffer<float, 16>(_data) {}

    static Mat identity() {
        return Mat();
    }

    Mat operator*(float scale) const {
        auto ret = Mat();
        for (int i = 0; i < 16; ++i) {
            ret[i] = (*this)[i] * scale;
        }
        return ret;
    }
    Mat operator*(Mat const& other) const {
        auto ret = Mat();
        for (int i = 0; i < 16; ++i) {
            int x = i % 4;
            int y = i / 4;
            ret[i] = 0.0;
            for (int j = 0; j < 4; ++j) {
                ret[i] += (*this)[x + j * 4] * other[j + y * 4];
            }
        }
        return ret;
    }

    static Mat translate(float x, float y, float z = 0.0) {
        return Mat((float[]){
            1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            x, y, z, 1.0,
        });
    }
    static Mat translate(Vec const& d) {
        return translate(d.x, d.y, d.z);
    }
    static Mat scale(float x, float y, float z = 1.0) {
        return Mat((float[]){
            x, 0.0, 0.0, 0.0,
            0.0, y, 0.0, 0.0,
            0.0, 0.0, z, 0.0,
            0.0, 0.0, 0.0, 1.0,
        });
    }
    static Mat scale(float s) {
        return Mat::scale(s, s, s);
    }
    static Mat rotateX(float angle) {
        auto c = cos(angle);
        auto s = sin(angle);
        return Mat((float[]){
            1.0, 0.0, 0.0, 0.0,
            0.0, c, s, 0.0,
            0.0, -s, c, 0.0,
            0.0, 0.0, 0.0, 1.0,
        });
    }
    static Mat rotateY(float angle) {
        auto c = cos(angle);
        auto s = sin(angle);
        return Mat((float[]){
            c, 0.0, -s, 0.0,
            0.0, 1.0, 0.0, 0.0,
            s, 0.0, c, 0.0,
            0.0, 0.0, 0.0, 1.0,
        });
    }
    static Mat rotateZ(float angle) {
        auto c = cos(angle);
        auto s = sin(angle);
        return Mat((float[]){
            c, s, 0.0, 0.0,
            -s, c, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0,
        });
    }

    static Mat orthographic(float left, float right, float bottom, float top, float near, float far) {
        return Mat((float[]){
            2.0f / (right-left), 0.0, 0.0, 0.0,
            0.0, 2.0f / (top-bottom), 0.0, 0.0,
            0.0, 0.0, 2.0f / (near-far), 0.0,
            (left+right) / (left-right), (bottom+top) / (bottom-top), (near+far) / (near-far), 1.0,
        });
    }
    static Mat uiOrtho(float width, float height, float depth = 100.0) {
        // Ortho, but with 0,0 as the top-left rather than bottom-left
        return orthographic(0, width, height, 0, 0, depth);
    }
    static Mat perspective(float fov, float aspect, float near, float far) {
        float f = tan(PI/2 - fov/2);
        float invRange = 1.0 / (near - far);
        return Mat((float[]){
            f / aspect, 0.0, 0.0, 0.0,
            0.0, f, 0.0, 0.0,
            0.0, 0.0, (near+far) * invRange, -1.0,
            0.0, 0.0, near * far * invRange * 2.0f, 0.0,
        });
    }
};
