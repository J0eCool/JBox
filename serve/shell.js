let resolution = {
    width: 800,
    height: 600,
};

let canvas = document.getElementById('canvas');
let gl = canvas.getContext('webgl2');

let loadedModules = {
    files,
    input,
    gl,
    math,
    objects,
    time,
    wasm,
};

async function parserTest() {
    let lexer = await wasm.loadModule('lexer', loadedModules);
    console.log('Lexing!');
    let lexInput = await fetch('parser.itl').then((x) => x.text());
    let tokens = lexer.lex(lexInput);
    let parser = await wasm.loadModule('parser', loadedModules);
    console.log('Parsing!');
    let ast = parser.parse(tokens);
    console.log(ast);
    let p = parser.prettyPrint(ast);
    console.log('postparse:', p);
}

async function main() {
    // await parserTest();

    let mod = await wasm.loadModule('game', loadedModules);
    mod.init(resolution.width, resolution.height);

    function frame() {
        mod.frame();

        requestAnimationFrame(frame);
    }
    requestAnimationFrame(frame);
}

main();
