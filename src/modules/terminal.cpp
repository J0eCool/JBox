/**IT_START**/

import "graphics" {
    func updateImage(buffer);
    func loadImage(string) -> buffer;
}
export {
    func init(s32, s32);
    func frame();
}

/**IT_END**/

#include "common.h"

int width = 800;
int height = 600;

PixelBuffer* pixels;
PixelBuffer* font;

void init(int w, int h) {
    width = w;
    height = h;

    pixels = new PixelBuffer(w, h);

    // Init colors
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            pixels->ref(x, y) = black;
        }
    }

    // TODO: fix for hardcoded 96x128 resolution
    auto buff = loadImage("textures/font.png");
    font = new PixelBuffer(buff, 96, 128);
    for (int y = 0; y < font->height(); ++y) {
        for (int x = 0; x < font->width(); ++x) {
            pixels->ref(x, y) = font->ref(x, y) | black;
        }
    }
}

void frame() {
    updateImage(pixels);
}
