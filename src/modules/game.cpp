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

        auto mat = Mat4()
            * Mat4::uiOrtho(800, 600)
            * Mat4::translate(50, 50)
            * Mat4::scale(700, 900)
            ;
        gl::uniformMatrix4fv(matrixLoc, false, &mat);

        gl::bindBuffer(gl_ARRAY_BUFFER, verts);
        gl::vertexAttribPointer(posLoc, 2, gl_FLOAT, false, 0, 0);
        gl::bindBuffer(gl_ARRAY_BUFFER, texUvs);
        gl::vertexAttribPointer(texUvLoc, 2, gl_FLOAT, false, 0, 0);
        gl::bindTexture(gl_TEXTURE_2D, texId);
        gl::drawArrays(gl_TRIANGLES, 0, 12);
    }
} gui;

bool isHeld[256];
void onKeyDown(int key) {
    isHeld[(unsigned char)key] = true;
}

void onKeyUp(int key) {
    isHeld[(unsigned char)key] = false;
}

void init(int w, int h) {
    input::registerOnKeyDown(onKeyDown);
    input::registerOnKeyUp(onKeyUp);
}

void frame() {
    int dx = isHeld[(int)'d'] - isHeld[(int)'a'];
    int dy = isHeld[(int)'s'] - isHeld[(int)'w'];
    world.rot += dx * TAU * 0.7 / 60;
    world.dist += dy * 2.5 / 60;
    if (world.dist < 1.25) { world.dist = 1.25; }

    world.draw();
    gui.draw();
}
