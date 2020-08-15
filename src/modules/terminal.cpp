/**IT_START**/

import "graphics" {
    func updateImage(buffer);
    func loadImage(string) -> buffer;
}
import "input" {
    func log(string, s32);
    func registerOnKeyDown(func(s32));
    func registerOnKeyUp(func(s32));
}
import "wasm" {
    // func loadModule(string, func(ref));
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

void onKeyDown(int key) {
    // if (key >= 'A' && key <= 'Z') {
    //     text += key + (!isShiftHeld * ('a' - 'A'));
    // } else if (key >= '0' && key <= '9') {
    //     if (!isShiftHeld) {
    //         text += key;
    //     } else {
    //         // symbols corresponding to 0-9 (with QWERTY I guess)
    //         const char* symbols = ")!@#$%^&*(";
    //         text += symbols[key-'0'];
    //     }
    // } else
    if (key >= ' ' && key <= '~') {
        // other keys should just be printable?
        text += key;
    } else {
        switch (key) {
        case 1: // shift
            isShiftHeld = true;
            break;
        case 2: // ctrl
            isCtrlHeld = true;
            break;
        case 5: // escape
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
    pixels->fill(black);
    drawText(Point(40, 40), "$>" + text + '|');
    updateImage(pixels);
}
