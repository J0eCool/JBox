/**IT_START**/

include "gl.h"

type Image = struct {
    pixels: buffer;
    width: s32;
    height: s32;
}

import input {
    func log(string, s32);
    func registerOnKeyDown(func(s32));
    func registerOnKeyUp(func(s32));
}
import imageDrawing {
    func loadImage(string, func(Image));
}
import time {
    func now() -> f32;
}
export {
    func init(s32, s32);
    func frame();
}

/**IT_END**/

#include "common.h"
#include "mat4.h"
#include "vec3.h"

#include <string>

class WorldScene {
    void* program;
    int posLoc;
    int colorLoc;
    void* matrixLoc;
    void* verts;
    void* colors;
public:
    float dist = 3.0;
    float rot = 0.0;
    WorldScene() {
        const char* vertShader = R"(
            attribute vec4 aPos;
            attribute vec3 aColor;

            uniform mat4 uMatrix;

            varying vec3 vColor;

            void main() {
                gl_Position = uMatrix * aPos;
                vColor = aColor;
            }
        )";
        const char* fragShader = R"(
            precision mediump float;

            varying vec3 vColor;

            void main() {
                gl_FragColor = vec4(vColor, 1);
            }
        )";

        program = loadProgram(vertShader, fragShader);
        posLoc = gl::getAttribLocation(program, "aPos");
        colorLoc = gl::getAttribLocation(program, "aColor");
        matrixLoc = gl::getUniformLocation(program, "uMatrix");

        verts = createVbo(cubeModel());

        Vec3 red(1.0, 0.0, 0.0);
        Vec3 green(0.0, 1.0, 0.0);
        Vec3 blue(0.0, 0.0, 1.0);
        Vec3 yellow(1.0, 1.0, 0.0);
        Vec3 cyan(0.0, 1.0, 1.0);
        Vec3 magenta(1.0, 0.0, 1.0);
        colors = createVbo(Buffer<Vec3> (36, (Vec3[36]){
            red, red, red, red, red, red,
            green, green, green, green, green, green,
            blue, blue, blue, blue, blue, blue,
            yellow, yellow, yellow, yellow, yellow, yellow,
            cyan, cyan, cyan, cyan, cyan, cyan,
            magenta, magenta, magenta, magenta, magenta, magenta,
        }));

        gl::enableVertexAttribArray(posLoc);
        gl::enableVertexAttribArray(colorLoc);
    }

    void draw() {
        gl::enable(gl_DEPTH_TEST);
        gl::enable(gl_CULL_FACE);
        gl::clearColor(0.1, 0, 0, 1);
        gl::clear(gl_COLOR_BUFFER_BIT | gl_DEPTH_BUFFER_BIT);

        gl::useProgram(program);

        auto mat = Mat4()
            * Mat4::perspective(90, 1.333, 0.1, 100.0)
            * Mat4::translate(0, 0, -dist)
            * Mat4::rotateY(rot)
            ;
        gl::uniformMatrix4fv(matrixLoc, false, &mat);

        gl::bindBuffer(gl_ARRAY_BUFFER, verts);
        gl::vertexAttribPointer(posLoc, 3, gl_FLOAT, false, 0, 0);
        gl::bindBuffer(gl_ARRAY_BUFFER, colors);
        gl::vertexAttribPointer(colorLoc, 3, gl_FLOAT, false, 0, 0);
        gl::drawArrays(gl_TRIANGLES, 0, 36);

        mat = mat * Mat4::translate(1.1, 0);
        gl::uniformMatrix4fv(matrixLoc, false, &mat);
        gl::drawArrays(gl_TRIANGLES, 0, 36);
    }
} world;

class UiScene {
    void* program;
    int posLoc;
    int texUvLoc;
    void* matrixLoc;
    void* verts;
    void* texUvs;
    static void* texId;
    Mat4 projection;
public:
    float dist = 3.0;
    float rot = 0.0;
    UiScene() {
        const char* vertShader = R"(
            attribute vec4 aPos;
            attribute vec2 aTexUv;

            uniform mat4 uMatrix;

            varying vec2 vTexUv;

            void main() {
                gl_Position = uMatrix * aPos;
                vTexUv = aTexUv;
            }
        )";
        const char* fragShader = R"(
            precision mediump float;

            uniform sampler2D uImage;

            varying vec2 vTexUv;

            void main() {
                gl_FragColor = texture2D(uImage, vTexUv);
            }
        )";

        program = loadProgram(vertShader, fragShader);
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
        imageDrawing::loadImage("textures/font.png", UiScene::onFontLoad);

        projection = Mat4::uiOrtho(800, 600);
    }

    static void onFontLoad(Image* image) {
        gl::bindTexture(gl_TEXTURE_2D, texId);
        gl::texParameteri(gl_TEXTURE_2D, gl_TEXTURE_WRAP_S, gl_CLAMP_TO_EDGE);
        gl::texParameteri(gl_TEXTURE_2D, gl_TEXTURE_WRAP_T, gl_CLAMP_TO_EDGE);
        gl::texParameteri(gl_TEXTURE_2D, gl_TEXTURE_MIN_FILTER, gl_NEAREST);
        gl::texParameteri(gl_TEXTURE_2D, gl_TEXTURE_MAG_FILTER, gl_NEAREST);
        gl::texImage2D(gl_TEXTURE_2D, 0, gl_RGBA, image->width, image->height, 0,
            gl_RGBA, gl_UNSIGNED_BYTE, image->pixels);
    }

    void draw() {
        gl::enable(gl_DEPTH_TEST);
        gl::enable(gl_BLEND);
        gl::blendFunc(gl_SRC_ALPHA, gl_ONE_MINUS_SRC_ALPHA);
        gl::clear(gl_DEPTH_BUFFER_BIT);

        gl::useProgram(program);

        char strBuff[128];
        snprintf(strBuff, sizeof(strBuff), "dist = %f", world.dist);
        drawText(Vec3(50, 50), strBuff);
        snprintf(strBuff, sizeof(strBuff), "rot  = %f", world.rot);
        drawText(Vec3(50, 80), strBuff);
    }

    void drawText(Vec3 pos, std::string str) {
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

            auto cur = pos + Vec3(18*i, 0);
            auto matrix = projection * Mat4::translate(cur) * Mat4::scale(18, 24);
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

float lastFrame;
void init(int w, int h) {
    input::registerOnKeyDown(onKeyDown);
    input::registerOnKeyUp(onKeyUp);
    lastFrame = time::now();
}

void frame() {
    auto t = time::now();
    auto dt = t - lastFrame;
    lastFrame = t;
    int dx = isHeld[(int)'d'] - isHeld[(int)'a'];
    int dy = isHeld[(int)'s'] - isHeld[(int)'w'];
    world.rot += dx * TAU * 0.3 * dt;
    world.dist += dy * 2.5 * dt;
    if (world.dist < 1.25) { world.dist = 1.25; }

    world.draw();
    gui.draw();
}
