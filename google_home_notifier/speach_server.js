var express = require('express');
var googlehome = require('./google-home-notifier');
var ngrok = require('ngrok');
var bodyParser = require('body-parser');
var app = express();
const serverPort = 8080;

var urlencodedParser = bodyParser.urlencoded({ extended: false });

app.use(bodyParser.json());
app.post('/google-home-notifier', urlencodedParser, function (req, res) {
  if (!req.body) return res.sendStatus(400)
  var text = req.body.text;
  var deviceName = req.body.deviceName;
  console.log(req.body);
  var ip = req.body.ip;
  if (text) {
    try {

      googlehome.ip(ip);
      googlehome.device(deviceName, "ja");
      googlehome.notify(text, function (notifyRes) {
        console.log(notifyRes);
        res.send(deviceName + ' will say: ' + text + '\n');
      });
    } catch (err) {
      console.log(err);
      res.sendStatus(500);
      res.send(err);
    }
  } else {
    res.send('Please POST "text=Hello Google Home"');
  }
})

app.listen(serverPort, function () {
  //  ngrok.connect(serverPort, function (err, url) {
  console.log('POST "text=Hello Google Home" to:');
  console.log('    http://localhost:' + serverPort + '/google-home-notifier');
  //  console.log('    ' + url + '/google-home-notifier');
  console.log('example:');
  //  console.log('curl -X POST -d "text=Hello Google Home" ' + url + '/google-home-notifier');
  //});
})
