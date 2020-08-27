/**IT_START**/

include "gl.h"

import input {
    func log(string, s32);
    func registerOnKeyDown(func(s32));
    func registerOnKeyUp(func(s32));
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

class Program {
    void* program;
    int posLoc;
    int colorLoc;
    void* matrixLoc;
    void* verts;
    void* colors;
public:
    float dist = 3.0;
    float rot = 0.0;
    Program() {
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

        verts = gl::createBuffer();
        gl::bindBuffer(gl_ARRAY_BUFFER, verts);
        auto cube = cubeModel();
        gl::bufferData(gl_ARRAY_BUFFER, &cube, gl_STATIC_DRAW);

        Vec3 red(1.0, 0.0, 0.0);
        Vec3 green(0.0, 1.0, 0.0);
        Vec3 blue(0.0, 0.0, 1.0);
        Vec3 yellow(1.0, 1.0, 0.0);
        Vec3 cyan(0.0, 1.0, 1.0);
        Vec3 magenta(1.0, 0.0, 1.0);
        Buffer<Vec3> colorData(36, (Vec3[36]){
            red, red, red, red, red, red,
            green, green, green, green, green, green,
            blue, blue, blue, blue, blue, blue,
            yellow, yellow, yellow, yellow, yellow, yellow,
            cyan, cyan, cyan, cyan, cyan, cyan,
            magenta, magenta, magenta, magenta, magenta, magenta,
        });
        colors = gl::createBuffer();
        gl::bindBuffer(gl_ARRAY_BUFFER, colors);
        gl::bufferData(gl_ARRAY_BUFFER, &colorData, gl_STATIC_DRAW);
    }

    void frame() {
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

        gl::enableVertexAttribArray(posLoc);
        gl::bindBuffer(gl_ARRAY_BUFFER, verts);
        gl::vertexAttribPointer(posLoc, 3, gl_FLOAT, false, 0, 0);
        gl::enableVertexAttribArray(colorLoc);
        gl::bindBuffer(gl_ARRAY_BUFFER, colors);
        gl::vertexAttribPointer(colorLoc, 3, gl_FLOAT, false, 0, 0);
        gl::drawArrays(gl_TRIANGLES, 0, 36);
    }

    Buffer<Vec3> cubeModel() {
        float h = 0.5; // cube size; using 0.5 for centered unit cube
        // ABCD on bottom, EFGH on top
        Vec3 A(-h, -h, -h);
        Vec3 B(-h, +h, -h);
        Vec3 C(+h, +h, -h);
        Vec3 D(+h, -h, -h);
        Vec3 E(-h, -h, +h);
        Vec3 F(-h, +h, +h);
        Vec3 G(+h, +h, +h);
        Vec3 H(+h, -h, +h);

        // 3 verts per tri, 2 tris per face, 6 faces
        const int nTris = 3 * 2 * 6;
        return Buffer<Vec3>(nTris, (Vec3[nTris]){
            A, B, C, C, D, A, // bottom
            A, E, F, F, B, A, // left
            A, D, H, H, E, A, // front
            D, C, G, G, H, D, // right
            B, F, G, G, C, B, // back
            E, H, G, G, F, E, // top
        });
    }
} program;

bool isHeld[256];
void onKeyDown(int key) {
    isHeld[key] = true;
}

void onKeyUp(int key) {
    isHeld[key] = false;
}

void init(int w, int h) {
    input::registerOnKeyDown(onKeyDown);
    input::registerOnKeyUp(onKeyUp);
}

void frame() {
    int dx = isHeld[(int)'d'] - isHeld[(int)'a'];
    int dy = isHeld[(int)'s'] - isHeld[(int)'w'];
    program.rot += dx * TAU * 0.7 / 60;
    program.dist += dy * 2.5 / 60;
    if (program.dist < 1.25) { program.dist = 1.25; }
    program.frame();
}
