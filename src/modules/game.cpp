/**IT_START**/

include "files.h"
include "gl.h"

type MouseEvent = struct {
    button: s32;
    x: f32;
    y: f32;
}
import input {
    func log(string, s32);
    func registerOnKeyDown(func(s32));
    func registerOnKeyUp(func(s32));
    func registerOnMouseDown(func(MouseEvent));
    func registerOnMouseUp(func(MouseEvent));
    func registerOnMouseMove(func(MouseEvent));
}

import math {
    func rand() -> f32;
}
import time {
    func now() -> f32;
}
export {
    func init(s32, s32);
    func frame();
}

/**IT_END**/

#include "mat.h"
#include "vec.h"

#include <string>
#include <vector>

float randBool(float p) {
    return math::rand() < p;
}

FileLoader loader;

class UiScene {
    void* program;
    int posLoc;
    int texUvLoc;
    void* matrixLoc;
    void* verts;
    void* texUvs;
    static void* texId;
    Mat projection;

    const std::string fragUrl = "shaders/simple2d.frag";
    const std::string vertUrl = "shaders/simple2d.vert";
public:
    UiScene() {
        loader.beginFetch(fragUrl);
        loader.beginFetch(vertUrl);
    }

    void onFilesLoaded() {
        auto vertText = loader.read(vertUrl);
        auto fragText = loader.read(fragUrl);
        program = loadProgram(vertText.c_str(), fragText.c_str());
        posLoc = gl::getAttribLocation(program, "aPos");
        texUvLoc = gl::getAttribLocation(program, "aTexUv");
        matrixLoc = gl::getUniformLocation(program, "uMatrix");

        verts = createVbo(Buffer<float>(12, (float[12]){
            0.0, 0.0,
            0.0, 1.0,
            1.0, 0.0,
            1.0, 0.0,
            0.0, 1.0,
            1.0, 1.0,
        }));
        // texUvs get overridden, but we need to initialize them in the first place
        texUvs = createVbo(Buffer<float>(12, (float[12]){
            0.0, 0.0,
            0.0, 1.0,
            1.0, 0.0,
            1.0, 0.0,
            0.0, 1.0,
            1.0, 1.0,
        }));

        gl::enableVertexAttribArray(posLoc);
        gl::enableVertexAttribArray(texUvLoc);

        texId = gl::createTexture();
        files::loadImage("textures/font.png", UiScene::onFontLoad);

        projection = Mat::uiOrtho(800, 600);
    }

    static void onFontLoad(ImageResponse* image) {
        gl::bindTexture(gl_TEXTURE_2D, texId);
        gl::texParameteri(gl_TEXTURE_2D, gl_TEXTURE_WRAP_S, gl_CLAMP_TO_EDGE);
        gl::texParameteri(gl_TEXTURE_2D, gl_TEXTURE_WRAP_T, gl_CLAMP_TO_EDGE);
        gl::texParameteri(gl_TEXTURE_2D, gl_TEXTURE_MIN_FILTER, gl_NEAREST);
        gl::texParameteri(gl_TEXTURE_2D, gl_TEXTURE_MAG_FILTER, gl_NEAREST);
        gl::texImage2D(gl_TEXTURE_2D, 0, gl_RGBA, image->width, image->height, 0,
            gl_RGBA, gl_UNSIGNED_BYTE, (Buffer<unsigned char>*)image->pixels);
    }

    void draw() {
        gl::enable(gl_DEPTH_TEST);
        gl::enable(gl_BLEND);
        gl::blendFunc(gl_SRC_ALPHA, gl_ONE_MINUS_SRC_ALPHA);
        gl::clear(gl_DEPTH_BUFFER_BIT);

        gl::useProgram(program);

        char strBuff[128];
        float yPos = 50.0f;
        auto line = [&](const char* label, int val) {
            snprintf(strBuff, sizeof(strBuff), "%s: %d", label, val);
            drawText(Vec(50, yPos), strBuff);
            yPos += 22;
        };

        line("initProb", 100*world.initProb);
        line("deadLo  ", world.deadLo);
        line("deadHi  ", world.deadHi);
        line("liveLo  ", world.liveLo);
        line("liveHi  ", world.liveHi);
    }

    void drawText(Vec pos, std::string str) {
        gl::bindTexture(gl_TEXTURE_2D, texId);
        gl::bindBuffer(gl_ARRAY_BUFFER, verts);
        gl::vertexAttribPointer(posLoc, 2, gl_FLOAT, false, 0, 0);
        float w = 1.0 / 16.0;
        for (int i = 0; i < str.size(); ++i) {
            char c = str[i];
            float x = (c % 16) * w;
            float y = (c / 16) * w;
            float u0 = x;
            float u1 = u0 + w;
            float v0 = y;
            float v1 = v0 + w;
            auto uvs = Buffer<float>(12, (float[12]){
                u0, v0,
                u0, v1,
                u1, v0,
                u1, v0,
                u0, v1,
                u1, v1,
            });
            gl::bindBuffer(gl_ARRAY_BUFFER, texUvs);
            gl::bufferSubData(gl_ARRAY_BUFFER, 0, &uvs);
            gl::vertexAttribPointer(texUvLoc, 2, gl_FLOAT, false, 0, 0);

            auto cur = pos + Vec(12*i, 0);
            auto matrix = projection * Mat::translate(cur) * Mat::scale(12, 18);
            gl::uniformMatrix4fv(matrixLoc, false, &matrix);
            gl::drawArrays(gl_TRIANGLES, 0, 12);
        }
    }
} gui;

bool isHeld[256];
void onKeyDown(int key) {
    isHeld[(unsigned char)key] = true;
}

void onKeyUp(int key) {
    isHeld[(unsigned char)key] = false;
}

bool isMouseDown = false;
Vec mousePos;
void onMouseDown(MouseEvent* event) {
    isMouseDown = true;
    mousePos = Vec(event->x, event->y);
}
void onMouseUp(MouseEvent* event) {
    isMouseDown = false;
}
void onMouseMove(MouseEvent* event) {
    if (!isMouseDown) {
        return;
    }

    auto pos = Vec(event->x, event->y);
    auto delta = mousePos - pos;
    mousePos = pos;
    world.rot -= delta.x * TAU;
    world.dist += delta.y * 2.5;
}

float lastFrame;
void init(int w, int h) {
    input::registerOnKeyDown(onKeyDown);
    input::registerOnKeyUp(onKeyUp);

    input::registerOnMouseDown(onMouseDown);
    input::registerOnMouseUp(onMouseUp);
    input::registerOnMouseMove(onMouseMove);

    lastFrame = time::now();
}

int axis(char lo, char hi) {
    return isHeld[(int)hi] - isHeld[(int)lo];
}
bool isDown(char c) {
    auto ret = isHeld[(int)c];
    isHeld[(int)c] = false;
    return ret;
}
int isDownAxis(char lo, char hi) {
    return isDown(hi) - isDown(lo);
}

bool didLoadFiles = false;
void frame() {
    if (!didLoadFiles) {
        if (loader.anyPending()) {
            return;
        }
        gui.onFilesLoaded();
        didLoadFiles = true;
    }

    auto t = time::now();
    auto dt = t - lastFrame;
    lastFrame = t;

    int dx = axis('a', 'd');
    int dy = axis('w', 's');

    gui.draw();
}
