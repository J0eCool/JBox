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
    func uniformMatrix4fv(any, u1, array(f32));

    // vbos
    func createBuffer() -> any;
    func bindBuffer(u32, any);
    func bufferData(u32, buffer, u32);
    func bufferSubData(u32, u32, buffer);

    // textures
    func createTexture() -> any;
    func bindTexture(s32, any);
    func texParameteri(s32, s32, s32);
    func texImage2D(s32, u32, s32, u32, u32, u32, s32, s32, array(u8));

    // clearing
    func clearColor(f32, f32, f32, f32);
    func clear(u32);

    func useProgram(any);
    func enableVertexAttribArray(s32);
    func vertexAttribPointer(s32, u32, u32, u1, u32, u32);

    func drawArrays(u32, u32, u32);

    func cullFace(s32);
    func enable(s32);
    func blendFunc(s32, s32);
}

import input {
    func log(string, s32);
}

/**IT_END**/

enum GlConstant {
    // TODO: maybe get these programmatically in the future...
    // source: https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API/Constants

    // clearing buffers
    gl_DEPTH_BUFFER_BIT = 0x00000100,
    gl_STENCIL_BUFFER_BIT = 0x00000400,
    gl_COLOR_BUFFER_BIT = 0x00004000,

    // rendering primitives
    gl_POINTS = 0x0000,
    gl_LINES = 0x0001,
    gl_LINE_LOOP = 0x0002,
    gl_LINE_STRIP = 0x0003,
    gl_TRIANGLES = 0x0004,
    gl_TRIANGLE_STRIP = 0x0005,
    gl_TRIANGLE_FAN = 0x0006,

    // blending modes
    gl_ZERO = 0,
    gl_ONE = 1,
    gl_SRC_COLOR = 0x0300,
    gl_ONE_MINUS_SRC_COLOR = 0x0301,
    gl_SRC_ALPHA = 0x0302,
    gl_ONE_MINUS_SRC_ALPHA = 0x0303,
    gl_DST_ALPHA = 0x0304,
    gl_ONE_MINUS_DST_ALPHA = 0x0305,
    gl_DST_COLOR = 0x0306,
    gl_ONE_MINUS_DST_COLOR = 0x0307,
    gl_SRC_ALPHA_SATURATE = 0x0308,
    gl_CONSTANT_COLOR = 0x8001,
    gl_ONE_MINUS_CONSTANT_COLOR = 0x8002,
    gl_CONSTANT_ALPHA = 0x8003,
    gl_ONE_MINUS_CONSTANT_ALPHA = 0x8004,

    // blending equations
    gl_FUNC_ADD = 0x8006,
    gl_FUNC_SUBTRACT = 0x800A,
    gl_FUNC_REVERSE_SUBTRACT = 0x800B,

    gl_FRAGMENT_SHADER = 0x8B30,
    gl_VERTEX_SHADER = 0x8B31,
    gl_COMPILE_STATUS = 0x8B81,
    gl_LINK_STATUS = 0x8B82,

    // buffers
    gl_STATIC_DRAW = 0x88E4,
    gl_STREAM_DRAW = 0x88E0,
    gl_DYNAMIC_DRAW = 0x88E8,
    gl_ARRAY_BUFFER = 0x8892,
    gl_ELEMENT_ARRAY_BUFFER = 0x8893,
    gl_BUFFER_SIZE = 0x8764,
    gl_BUFFER_USAGE = 0x8765,

    // data types
    gl_BYTE = 0x1400,
    gl_UNSIGNED_BYTE = 0x1401,
    gl_SHORT = 0x1402,
    gl_UNSIGNED_SHORT = 0x1403,
    gl_INT = 0x1404,
    gl_UNSIGNED_INT = 0x1405,
    gl_FLOAT = 0x1406,

    // pixel formats
    gl_DEPTH_COMPONENT = 0x1902,
    gl_ALPHA = 0x1906,
    gl_RGB = 0x1907,
    gl_RGBA = 0x1908,
    gl_LUMINANCE = 0x1909,
    gl_LUMINANCE_ALPHA = 0x190A,

    // culling
    gl_CULL_FACE = 0x0B44,
    gl_FRONT = 0x0404,
    gl_BACK = 0x0405,
    gl_FRONT_AND_BACK = 0x0408,

    // enabling and disabling
    gl_BLEND = 0x0BE2,
    gl_DEPTH_TEST = 0x0B71,
    gl_DITHER = 0x0BD0,
    gl_POLYGON_OFFSET_FILL = 0x8037,
    gl_SAMPLE_ALPHA_TO_COVERAGE = 0x809E,
    gl_SAMPLE_COVERAGE = 0x80A0,
    gl_SCISSOR_TEST = 0x0C11,
    gl_STENCIL_TEST = 0x0B90,

    // textures
    gl_NEAREST = 0x2600,
    gl_LINEAR = 0x2601,
    gl_NEAREST_MIPMAP_NEAREST = 0x2700,
    gl_LINEAR_MIPMAP_NEAREST = 0x2701,
    gl_NEAREST_MIPMAP_LINEAR = 0x2702,
    gl_LINEAR_MIPMAP_LINEAR = 0x2703,
    gl_TEXTURE_MAG_FILTER = 0x2800,
    gl_TEXTURE_MIN_FILTER = 0x2801,
    gl_TEXTURE_WRAP_S = 0x2802,
    gl_TEXTURE_WRAP_T = 0x2803,
    gl_TEXTURE_2D = 0x0DE1,
    gl_TEXTURE = 0x1702,
    gl_TEXTURE_CUBE_MAP = 0x8513,
    gl_TEXTURE_BINDING_CUBE_MAP = 0x8514,
    gl_TEXTURE_CUBE_MAP_POSITIVE_X = 0x8515,
    gl_TEXTURE_CUBE_MAP_NEGATIVE_X = 0x8516,
    gl_TEXTURE_CUBE_MAP_POSITIVE_Y = 0x8517,
    gl_TEXTURE_CUBE_MAP_NEGATIVE_Y = 0x8518,
    gl_TEXTURE_CUBE_MAP_POSITIVE_Z = 0x8519,
    gl_TEXTURE_CUBE_MAP_NEGATIVE_Z = 0x851A,
    gl_MAX_CUBE_MAP_TEXTURE_SIZE = 0x851C,
    gl_TEXTURE0 = 0x84C0, // TEXTURE1 through 31 = TEXTURE_0+n
    gl_ACTIVE_TEXTURE = 0x84E0,
    gl_REPEAT = 0x2901,
    gl_CLAMP_TO_EDGE = 0x812F,
    gl_MIRRORED_REPEAT = 0x8370,
};

#include <string>

#include "vec3.h"

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

template <typename T>
void* createVbo(Buffer<T> buffer, GlConstant bufferType = gl_STATIC_DRAW) {
    void* vbo = gl::createBuffer();
    gl::bindBuffer(gl_ARRAY_BUFFER, vbo);
    gl::bufferData(gl_ARRAY_BUFFER, &buffer, bufferType);
    return vbo;
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
