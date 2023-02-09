const express = require('express');
const path = require('path');
const iconv = require('iconv-lite');
const {exec} = require("child_process");

const PORT = 3000;

const app = express();

const createPath = (page) => path.resolve(__dirname, 'views', `${page}.html`);

const urlEncodedParser = express.urlencoded({extended:false});

var addon = require('./build/Release/addon');

app.listen(PORT, 'localhost', (error) => {
    error ? console.log(error) : console.log(`listening port ${PORT}`);
});

app.get('/', (req, res) => {
    res.sendFile(createPath('index'));
})

app.post('/', urlEncodedParser, (req, res) => {
    if (!req.body) {
        return res.sendStatus(400);
    } else {
        var name = req.body.userName;


        res.writeHead(200, {"Content-Type": "text/html; charset=utf-8"});

        if(addon.userExists(name)) {
            res.write('<h1>Пользователь ' + name + ' есть</h1>', 'utf-8');
        }else {
            res.write('<h1>Пользователя ' + name + ' нет</h1>', 'utf-8');
        }
        res.write('<a href="/">return</a>');
        res.end();

    }
})

app.use((req, res) => {
    res
        .status(404)
        .sendFile(createPath('error'));
})

