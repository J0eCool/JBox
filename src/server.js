const http = require('http');
const fs = require('fs');

const port = 8080;

const server = http.createServer((req, res) => {
    console.log(req.url);
    let path = './' + req.url;

    fs.readFile(path, (err, data) => {
        if (err) {
            console.log('ERROR:', err);
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
})
server.listen(port, (err) => {
    if (err) {
        console.log('ERROR:', err);
        return;
    }
    console.log('Started server on port', port);
})
