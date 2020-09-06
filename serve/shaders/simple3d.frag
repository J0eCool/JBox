precision mediump float;

uniform vec3 uColor;
uniform vec3 uLightPos;

varying vec3 vPos;

void main() {
    float range = 3.0;
    float dist = length(vPos - uLightPos);
    float lum = (range - dist) / range;
    float light = clamp(1.0 - lum*lum, 0.0, 1.0);
    gl_FragColor = vec4(uColor * light, 1);
}
