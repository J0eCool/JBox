let elem = document.getElementById('canvas');
let ctx = elem.getContext('2d');
// let image = new ImageData(elem.width, elem.height);
let image = null;

let graphics = {
    updateImage(buffer) {
        image.data.set(new Uint8Array(buffer));
    },
};


async function run() {
    // Load modules
    let wasm = await circlesComponent.instantiate({
        graphics,
    });
    image = new ImageData(400, 300);

    function frame() {
        wasm.run();
        ctx.putImageData(image, 0, 0);

        requestAnimationFrame(frame);
    }
    requestAnimationFrame(frame);
}

run();
