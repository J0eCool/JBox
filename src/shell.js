async function run() {
    let elem = document.getElementById('canvas');
    let ctx = elem.getContext('2d');
    let image = new ImageData(elem.width, elem.height);
    let i32s = new Uint32Array(image.data.buffer);

    let canvas = {
        setPixel(x, y, val) {
            i32s[x + y*elem.width] = val;
        },
    };

    // Load modules
    let wasm = await imageComponent.instantiate({
        canvas,
    });

    // Run it
    wasm.run();

    // Update frame data
    ctx.putImageData(image, 0, 0);
}

run();
