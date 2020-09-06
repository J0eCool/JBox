precision mediump float;

uniform sampler2D uImage;

varying vec2 vTexUv;

void main() {
    gl_FragColor = texture2D(uImage, vTexUv);
}
