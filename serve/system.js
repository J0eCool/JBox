// a set of modules that make up the syscall layer

function keyboardEventWrapper(callback) {
    return (event) => {
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
            return callback(key);
        }
        if (event.key.length == 1) {
            return callback(event.key.charCodeAt(0));
        }
        return callback(0);
    };
}
function mouseEventWrapper(callback) {
    return function(event) {
        return callback({
            button: event.button,
            x: event.clientX / canvas.clientWidth,
            y: event.clientY / canvas.clientHeight,
        });
    };
}
let input = {
    log: console.log,
    registerOnKeyDown(callback) {
        document.onkeydown = keyboardEventWrapper(callback);
    },
    registerOnKeyUp(callback) {
        document.onkeyup = keyboardEventWrapper(callback);
    },
    registerOnMouseDown(callback) {
        canvas.onmousedown = mouseEventWrapper(callback);
    },
    registerOnMouseUp(callback) {
        canvas.onmouseup = mouseEventWrapper(callback);
    },
    registerOnMouseMove(callback) {
        canvas.onmousemove = mouseEventWrapper(callback);
    },
};
let nextFileId = 1;
let files = {
    loadText(url, callback, ctx) {
        let id = nextFileId++;
        fetch(url).then((resp) => resp.text()).then((text) => {
            let data = {
                id,
                url,
                text,
            };
            callback(data, ctx);
        });
        return id;
    },
    loadImage(url, callback, ctx) {
        let id = nextFileId++;
        let img = new Image();
        img.src = url;
        img.decode().then(() => {
            let off = new OffscreenCanvas(img.width, img.height);
            let offCtx = off.getContext('2d');
            offCtx.drawImage(img, 0, 0);
            let offImg = offCtx.getImageData(0, 0, img.width, img.height);
            let image = {
                id,
                url,
                pixels: offImg.data.buffer.slice(),
                width: img.width,
                height: img.height,
            };
            callback(image, ctx);
        });
        return id;
    },
};
let math = {
    rand() {
        return Math.random();
    },
    sin: Math.sin,
    cos: Math.cos,
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
let time = {
    now() {
        return performance.now() / 1000;
    },
};
let wasm = {
    getLoadedModules() {
        // make a deepish (2-level) copy
        let ret = {};
        for (let k in loadedModules) {
            if (loadedModules[k] === gl) {
                // refuses to obj.assign
                ret[k] = gl;
            } else {
                ret[k] = Object.assign({}, loadedModules[k]);
            }
        }
        return ret;
    },
    loadModule(name, imports, callback) {
        let components = {
            'circles': circlesComponent,
            'game': gameComponent,
            'lexer': lexerComponent,
            'life': lifeComponent,
            'parser': parserComponent,
            'terminal': terminalComponent,
        };
        return components[name].instantiate(imports).then((mod) => {
            loadedModules[name] = mod;
            if (callback) {
                callback(mod);
            }
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
