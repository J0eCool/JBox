/**IT_START**/

include "gl.h"

type Image = struct {
    pixels: buffer;
    width: s32;
    height: s32;
}

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

#include "mat.h"
#include "vec.h"

#include <string>
#include <vector>

Vec red(1.0, 0.0, 0.0);
Vec green(0.0, 1.0, 0.0);
Vec blue(0.0, 0.0, 1.0);
Vec yellow(1.0, 1.0, 0.0);
Vec cyan(0.0, 1.0, 1.0);
Vec magenta(1.0, 0.0, 1.0);

class WorldScene {
    void* program;
    int posLoc;
    void* matrixLoc;
    void* colorLoc;
    void* lightPosLoc;
    void* verts;

    struct Cube {
        Vec pos;
        Vec size;
        Vec color;
    };
    std::vector<Cube> cubes;
public:
    float dist = 5.0;
    float rot = 0.0;
    WorldScene() {
        const char* vertShader = R"(
            attribute vec4 aPos;

            uniform mat4 uMatrix;

            varying vec3 vPos;

            void main() {
                vec4 pos = uMatrix * aPos;
                gl_Position = pos;
                vPos = pos.xyz;
            }
        )";
        const char* fragShader = R"(
            precision mediump float;

            uniform vec3 uColor;
            uniform vec3 uLightPos;

            varying vec3 vPos;

            void main() {
                float range = 3.0;
                float dist = length(vPos - uLightPos);
                float lum = (range - dist) / range;
                float light = clamp(1.0 - lum*lum, 0.0, 1.0);
                gl_FragColor = vec4(uColor * light, 1);
            }
        )";

        program = loadProgram(vertShader, fragShader);
        posLoc = gl::getAttribLocation(program, "aPos");
        matrixLoc = gl::getUniformLocation(program, "uMatrix");
        colorLoc = gl::getUniformLocation(program, "uColor");
        lightPosLoc = gl::getUniformLocation(program, "uLightPos");

        verts = createVbo(cubeModel());

        cubes.push_back(Cube { Vec(0, 0.5, 0.75), Vec(1, 1, 1), Vec(1, 0, 0)});
        cubes.push_back(Cube { Vec(1.5, 0, 1), Vec(1, 1, 2), Vec(0, 1, 0)});
        cubes.push_back(Cube { Vec(-1.5, -0.5, 1), Vec(1, 1, 1), Vec(0, 1, 1)});
        cubes.push_back(Cube { Vec(0, 0, 0), Vec(5, 5, 0.5), Vec(0.7, 0.7, 0.7)});
    }

    void draw() {
        gl::enable(gl_DEPTH_TEST);
        gl::enable(gl_CULL_FACE);
        gl::clearColor(0.1, 0, 0, 1);
        gl::clear(gl_COLOR_BUFFER_BIT | gl_DEPTH_BUFFER_BIT);

        gl::useProgram(program);

        gl::enableVertexAttribArray(posLoc);

        gl::bindBuffer(gl_ARRAY_BUFFER, verts);
        gl::vertexAttribPointer(posLoc, 3, gl_FLOAT, false, 0, 0);

        // auto camera = Mat::rotateX(PI/2) * Mat::translate(0, -dist, 2);
        auto cameraPos = Vec::polar2d(rot, dist) + Vec(0, 0, 2);
        // auto camera = Mat::translate(cameraPos) * Mat::rotateX(PI / 2);// * Mat::rotateY(rot);
        auto camera = Mat::lookAt(cameraPos, Vec(0, 0, 1));
        auto view = camera.inverse();
        auto proj = Mat::perspective(70, 1.333, 0.1, 100.0);
        auto viewproj = proj * view;
        auto lightPos = viewproj * Vec(1, 1, 1);
        gl::uniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
        for (auto& cube : cubes) {
            auto mat = viewproj * Mat::translate(cube.pos) * Mat::scale(cube.size);
            gl::uniformMatrix4fv(matrixLoc, false, &mat);
            gl::uniform3f(colorLoc, cube.color.x, cube.color.y, cube.color.z);
            gl::drawArrays(gl_TRIANGLES, 0, 36);
        }
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
    Mat projection;
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

        projection = Mat::uiOrtho(800, 600);
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
        drawText(Vec(50, 50), strBuff);
        snprintf(strBuff, sizeof(strBuff), "rot  = %f", world.rot);
        drawText(Vec(50, 80), strBuff);
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

            auto cur = pos + Vec(18*i, 0);
            auto matrix = projection * Mat::translate(cur) * Mat::scale(18, 24);
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
