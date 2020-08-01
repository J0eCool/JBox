#include <malloc.h>

void* malloc(unsigned int);

class ITBuffer {
    int size;
    void* data;
public:
    ITBuffer(int n, void* d) : size(n), data(d) {}
};

template <typename T, int N>
class Buffer {
    T* data;
public:
    Buffer() : data((T*)malloc(N * sizeof(T))) {}

    T& operator[](int idx) {
        return data[idx];
    }

    operator ITBuffer() const {
        return ITBuffer(N * sizeof(T), data);
    }
};

/**IT_START**/

import "canvas" {
    func updateImage(buffer);
    func testy(buffer);
    func log(string, u32);
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
        if (x + y * width > numPixels - 6000) {
            log("YO", color);
        }
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
    int dist = (p - center).size();
    return (dist <= r);
}

void run() {
    PixelBuffer pixels;

    Point A(120, 130);
    Point B(190, 180);
    Point C(275, 140);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Point p(x, y);
            int r = 0xff * circle(A, p, 120);
            int g = 0xff * circle(B, p, 100);
            int b = 0xff * circle(C, p, 140);
            pixels.setPixel(x, y, rgb(r, g, b));
            // pixels.setPixel(x, y, rgb(x * 0xff / width, 0xff, 0xff));
        }
    }
    log("Pixels", pixels[numPixels - 100]);

    updateImage(pixels);

    Buffer<int, 4> foo;
    foo[0] = 2;
    foo[1] = 3;
    foo[2] = 5;
    foo[3] = 7;
    testy(foo);

    Buffer<char, 3> bar;
    bar[0] = 32;
    bar[1] = 12;
    bar[2] = 6;
    testy(bar);
}
