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

#include <string>

class Program {
    void* program;
    int posLoc;
    int colorLoc;
    void* matrixLoc;
    void* verts;
    void* colors;
public:
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

        Buffer<float> vertData(18, (float[]){
            -0.5, -0.5, 0.0,
            0.5, -0.5, 0.0,
            0.5, 0.5, 0.0,

            0.5, 0.5, 0.0,
            -0.5, 0.5, 0.0,
            -0.5, -0.5, 0.0,
        });
        verts = gl::createBuffer();
        gl::bindBuffer(gl_ARRAY_BUFFER, verts);
        gl::bufferData(gl_ARRAY_BUFFER, &vertData, gl_STATIC_DRAW);

        Buffer<float> colorData(18, (float[]){
            1.0, 0.0, 0.0,
            0.0, 1.0, 0.0,
            0.0, 0.0, 1.0,
            1.0, 0.0, 0.0,
            0.0, 1.0, 0.0,
            0.0, 0.0, 1.0,
        });
        colors = gl::createBuffer();
        gl::bindBuffer(gl_ARRAY_BUFFER, colors);
        gl::bufferData(gl_ARRAY_BUFFER, &colorData, gl_STATIC_DRAW);
    }

    void frame(float x, float y) {
        gl::clearColor(0.1, 0, 0, 1);
        gl::clear(gl_COLOR_BUFFER_BIT);

        gl::useProgram(program);
        gl::enableVertexAttribArray(posLoc);
        gl::bindBuffer(gl_ARRAY_BUFFER, verts);
        gl::vertexAttribPointer(posLoc, 3, gl_FLOAT, false, 0, 0);
        gl::enableVertexAttribArray(colorLoc);
        gl::bindBuffer(gl_ARRAY_BUFFER, colors);
        gl::vertexAttribPointer(colorLoc, 3, gl_FLOAT, false, 0, 0);

        auto mat = Mat4::scale(0.5) * Mat4::translate(x, y, 0);
        gl::uniformMatrix4fv(matrixLoc, false, mat.toITBuffer());

        gl::drawArrays(gl_TRIANGLES, 0, 6);
    }
} program;

void onKeyDown(int key) {
}

void onKeyUp(int key) {
}

void init(int w, int h) {
    input::registerOnKeyDown(onKeyDown);
    input::registerOnKeyUp(onKeyUp);
}

int t = 0;
void frame() {
    float x = cos(t * TAU / 178);
    float y = sin(t * TAU / 78);
    program.frame(x, y);
    t++;
}
