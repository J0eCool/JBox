let canvas = document.getElementById('canvas');
let ctx = canvas.getContext('2d');
ctx.imageSmoothingEnabled = false;
let image = new ImageData(canvas.width, canvas.height);

let fontBuffer;

let input = {
    log(msg, x) {
        console.log(msg, x);
    },
    registerOnKeyDown(callback) {
        document.onkeydown = (event) => {
            callback(event.keyCode);
        };
    },
};
let graphics = {
    updateImage(buffer) {
        image.data.set(new Uint8Array(buffer));
        ctx.putImageData(image, 0, 0);

        // scale the image to cover the full canvas
        let sw = canvas.width / image.width;
        let sh = canvas.height / image.height;
        ctx.drawImage(canvas, 0, 0, sw * canvas.width, sh * canvas.height);
    },
    loadImage(url) {
        return fontBuffer;
    },
};
let math = {
    rnd() {
        return Math.random();
    },
};

async function run(component) {
    // internal resolution; separate from the canvas resolution
    let w = 400;
    let h = 300;
    image = new ImageData(w, h);

    // Load modules
    let wasm = await component.instantiate({
        graphics,
        input,
        math,
    });

    // Load font asset
    // TODO: figure out some callback-based API
    let img = new Image();
    img.src = 'textures/font.png';
    await img.decode();
    let off = new OffscreenCanvas(img.width, img.height);
    let offCtx = off.getContext('2d');
    // ctx.drawImage(img, 0, 0);
    offCtx.drawImage(img, 0, 0);
    let offImg = offCtx.getImageData(0, 0, img.width, img.height);
    fontBuffer = offImg.data.buffer.slice();

    wasm.init(w, h);

    function frame() {
        wasm.frame();

        requestAnimationFrame(frame);
    }
    requestAnimationFrame(frame);
}

// run(circlesComponent);
// run(lifeComponent);
run(terminalComponent);
