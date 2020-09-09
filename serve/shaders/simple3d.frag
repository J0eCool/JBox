precision mediump float;

uniform vec3 uColor;
uniform vec3 uLightPos;

varying vec3 vPos;
varying vec3 vNormal;

void main() {
    float ambient = 0.2;
    float brightness = 40.0;

    vec3 normal = normalize(vNormal);
    vec3 delta = uLightPos - vPos;
    vec3 dir = normalize(delta);
    float dist = length(delta);

    float lum = dot(normal, dir) * brightness / dist;
    float light = max(ambient, lum);
    gl_FragColor = vec4(uColor * light, 1);
}
