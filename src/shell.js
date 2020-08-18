let canvas = document.getElementById('canvas');
let ctx = canvas.getContext('2d');
ctx.imageSmoothingEnabled = false;
let image = new ImageData(canvas.width, canvas.height);

let fontBuffer;

function getKeyCode(event) {
    // Custom enum of key values because what are JS KeyboardEvents even
    let key = {
        'Shift': 1,
        'Control': 2,
        'Alt': 3,
        'Enter': 4,
        'Escape': 5,
        'Backspace': 6,
        'Delete': 7,
    }[event.key];
    if (key !== undefined) {
        return key;
    }
    return event.key.charCodeAt(0);
}
let input = {
    log(msg, x) {
        console.log(msg, x);
    },
    registerOnKeyDown(callback) {
        document.onkeydown = (event) => {
            callback(getKeyCode(event));
        };
    },
    registerOnKeyUp(callback) {
        document.onkeyup = (event) => {
            callback(getKeyCode(event));
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
let wasm = {
    loadModule(name, callback) {
        let components = {
            'circles': circlesComponent,
            'life': lifeComponent,
            'terminal': terminalComponent,
        }
        return components[name].instantiate(allImports).then((mod) => {
            callback(mod);
            return mod;
        });
    },
    callInit(mod, x, y) {
        mod.init(x, y);
    },
    callFrame(mod) {
        mod.frame();
    },
};

let allImports = {
    graphics,
    input,
    math,
    wasm,
};

async function run() {
    // internal resolution; separate from the canvas resolution
    let w = 400;
    let h = 300;
    image = new ImageData(w, h);

    // Load modules
    let mod = await wasm.loadModule('terminal', () => {});

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

    mod.init(w, h);

    function frame() {
        mod.frame();

        requestAnimationFrame(frame);
    }
    requestAnimationFrame(frame);
}

run();
