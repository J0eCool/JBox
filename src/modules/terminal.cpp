/**IT_START**/

type Image = struct {
    pixels: buffer;
    width: s32;
    height: s32;
}

import "graphics" {
    func updateImage(buffer);
    func loadImage(string, func(Image));
}
import "input" {
    func log(string, s32);
    func registerOnKeyDown(func(s32));
    func registerOnKeyUp(func(s32));
}
import "wasm" {
    func loadModule(string, func(any));
    func callInit(any, s32, s32);
    func callFrame(any);
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

std::string text = "";
bool isShiftHeld = false;
bool isCtrlHeld = false;

void* module = nullptr;
void onModuleLoad(void* mod) {
    module = mod;
    callInit(module, width, height);
}

void onFontLoad(Image* image) {
    font = new PixelBuffer(image->pixels, image->width, image->height);
    charSize = Point(image->width / 16, image->height / 16);
}

void onKeyDown(int key) {
    if (key >= ' ' && key <= '~') {
        // keys in this range are printable ASCII
        text += key;
    } else {
        switch (key) {
        case 1: // shift
            isShiftHeld = true;
            break;
        case 2: // ctrl
            isCtrlHeld = true;
            break;
        case 4:
            loadModule(text.c_str(), onModuleLoad);
            text.clear();
            break;
        case 5: // escape
            if (module) {
                module = nullptr;
            }
            text.clear();
            break;
        case 6: // backspace
            if (isCtrlHeld) {
                // if ctrl is held, delete whole word
                // delete until last char is ' ', then reuse standard backspace
                // logic to unconditionally delete that ' ' too
                while (text.size() > 0 && text[text.size()-1] != ' ') {
                    text.pop_back();
                }
            }
            if (text.size() > 0) {
                text.pop_back();
            }
            break;
        default:
            log("Unknown key down:", key);
            break;
        }
    }
}

void onKeyUp(int key) {
    switch (key) {
    case 1: // shift
        isShiftHeld = false;
        break;
    case 2: // ctrl
        isCtrlHeld = false;
        break;
    }
}

void init(int w, int h) {
    registerOnKeyDown(onKeyDown);
    registerOnKeyUp(onKeyUp);

    width = w;
    height = h;

    pixels = new PixelBuffer(w, h);

    loadImage("textures/font.png", onFontLoad);
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
    if (!font) {
        return;
    }
    for (int i = 0; i < text.size(); ++i) {
        drawChar(pos + Point(i * charSize.x, 0), text[i]);
    }
}

void frame() {
    if (module) {
        callFrame(module);
        return;
    }
    pixels->fill(black);
    drawText(Point(40, 40), "$>" + text + '|');
    updateImage(pixels);
}
