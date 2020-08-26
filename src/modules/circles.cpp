/**IT_START**/

type Image = struct {
    pixels: buffer;
    width: s32;
    height: s32;
}

import imageDrawing {
    func updateImage(Image);
}
export {
    func init(s32, s32);
    func frame();
}

/**IT_END**/

#include <math.h>

#include "common.h"

int circle(Point center, Point p, int r) {
    int dist = (p - center).size2();
    return r*r / (dist + 1);
}

int width = 800;
int height = 600;
Buffer<int> pixels(0);

// int t = 1 << 28;
int t = 0;

void init(int w, int h) {
    width = w;
    height = h;
    pixels = Buffer<int>(w * h);
}

void frame() {
    Point A(120, 130);
    Point B(190 + t * 7 / 5, 180);
    Point C(275, 140 + t);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Point p(x, y);
            int r = circle(A, p, 120 + t * 3 / 7 + x * y);
            // pixels.setPixel(x, y, rgb(r, 0, 0));
            int a = 0xff << 24;
            pixels[x + y*width] = a | r;
        }
    }

    Image img(&pixels, width, height);
    imageDrawing::updateImage(&img);
    t += 1000;
}
