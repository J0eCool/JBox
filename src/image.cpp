/**IT_START**/

import "canvas" {
    func updateImage(buffer);
}
export {
    func run();
}

/**IT_END**/

#include <math.h>

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

const int width = 400;
const int height = 300;
const int numPixels = width * height;

class PixelBuffer : public Buffer<int, numPixels> {
public:
    void setPixel(int x, int y, int color) {
        (*this)[x + y * width] = color;
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

int circle(Point center, Point p, int r) {
    int dist = (p - center).size2();
    return r*r / (dist + 1);
}

PixelBuffer pixels;

// int t = 1 << 28;
int t = 0;

void run() {
    Point A(120, 130);
    Point B(190 + t * 7 / 5, 180);
    Point C(275, 140 + t);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Point p(x, y);
            int r = circle(A, p, 120 + t * 3 / 7 + x * y);
            // pixels.setPixel(x, y, rgb(r, 0, 0));
            int a = 0xff << 24;
            pixels.setPixel(x, y, a | r);
        }
    }

    updateImage(pixels);
    t += 1000;
}
