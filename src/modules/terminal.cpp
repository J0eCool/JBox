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

#include <string>

int width = 800;
int height = 600;

PixelBuffer* pixels;
PixelBuffer* font;
Point charSize;

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
    charSize = Point(font->width() / 16, font->height() / 16);
}

void drawChar(Point pos, char c) {
    auto fPos = Point(c % 16, c / 16) * charSize;
    for (int y = 0; y < charSize.y; ++y) {
        for (int x = 0; x < charSize.x; ++x) {
            Point off(x, y);
            (*pixels)[pos + off] = (*font)[fPos + off] | black;
        }
    }
}
void drawText(Point pos, std::string const& text) {
    for (int i = 0; i < text.size(); ++i) {
        drawChar(pos + Point(i * charSize.x, 0), text[i]);
    }
}

void frame() {
    drawText(Point(40, 40), "Hello world!");
    drawText(Point(120, 240), "$-#~: We \"can\" do <(thing's)> {now};");
    updateImage(pixels);
}
