const http = require('http');
const fs = require('fs');

const port = 8080;

const server = http.createServer((req, res) => {
    console.log('Request at:', req.url);
    let path = './serve' + req.url;
    if (req.url === '/') {
        path = './serve/index.html';
    }
    try {
        fs.accessSync(path);
    } catch (err) {
        path = './out' + req.url;
    }

    console.log('Reading from:', path);
    fs.readFile(path, (err, data) => {
        if (err) {
            console.error(err);
            return;
        }
        let mime = 'text/plain';
        if (path.endsWith('.html')) {
            mime = 'text/html';
        } else if (path.endsWith('.css')) {
            mime = 'text/css';
        } else if (path.endsWith('.js')) {
            mime = 'text/javascript';
        } else if (path.endsWith('.wasm')) {
            mime = 'application/wasm';
        }
        res.writeHead(200, {
            'Content-Type': mime,
        })
        res.end(data);
    });
});
server.listen(port, (err) => {
    if (err) {
        console.log('ERROR:', err);
        return;
    }
    console.log('Started server on port', port);
})
