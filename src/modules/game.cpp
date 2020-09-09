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

class WorldScene {
    void* program;
    int posLoc;
    int normalLoc;
    void* modelViewProjLoc;
    void* modelLoc;
    void* colorLoc;
    void* lightPosLoc;
    void* verts;
    void* normals;

    const std::string fragUrl = "shaders/simple3d.frag";
    const std::string vertUrl = "shaders/simple3d.vert";

    static const int gridSize = 24;
    static const int numGridElems = gridSize * gridSize * gridSize;
    bool grid[numGridElems];
public:
    float dist = 36;
    float rot = 0.0;
    WorldScene() {
        loader.beginFetch(fragUrl);
        loader.beginFetch(vertUrl);

        initializeGrid();
    }

    void onFilesLoaded() {
        auto vertText = loader.read(vertUrl);
        auto fragText = loader.read(fragUrl);
        program = loadProgram(vertText.c_str(), fragText.c_str());
        posLoc = gl::getAttribLocation(program, "aPos");
        normalLoc = gl::getAttribLocation(program, "aNormal");
        modelViewProjLoc = gl::getUniformLocation(program, "uModelViewProj");
        modelLoc = gl::getUniformLocation(program, "uModel");
        colorLoc = gl::getUniformLocation(program, "uColor");
        lightPosLoc = gl::getUniformLocation(program, "uLightPos");

        auto cube = cubeModel();
        auto norm = computeNormals(cube);
        verts = createVbo(cube);
        normals = createVbo(norm);
    }

    float initProb = 0.34f;
    int deadLo = 13;
    int deadHi = 24;
    int liveLo = 9;
    int liveHi = 21;
    void initializeGrid() {
        for (int i = 0; i < numGridElems; ++i) {
            grid[i] = randBool(initProb);
        }
    }

    bool nextGridStep(int idx) {
        int nNeighbors = 0;
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dz = -1; dz <= 1; ++dz) {
                    int i = idx + dx + gridSize * (dy + gridSize * dz);
                    i = (i + numGridElems) % numGridElems;
                    nNeighbors += grid[i];
                }
            }
        }
        bool alive = grid[idx];
        // if (!alive) {
        //     return randBool((nNeighbors + 1) / 270.0f);
        // }
        if (!alive) {
            return nNeighbors >= deadLo && nNeighbors <= deadHi;
        } else {
            return nNeighbors >= liveLo && nNeighbors <= liveHi;
        }
    }

    void frame() {
        bool nextGrid[numGridElems];
        for (int i = 0; i < numGridElems; ++i) {
            nextGrid[i] = nextGridStep(i);
        }
        memcpy(grid, nextGrid, numGridElems);
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
        gl::bindBuffer(gl_ARRAY_BUFFER, normals);
        gl::vertexAttribPointer(normalLoc, 3, gl_FLOAT, false, 0, 0);

        // auto camera = Mat::rotateX(PI/2) * Mat::translate(0, -dist, 2);
        auto cameraPos = Vec::polar2d(-rot, dist) + Vec(0, 0, 2);
        // auto camera = Mat::translate(cameraPos) * Mat::rotateX(PI / 2);// * Mat::rotateY(rot);
        auto camera = Mat::lookAt(cameraPos, Vec(0, 0, 1));
        auto view = camera.inverse();
        auto proj = Mat::perspective(70, 1.333, 0.1, 100.0);
        auto viewProj = proj * view;
        auto lightPos = cameraPos + Vec(0, 0, 1);
        gl::uniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);

        for (int i = 0; i < numGridElems; ++i) {
            if (!grid[i]) {
                continue;
            }
            int x = i % gridSize;
            int y = (i / gridSize) % gridSize;
            int z = i / gridSize / gridSize;
            auto h = gridSize / 2.0f;
            auto pos = Vec(x - h, y - h, z - h);
            auto model = Mat::translate(pos);
            auto mvp = viewProj * model;
            gl::uniformMatrix4fv(modelViewProjLoc, false, &mvp);
            gl::uniformMatrix4fv(modelLoc, false, &model);
            gl::uniform3f(colorLoc, 1.0f, 0.0f, 0.0f);
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
            gl_RGBA, gl_UNSIGNED_BYTE, image->pixels);
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
        world.onFilesLoaded();
        gui.onFilesLoaded();
        didLoadFiles = true;
    }

    auto t = time::now();
    auto dt = t - lastFrame;
    lastFrame = t;
    if (isDown('r')) {
        world.initializeGrid();
    }
    world.initProb += 0.01 * isDownAxis('n', 'm');
    world.deadLo += isDownAxis('y', 'h');
    world.deadHi += isDownAxis('u', 'j');
    world.liveLo += isDownAxis('i', 'k');
    world.liveHi += isDownAxis('o', 'l');

    int dx = axis('a', 'd');
    int dy = axis('w', 's');
    world.rot += dx * TAU * 0.3 * dt;
    world.dist += dy * 2.5 * dt;
    if (world.dist < 1.25) { world.dist = 1.25; }

    static int nFrame = 0;
    nFrame++;
    if (nFrame % 5 == 0) {
        world.frame();
    }

    world.draw();
    gui.draw();
}
