#pragma once

#include <math.h>

struct Point {
    int x, y;
    Point() : x(0), y(0) {}
    Point(int _x, int _y) : x(_x), y(_y) {}

    #define BINOP(op) \
    Point operator op(Point const& p) const { \
        return Point(x op p.x, y op p.y); \
    }
    BINOP(+)
    BINOP(-)
    BINOP(*)
    #undef BINOP

    int size2() const {
        return x * x + y * y;
    }
    int size() const {
        return sqrt(size2());
    }
};

class PixelBuffer : public Buffer<int> {
    int _width;
    int _height;
public:
    PixelBuffer(int w, int h) : Buffer<int>(w*h), _width(w), _height(h) {}
    PixelBuffer() : Buffer<int>(0), _width(0), _height(0) {}
    PixelBuffer(ITBuffer* buffer, int w, int h) : Buffer<int>(buffer, w*h), _width(w), _height(h) {}

    int& ref(int x, int y) {
        return (*(Buffer<int>*)this)[x + y*_width];
    }
    int& operator[](Point p) {
        return ref(p.x, p.y);
    }

    int width() const { return _width; }
    int height() const { return _height; }

    void fill(int color) {
        for (int y = 0; y < _height; ++y) {
            for (int x = 0; x < _width; ++x) {
                this->ref(x, y) = color;
            }
        }
    }
};

int rgb(int r, int g, int b) {
    int a = 0xff;
    return ((0xff & a) << 0x18)
        | ((0xff & r) << 0x00)
        | ((0xff & g) << 0x08)
        | ((0xff & b) << 0x10)
        ;
}

const int white = rgb(0xff, 0xff, 0xff);
const int black = rgb(0, 0, 0);
const int red = rgb(0xff, 0, 0);
const int green = rgb(0, 0xff, 0);
const int blue = rgb(0, 0, 0xff);

const float PI = 3.1415926535;
const float TAU = 2*PI;
