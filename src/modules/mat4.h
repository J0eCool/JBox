#pragma once

class Mat4 : public FixedBuffer<float, 16> {
public:
    // default: identity matrix
    Mat4() : FixedBuffer<float, 16>((float[16]){
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0,
    }) {}

    Mat4(float const _data[16]) : FixedBuffer<float, 16>(_data) {}

    static Mat4 identity() {
        return Mat4();
    }

    Mat4 operator*(float scale) const {
        auto ret = Mat4();
        for (int i = 0; i < 16; ++i) {
            ret[i] = (*this)[i] * scale;
        }
        return ret;
    }
    Mat4 operator*(Mat4 const& other) const {
        auto ret = Mat4();
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

    static Mat4 translate(float x, float y, float z = 0.0) {
        return Mat4((float[]){
            1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            x, y, z, 1.0,
        });
    }
    static Mat4 scale(float x, float y, float z = 1.0) {
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

    static Mat4 orthographic(float left, float right, float bottom, float top, float near, float far) {
        return Mat4((float[]){
            2.0f / (right-left), 0.0, 0.0, 0.0,
            0.0, 2.0f / (top-bottom), 0.0, 0.0,
            0.0, 0.0, 2.0f / (near-far), 0.0,
            (left+right) / (left-right), (bottom+top) / (bottom-top), (near+far) / (near-far), 1.0,
        });
    }
    static Mat4 uiOrtho(float width, float height, float depth = 100.0) {
        // Ortho, but with 0,0 as the top-left rather than bottom-left
        return orthographic(0, width, height, 0, 0, depth);
    }
    static Mat4 perspective(float fov, float aspect, float near, float far) {
        float f = tan(PI/2 - fov/2);
        float invRange = 1.0 / (near - far);
        return Mat4((float[]){
            f / aspect, 0.0, 0.0, 0.0,
            0.0, f, 0.0, 0.0,
            0.0, 0.0, (near+far) * invRange, -1.0,
            0.0, 0.0, near * far * invRange * 2.0f, 0.0,
        });
    }
};
