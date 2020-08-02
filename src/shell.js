let canvas = document.getElementById('canvas');
let ctx = canvas.getContext('2d');
ctx.imageSmoothingEnabled = false;
let image = new ImageData(canvas.width, canvas.height);

let graphics = {
    updateImage(buffer) {
        image.data.set(new Uint8Array(buffer));
    },
};

async function runCircles() {
    // internal resolution; separate from the canvas resolution
    let w = 800;
    let h = 600;
    image = new ImageData(w, h);

    // scaling factor, manages the ratio from internal to canvas resolution
    let sw = canvas.width / w;
    let sh = canvas.height / h;

    // Load modules
    let wasm = await circlesComponent.instantiate({
        graphics,
    });

    wasm.init(w, h);

    function frame() {
        wasm.frame();

        ctx.putImageData(image, 0, 0);
        // scale the image to cover the full canvas
        ctx.drawImage(canvas, 0, 0, sw * canvas.width, sh * canvas.height);

        requestAnimationFrame(frame);
    }
    requestAnimationFrame(frame);
}

runCircles();
