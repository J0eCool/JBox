/**IT_START**/

import gl {
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

    func cullFace(s32);
    func enable(s32);
}

import input {
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

    // culling
    gl_CULL_FACE = 0x0B44,
    gl_FRONT = 0x0404,
    gl_BACK = 0x0405,
    gl_FRONT_AND_BACK = 0x0408,
};

#include <string>

void* loadShader(GlConstant ty, const char* text) {
    void* shader = gl::createShader(ty);
    gl::shaderSource(shader, text);
    gl::compileShader(shader);
    if (!gl::getShaderParameter(shader, gl_COMPILE_STATUS)) {
        auto msg = std::string("Shader failed to compile: ") + gl::getShaderInfoLog(shader);
        input::log(msg.c_str(), 0);
        shader = nullptr;
    }
    return shader;
}

void* loadProgram(const char* vertText, const char* fragText) {
    void* program = gl::createProgram();
    gl::attachShader(program, loadShader(gl_VERTEX_SHADER, vertText));
    gl::attachShader(program, loadShader(gl_FRAGMENT_SHADER, fragText));
    gl::linkProgram(program);
    if (!gl::getProgramParameter(program, gl_LINK_STATUS)) {
        auto msg = std::string("Program failed to link: ") + gl::getProgramInfoLog(program);
        input::log(msg.c_str(), 0);
        program = nullptr;
    }
    return program;
}
