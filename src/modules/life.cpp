/**IT_START**/

import "graphics" {
    func updateImage(buffer);
}
import "math" {
    func rand() -> f32;
}
export {
    func init(s32, s32);
    func frame();
}

/**IT_END**/

int rgb(int r, int g, int b) {
    int a = 0xff;
    return ((0xff & a) << 0x18)
        | ((0xff & r) << 0x00)
        | ((0xff & g) << 0x08)
        | ((0xff & b) << 0x10)
        ;
}

int width = 800;
int height = 600;

Buffer<int> pixels(0);

int white = rgb(0xcf, 0xaf, 0xff);
int black = rgb(0, 0, 0);

void init(int w, int h) {
    width = w;
    height = h;
    pixels = Buffer<int>(w * h);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            pixels[x + y*width] = (rand() < 0.15) ? white : black;
        }
    }
}

inline int isAlive(int x, int y) {
    return pixels[x + y*width] == white;
}

int countNeighbors(int x, int y) {
    int nAlive = 0;

    // // cooler way to do it for cool kids
    // int deltas[8] = { -1, 1, -width, width, -1-width, -1+width, 1-width, 1+width };
    // int p = x + y*width;
    // for (int i = 0; i < 8; ++i) {
    //     nAlive += (pixels[(p + deltas[i]) % (width*height)] == white);
    // }

    for (int dx = -1; dx <= 1; ++dx) {
        int i = (x + dx + width) % width;
        for (int dy = -1; dy <= 1; ++dy) {
            int j = (y + dy + height) % height;
            if (isAlive(i, j) && (dx != 0 || dy != 0)) {
                nAlive++;
            }
        }
    }
    return nAlive;
}

int t = 0;
void frame() {
    t++;
    if (t % 3) return;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int nAlive = countNeighbors(x, y);
            int next = black;
            if (isAlive(x, y) && (nAlive == 2 || nAlive == 3)) {
                next = white;
            } else if (!isAlive(x, y) && nAlive == 3) {
                next = white;
            }
            pixels[x + y*width] = next;
        }
    }

    updateImage(pixels);
}
