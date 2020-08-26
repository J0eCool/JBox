/**IT_START**/

import "gl" {
    // shaders
    func createShader(u32) -> any;
    func shaderSource(any, string);
    func compileShader(any);
    func deleteShader(any);
    func getShaderParameter(any, u32) -> u1;
    func getShaderInfoLog(any) -> string;

    // shader programs
    func createProgram() -> any;
    func attachShader(any, any);
    func linkProgram(any);
    func getProgramParameter(any, u32) -> u1;
    func getProgramInfoLog(any) -> string;
    func getAttribLocation(any, string) -> s32;

    // vbos
    func createBuffer() -> any;
    func bindBuffer(u32, any);
    func bufferData(u32, buffer, u32);

    // clearing
    func clearColor(f32, f32, f32, f32);
    func clear(u32);

    func useProgram(any);
    func enableVertexAttribArray(s32);
    func vertexAttribPointer(s32, u32, u32, u1, u32, u32);

    func drawArrays(u32, u32, u32);
}

import "input" {
    func log(string, s32);
    func registerOnKeyDown(func(s32));
    func registerOnKeyUp(func(s32));
}
export {
    func init(s32, s32);
    func frame();
}

/**IT_END**/

enum GlConstant {
    // TODO: maybe get these programmatically in the future...
    gl_COLOR_BUFFER_BIT = 0x00004000,

    gl_FRAGMENT_SHADER = 0x8B30,
    gl_VERTEX_SHADER = 0x8B31,
    gl_COMPILE_STATUS = 0x8B81,
    gl_LINK_STATUS = 0x8B82,

    gl_STATIC_DRAW = 0x88E4,
    gl_ARRAY_BUFFER = 0x8892,

    // drawing types
    gl_POINTS = 0x0000,
    gl_LINES = 0x0001,
    gl_LINE_LOOP = 0x0002,
    gl_LINE_STRIP = 0x0003,
    gl_TRIANGLES = 0x0004,
    gl_TRIANGLE_STRIP = 0x0005,
    gl_TRIANGLE_FAN = 0x0006,

    // data types
    gl_BYTE = 0x1400,
    gl_UNSIGNED_BYTE = 0x1401,
    gl_SHORT = 0x1402,
    gl_UNSIGNED_SHORT = 0x1403,
    gl_INT = 0x1404,
    gl_UNSIGNED_INT = 0x1405,
    gl_FLOAT = 0x1406,
};

#include "common.h"

#include <string>

const char* vertShader = R"(
    attribute vec4 aPos;
    attribute vec3 aColor;

    varying vec3 vColor;

    void main() {
        gl_Position = aPos;
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

void* loadShader(GlConstant ty, const char* text) {
    void* shader = createShader(ty);
    shaderSource(shader, text);
    compileShader(shader);
    if (!getShaderParameter(shader, gl_COMPILE_STATUS)) {
        auto msg = std::string("Shader failed to compile: ") + getShaderInfoLog(shader);
        log(msg.c_str(), 0);
        shader = nullptr;
    }
    return shader;
}

void onKeyDown(int key) {
}

void onKeyUp(int key) {
}

struct GLState {
    void* program;
    int posLoc;
    int colorLoc;
    void* verts;
    void* colors;
} state;

void init(int w, int h) {
    registerOnKeyDown(onKeyDown);
    registerOnKeyUp(onKeyUp);

    void* program = createProgram();
    attachShader(program, loadShader(gl_VERTEX_SHADER, vertShader));
    attachShader(program, loadShader(gl_FRAGMENT_SHADER, fragShader));
    linkProgram(program);
    if (!getProgramParameter(program, gl_LINK_STATUS)) {
        auto msg = std::string("Program failed to link: ") + getProgramInfoLog(program);
        log(msg.c_str(), 0);
        program = nullptr;
    }
    state.program = program;
    state.posLoc = getAttribLocation(program, "aPos");
    state.colorLoc = getAttribLocation(program, "aColor");

    log("PosLoc: ", state.posLoc);
    log("colorLoc: ", state.colorLoc);

    float rawVertData[] = {
        0.0, 0.0,
        0.5, 0.7,
        1.0, 0.0,
    };
    ITBuffer vertData(6*sizeof(float), rawVertData);
    void* verts = createBuffer();
    bindBuffer(gl_ARRAY_BUFFER, verts);
    bufferData(gl_ARRAY_BUFFER, &vertData, gl_STATIC_DRAW);
    state.verts = verts;

    float rawColorData[] = {
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, 1.0,
    };
    ITBuffer colorData(9*sizeof(float), rawColorData);
    void* colors = createBuffer();
    bindBuffer(gl_ARRAY_BUFFER, colors);
    bufferData(gl_ARRAY_BUFFER, &colorData, gl_STATIC_DRAW);
    state.colors = colors;
}

void frame() {
    clearColor(0.1, 0, 0, 1);
    clear(gl_COLOR_BUFFER_BIT);

    useProgram(state.program);
    enableVertexAttribArray(state.posLoc);
    bindBuffer(gl_ARRAY_BUFFER, state.verts);
    vertexAttribPointer(state.posLoc, 2, gl_FLOAT, false, 0, 0);
    enableVertexAttribArray(state.colorLoc);
    bindBuffer(gl_ARRAY_BUFFER, state.colors);
    vertexAttribPointer(state.colorLoc, 3, gl_FLOAT, false, 0, 0);

    drawArrays(gl_TRIANGLES, 0, 3);
}
