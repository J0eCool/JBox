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

    Vec operator*(Vec const& v) const {
        const Mat& m = *this;
        return Vec(
            v.x * m[0] + v.y * m[1] + v.z * m[2] + m[3],
            v.x * m[4] + v.y * m[5] + v.z * m[6] + m[7],
            v.x * m[8] + v.y * m[9] + v.z * m[10] + m[11]);
    }

    static Mat translate(float x, float y, float z = 0.0) {
        return Mat((float[]){
            1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            x, y, z, 1.0,
        });
    }
    static Mat translate(Vec const& v) {
        return translate(v.x, v.y, v.z);
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
    static Mat scale(Vec const& v) {
        return scale(v.x, v.y, v.z);
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

    static Mat lookAt(Vec pos, Vec target, Vec up = Vec(0, 0, 1)) {
        Vec zDir = (pos - target).unit();
        Vec xDir = up.cross(zDir).unit();
        Vec yDir = zDir.cross(xDir).unit();
        return Mat((float[]){
            xDir.x, xDir.y, xDir.z, 0,
            yDir.x, yDir.y, yDir.z, 0,
            zDir.x, zDir.y, zDir.z, 0,
            pos.x, pos.y, pos.z, 1,
        });
    }

    Mat inverse() const {
        // stolen+adapted from https://webglfundamentals.org/webgl/lessons/webgl-3d-camera.html
        const Mat& m = *this;
        auto m00 = m[0 * 4 + 0];
        auto m01 = m[0 * 4 + 1];
        auto m02 = m[0 * 4 + 2];
        auto m03 = m[0 * 4 + 3];
        auto m10 = m[1 * 4 + 0];
        auto m11 = m[1 * 4 + 1];
        auto m12 = m[1 * 4 + 2];
        auto m13 = m[1 * 4 + 3];
        auto m20 = m[2 * 4 + 0];
        auto m21 = m[2 * 4 + 1];
        auto m22 = m[2 * 4 + 2];
        auto m23 = m[2 * 4 + 3];
        auto m30 = m[3 * 4 + 0];
        auto m31 = m[3 * 4 + 1];
        auto m32 = m[3 * 4 + 2];
        auto m33 = m[3 * 4 + 3];
        auto tmp_0  = m22 * m33;
        auto tmp_1  = m32 * m23;
        auto tmp_2  = m12 * m33;
        auto tmp_3  = m32 * m13;
        auto tmp_4  = m12 * m23;
        auto tmp_5  = m22 * m13;
        auto tmp_6  = m02 * m33;
        auto tmp_7  = m32 * m03;
        auto tmp_8  = m02 * m23;
        auto tmp_9  = m22 * m03;
        auto tmp_10 = m02 * m13;
        auto tmp_11 = m12 * m03;
        auto tmp_12 = m20 * m31;
        auto tmp_13 = m30 * m21;
        auto tmp_14 = m10 * m31;
        auto tmp_15 = m30 * m11;
        auto tmp_16 = m10 * m21;
        auto tmp_17 = m20 * m11;
        auto tmp_18 = m00 * m31;
        auto tmp_19 = m30 * m01;
        auto tmp_20 = m00 * m21;
        auto tmp_21 = m20 * m01;
        auto tmp_22 = m00 * m11;
        auto tmp_23 = m10 * m01;

        auto t0 = (tmp_0 * m11 + tmp_3 * m21 + tmp_4 * m31) -
            (tmp_1 * m11 + tmp_2 * m21 + tmp_5 * m31);
        auto t1 = (tmp_1 * m01 + tmp_6 * m21 + tmp_9 * m31) -
            (tmp_0 * m01 + tmp_7 * m21 + tmp_8 * m31);
        auto t2 = (tmp_2 * m01 + tmp_7 * m11 + tmp_10 * m31) -
            (tmp_3 * m01 + tmp_6 * m11 + tmp_11 * m31);
        auto t3 = (tmp_5 * m01 + tmp_8 * m11 + tmp_11 * m21) -
            (tmp_4 * m01 + tmp_9 * m11 + tmp_10 * m21);

        auto d = 1.0f / (m00 * t0 + m10 * t1 + m20 * t2 + m30 * t3);

        return Mat((float[16]){
          d * t0,
          d * t1,
          d * t2,
          d * t3,
          d * ((tmp_1 * m10 + tmp_2 * m20 + tmp_5 * m30) -
                (tmp_0 * m10 + tmp_3 * m20 + tmp_4 * m30)),
          d * ((tmp_0 * m00 + tmp_7 * m20 + tmp_8 * m30) -
                (tmp_1 * m00 + tmp_6 * m20 + tmp_9 * m30)),
          d * ((tmp_3 * m00 + tmp_6 * m10 + tmp_11 * m30) -
                (tmp_2 * m00 + tmp_7 * m10 + tmp_10 * m30)),
          d * ((tmp_4 * m00 + tmp_9 * m10 + tmp_10 * m20) -
                (tmp_5 * m00 + tmp_8 * m10 + tmp_11 * m20)),
          d * ((tmp_12 * m13 + tmp_15 * m23 + tmp_16 * m33) -
                (tmp_13 * m13 + tmp_14 * m23 + tmp_17 * m33)),
          d * ((tmp_13 * m03 + tmp_18 * m23 + tmp_21 * m33) -
                (tmp_12 * m03 + tmp_19 * m23 + tmp_20 * m33)),
          d * ((tmp_14 * m03 + tmp_19 * m13 + tmp_22 * m33) -
                (tmp_15 * m03 + tmp_18 * m13 + tmp_23 * m33)),
          d * ((tmp_17 * m03 + tmp_20 * m13 + tmp_23 * m23) -
                (tmp_16 * m03 + tmp_21 * m13 + tmp_22 * m23)),
          d * ((tmp_14 * m22 + tmp_17 * m32 + tmp_13 * m12) -
                (tmp_16 * m32 + tmp_12 * m12 + tmp_15 * m22)),
          d * ((tmp_20 * m32 + tmp_12 * m02 + tmp_19 * m22) -
                (tmp_18 * m22 + tmp_21 * m32 + tmp_13 * m02)),
          d * ((tmp_18 * m12 + tmp_23 * m32 + tmp_15 * m02) -
                (tmp_22 * m32 + tmp_14 * m02 + tmp_19 * m12)),
          d * ((tmp_22 * m22 + tmp_16 * m02 + tmp_21 * m12) -
                (tmp_20 * m12 + tmp_23 * m22 + tmp_17 * m02)),
        });
    }
};
