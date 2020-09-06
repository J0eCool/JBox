attribute vec4 aPos;

uniform mat4 uMatrix;

varying vec3 vPos;

void main() {
    vec4 pos = uMatrix * aPos;
    gl_Position = pos;
    vPos = pos.xyz;
}
