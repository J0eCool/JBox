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
    func getUniformLocation(any, string) -> any;

    func uniform2f(any, f32, f32);
    func uniformMatrix4fv(any, u1, f32buffer);

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

#include <string>

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

void* loadProgram(const char* vertText, const char* fragText) {
    void* program = createProgram();
    attachShader(program, loadShader(gl_VERTEX_SHADER, vertText));
    attachShader(program, loadShader(gl_FRAGMENT_SHADER, fragText));
    linkProgram(program);
    if (!getProgramParameter(program, gl_LINK_STATUS)) {
        auto msg = std::string("Program failed to link: ") + getProgramInfoLog(program);
        log(msg.c_str(), 0);
        program = nullptr;
    }
    return program;
}