async function run() {
    let elem = document.getElementById('canvas');
    let ctx = elem.getContext('2d');
    let image = new ImageData(elem.width, elem.height);
    let i32s = new Uint32Array(image.data.buffer);

    let canvas = {
        updateImage(buffer) {
            console.log('buvfer:', buffer);
            // i32s.set(new Uint32Array(buffer));
            let inc = new Uint32Array(buffer);
            for (let i = 0; i < i32s.length; ++i) {
                i32s[i] = inc[i];
                // i32s[i] = 0x44556600 + 0xff * i / i32s.length;
            }
            // image.data.set(i32s);
        },
        testy(buffer) {
            console.log(buffer);
        },
        log(msg, x) {
            console.log(msg, x);
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
