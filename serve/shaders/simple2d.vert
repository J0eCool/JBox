attribute vec4 aPos;
attribute vec2 aTexUv;

uniform mat4 uMatrix;

varying vec2 vTexUv;

void main() {
    gl_Position = uMatrix * aPos;
    vTexUv = aTexUv;
}
