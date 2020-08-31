// Initialize WebGL
let canvas = document.getElementById('canvas');
let gl = canvas.getContext('webgl2');

// shaders
function loadShader(type, source) {
    let shader = gl.createShader(type);
    gl.shaderSource(shader, source);
    gl.compileShader(shader);
    if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
        console.error('Shader failed to compile: ' + gl.getShaderInfoLog(shader));
        gl.deleteShader(shader);
        shader = null;
    }
    return shader;
}
let vertShaderText = `
    attribute vec4 aVertexPosition;
    attribute vec2 aTexCoord;

    varying vec2 vTexCoord;

    void main() {
        gl_Position = aVertexPosition;
        vTexCoord = aTexCoord;
    }
`;
let fragShaderText = `
    precision mediump float;

    uniform sampler2D uImage;

    varying vec2 vTexCoord;

    void main() {
        gl_FragColor = texture2D(uImage, vTexCoord);
    }
`;
let shaderProgram = gl.createProgram();
gl.attachShader(shaderProgram, loadShader(gl.VERTEX_SHADER, vertShaderText));
gl.attachShader(shaderProgram, loadShader(gl.FRAGMENT_SHADER, fragShaderText));
gl.linkProgram(shaderProgram);
if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
    console.error('Program failed to link: ' + gl.getProgramInfoLog(shaderProgram));
    shaderProgram = null;
}
let programInfo = {
    program: shaderProgram,
    vertexPosition: gl.getAttribLocation(shaderProgram, 'aVertexPosition'),
    texCoord: gl.getAttribLocation(shaderProgram, 'aTexCoord'),
};

// quad vbos
let vertBuffer = gl.createBuffer();
gl.bindBuffer(gl.ARRAY_BUFFER, vertBuffer);
gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([
    -1.0, 1.0,
    1.0, 1.0,
    1.0, -1.0,
    -1.0, -1.0,
]), gl.STATIC_DRAW);
let texBuffer = gl.createBuffer();
gl.bindBuffer(gl.ARRAY_BUFFER, texBuffer);
gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([
    0.0, 0.0,
    1.0, 0.0,
    1.0, 1.0,
    0.0, 1.0,
]), gl.STATIC_DRAW);
let imageTex = gl.createTexture();

let resolution = {
    width: 800,
    height: 600,
};

// -----------------------

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
    if (event.key.length == 1) {
        return event.key.charCodeAt(0);
    }
    return 0;
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
let imageDrawing = {
    updateImage(image) {
        // Clear
        gl.clearColor(0, 0, 0, 1);
        // gl.clearDepth(1.0);
        // gl.enable(gl.DEPTH_TEST);
        // gl.depthFunc(gl.LEQUAL);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // Copy image data to GPU
        gl.bindTexture(gl.TEXTURE_2D, imageTex);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, image.width, image.height, 0,
            gl.RGBA, gl.UNSIGNED_BYTE, new Uint8Array(image.pixels));

        // Set shader attributes
        gl.useProgram(programInfo.program);
        gl.enableVertexAttribArray(programInfo.vertexPosition);
        gl.bindBuffer(gl.ARRAY_BUFFER, vertBuffer);
        gl.vertexAttribPointer(programInfo.vertexPosition, 2, gl.FLOAT, false, 0, 0);
        gl.enableVertexAttribArray(programInfo.texCoord);
        gl.bindBuffer(gl.ARRAY_BUFFER, texBuffer);
        gl.vertexAttribPointer(programInfo.texCoord, 2, gl.FLOAT, false, 0, 0);

        // Draw
        gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
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

let loadedModules = {
    imageDrawing,
    input,
    gl,
    math,
    objects,
    time,
    wasm,
};

async function main() {
    let lexer = await wasm.loadModule('lexer', loadedModules);
    console.log('Lexing!');
    let lexInput = 'print-line  (+ 2 (* 3 4))';
    let tokens = lexer.lex(lexInput);
    console.log('tokens:', tokens);

    // Load modules
    let mod = await wasm.loadModule('game', loadedModules);

    mod.init(resolution.width, resolution.height);

    function frame() {
        mod.frame();

        requestAnimationFrame(frame);
    }
    requestAnimationFrame(frame);
}

main();
