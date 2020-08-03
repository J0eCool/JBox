/**IT_START**/

import "graphics" {
    func updateImage(buffer);
}
import "math" {
    func rnd() -> f32;
}
export {
    func init(s32, s32);
    func frame();
}

/**IT_END**/

#include <math.h>

#include "common.h"

int width = 800;
int height = 600;

PixelBuffer *curr, *back;

const int nColors = 3;
const int colors[nColors] = { red, blue, green };

void init(int w, int h) {
    width = w;
    height = h;
    curr = new PixelBuffer(w, h);
    back = new PixelBuffer(w, h);

    // for (int y = 0; y < height; ++y) {
    //     for (int x = 0; x < width; ++x) {
    //         curr->ref(x, y) =
    //             ((rnd() < 0.35) ? red : black) |
    //             ((rnd() < 0.35) ? green : black) |
    //             ((rnd() < 0.35) ? blue : black);
    //     }
    // }
}

inline bool isAlive(int x, int y, int mask) {
    return back->ref(x, y) & (mask & 0x00ffffff);
}

int countNeighbors(int x, int y, int mask) {
    int nAlive = 0;

    // // cooler way to do it for cool kids
    // int deltas[8] = { -1, 1, -width, width, -1-width, -1+width, 1-width, 1+width };
    // int p = x + y*width;
    // for (int i = 0; i < 8; ++i) {
    //     nAlive += ((*back)[(p + deltas[i]) % (width*height)] == white);
    // }

    for (int dx = -1; dx <= 1; ++dx) {
        int i = (x + dx + width) % width;
        for (int dy = -1; dy <= 1; ++dy) {
            int j = (y + dy + height) % height;
            if (isAlive(i, j, mask) && (dx != 0 || dy != 0)) {
                nAlive++;
            }
        }
    }
    auto m2 = (mask << 8) | ((mask & 0x00ff0000) >> 16);
    if (rnd() < 0.4 && isAlive(x, y, m2)) {
        nAlive++;
    }

    return nAlive;
}

int t = 0;
void frame() {
    t++;
    // if (t % 2) return;

    // swap the buffers
    auto temp = back;
    back = curr;
    curr = temp;

    // Randomize part of it
    for (int k = 0; k < nColors; ++k) {
        int color = colors[k];
        float s = 9.14 * t / 60 + TAU * k / nColors;
        for (int y = 0; y < 30; ++y) {
            int j = int(y + height * (1.5 + sin(s) / 3)) % height;
            for (int x = 0; x < 30; ++x) {
                int i = int(x + width * (1.5 + cos(s) / 3)) % width;
                if (rnd() < 0.3) {
                    (*back)[(i + j*width)] |= color;
                }
            }
        }
    }

    // Update Game of Life
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int next = black;
            for (int i = 0; i < nColors; ++i) {
                int color = colors[i];
                int nAlive = countNeighbors(x, y, color);
                if (isAlive(x, y, color) && (nAlive >= 2 && nAlive <= 3)) {
                    next |= color;
                } else if (!isAlive(x, y, color) && (nAlive == 3)) {
                    next |= color;
                }
            }
            (*curr)[x + y*width] = next;
        }
    }

    updateImage(*curr);
}
