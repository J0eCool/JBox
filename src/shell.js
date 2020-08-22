let canvas = document.getElementById('canvas');
let ctx = canvas.getContext('2d');
ctx.imageSmoothingEnabled = false;
let image = new ImageData(canvas.width, canvas.height);

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
    loadImage(url, callback) {
        let img = new Image();
        img.src = url;
        img.decode().then(() => {
            let off = new OffscreenCanvas(img.width, img.height);
            let offCtx = off.getContext('2d');
            offCtx.drawImage(img, 0, 0);
            let offImg = offCtx.getImageData(0, 0, img.width, img.height);
            let image = {
                pixels: offImg.data.buffer.slice(),
                width: img.width,
                height: img.height,
            };
            callback(image);
        });
    },
};
let math = {
    rnd() {
        return Math.random();
    },
};
let objects = {
    create() {
        return {};
    },
    copy(obj) {
        return Object.assign({}, obj);
    },
    getField(obj, field) {
        return obj[field];
    },
    setField(obj, field, val) {
        obj[field] = val;
    },
};
let wasm = {
    getLoadedModules() {
        // make a deepish (2-level) copy
        let ret = {};
        for (let k in loadedModules) {
            ret[k] = Object.assign({}, loadedModules[k]);
        }
        return ret;
    },
    loadModule(name, imports, callback) {
        let components = {
            'circles': circlesComponent,
            'life': lifeComponent,
            'terminal': terminalComponent,
        };
        return components[name].instantiate(imports).then((mod) => {
            loadedModules[name] = mod;
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

let loadedModules = {
    graphics,
    input,
    math,
    objects,
    wasm,
};

async function run() {
    // internal resolution; separate from the canvas resolution
    let w = 800;
    let h = 600;
    image = new ImageData(w, h);

    // Load modules
    let mod = await wasm.loadModule('terminal', loadedModules, () => {});

    mod.init(w, h);

    function frame() {
        mod.frame();

        requestAnimationFrame(frame);
    }
    requestAnimationFrame(frame);
}

run();
