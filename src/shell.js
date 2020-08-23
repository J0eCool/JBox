// Initialize WebGL
let canvas = document.getElementById('canvas');
let gl = canvas.getContext('webgl');

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
    attribute vec4 vertexPosition;
    void main() {
        gl_Position = vertexPosition;
    }
`;
let fragShaderText = `
    precision mediump float;
    void main() {
        gl_FragColor = vec4(0.3, 0.7, 0.9, 1.0);
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
    vertexPosition: gl.getAttribLocation(shaderProgram, 'vertexPosition'),
};

// quad vbos
let vert = [
    -0.5, -0.5,
    -0.5, 0.5,
    0.5, 0.5,
    0.5, -0.5,
];
let vertBuffer = gl.createBuffer();
gl.bindBuffer(gl.ARRAY_BUFFER, vertBuffer);
gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vert), gl.STATIC_DRAW);
let tex = [0.0, 0.0, /**/ 1.0, 0.0, /**/ 1.0, 1.0, /**/ 0.0, 1.0];
let texBuffer = gl.createBuffer();
gl.bindBuffer(gl.ARRAY_BUFFER, texBuffer);
gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(tex), gl.STATIC_DRAW);
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
        // gl.viewport(0, 0, canvas.width, canvas.height);
        gl.clearColor(0, 0, 0, 1);
        // gl.clearDepth(1.0);
        // gl.enable(gl.DEPTH_TEST);
        gl.depthFunc(gl.LEQUAL);

        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        gl.useProgram(programInfo.program);

        gl.enableVertexAttribArray(programInfo.vertexPosition);
        gl.bindBuffer(gl.ARRAY_BUFFER, vertBuffer);
        gl.vertexAttribPointer(programInfo.vertexPosition, 2, gl.FLOAT, false, 0, 0);

        gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

        // gl.bindTexture(gl.TEXTURE_2D, imageTex);
        // gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, image.width, image.height, 0, gl.RGBA, gl.UNSIGNED_BYTE, image.buffer);

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
    imageDrawing,
    input,
    math,
    objects,
    wasm,
};

async function main() {
    // Load modules
    let mod = await wasm.loadModule('terminal', loadedModules, () => {});

    mod.init(resolution.width, resolution.height);

    function frame() {
        mod.frame();

        requestAnimationFrame(frame);
    }
    requestAnimationFrame(frame);
}

main();
