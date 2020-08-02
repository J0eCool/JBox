async function run() {
    let elem = document.getElementById('canvas');
    let ctx = elem.getContext('2d');
    let image = new ImageData(elem.width, elem.height);

    let canvas = {
        updateImage(buffer) {
            image.data.set(new Uint8Array(buffer));
        },
    };

    // Load modules
    let wasm = await imageComponent.instantiate({
        canvas,
    });

    function frame() {
        wasm.run();
        ctx.putImageData(image, 0, 0);

        requestAnimationFrame(frame);
    }
    requestAnimationFrame(frame);
}

run();
