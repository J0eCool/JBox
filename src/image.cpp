/**IT_START**/

import "canvas" {
    func setPixel(s32, s32, s32);
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

int width = 400;
int height = 300;

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
    Point A(120, 130);
    Point B(190, 180);
    Point C(275, 140);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Point p(x, y);
            int r = 0xff * circle(A, p, 120) * ((x+y) % 3 == 0);
            int g = 0xff * circle(B, p, 100) * ((x+y) % 3 == 1);;
            int b = 0xff * circle(C, p, 140) * ((x+y) % 3 == 2);;
            setPixel(x, y, rgb(r, g, b));
        }
    }
}
