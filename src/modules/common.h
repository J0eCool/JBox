#pragma once

class PixelBuffer : public Buffer<int> {
    int width;
    int height;
public:
    PixelBuffer(int w, int h) : Buffer<int>(w*h), width(w), height(h) {}

    int& ref(int x, int y) {
        return (*this)[x + y*width];
    }
};

struct Point {
    int x, y;
    Point(int _x, int _y) : x(_x), y(_y) {}

    #define BINOP(op) \
    Point operator op(Point const& p) const { \
        return Point(x op p.x, y op p.y); \
    }
    BINOP(+)
    BINOP(-)
    BINOP(*)

    int size2() const {
        return x * x + y * y;
    }
    int size() const {
        return sqrt(size2());
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
