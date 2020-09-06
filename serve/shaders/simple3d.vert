precision mediump float;

attribute vec4 aPos;
attribute vec3 aNormal;

uniform mat4 uModelViewProj;
uniform mat4 uModel;

varying vec3 vPos;
varying vec3 vNormal;

void main() {
    vec4 pos = uModelViewProj * aPos;
    gl_Position = pos;
    vPos = mat3(uModel) * aPos.xyz;
    vNormal = mat3(uModel) * aNormal;
}
