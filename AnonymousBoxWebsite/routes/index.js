var mongoose = require('mongoose');
var db = mongoose.createConnection('localhost', 'AnonymousBox');
var MessageSchema = require('../models/messageObjectSchema.js').messageSchema;
var newMessageSchema = db.model('messageSchema', MessageSchema);
var path = require('path');
var fs = require('fs');

exports.list = function(req, res){
    res.render('index.jade');
};
exports.post = function(req, res){
    var tempPath = req.files.file.path;
    var tempPathName = tempPath.split('/')[tempPath.split('/').length-1];
    var targetPath = path.resolve('./public/images/'+tempPathName);
    messageObject = {
        message: req.body.message,
        staytime: req.body.staytime,
        pictureurl: tempPathName
    };
    fs.rename(tempPath, targetPath, function(err) {
        if (err) throw err;
        console.log("Upload completed!");
    });
    var postMessage = new newMessageSchema(messageObject);
    postMessage.save(function(err, doc){
        if(err || !doc){
            throw 'Error';
        }else{
            console.log("created");
            console.log(doc);
            res.json(doc);
        }
    });

};
exports.getmessages = function(req, res){
    newMessageSchema.find().sort({_id:-1}).exec(function(err, docs){
        if(!err){
            res.json(docs);
        }else{
            console.log("got error");
        }
    });
};
