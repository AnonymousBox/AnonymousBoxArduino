var mongoose = require('mongoose');
var db = mongoose.createConnection('localhost', 'AnonymousBox');
var MessageSchema = require('../models/messageObjectSchema.js').MessageSchema;
var MessageModel = db.model('MessageSchema', MessageSchema);
var path = require('path');
var fs = require('fs');

var average = 0;
var getAverage = function(){
    var staytimes = 0;
    var staylen = 0
    MessageModel.find({}).select('staytime').exec(function(err, docs){
        docs.forEach(function(e){
            if(e.staytime){
                staytimes += e.staytime;
                staylen++;
            }
        });
        return staytimes/staylen
    });
}
exports.list = function(req, res){
    res.render('index.jade');
};
exports.post = function(req, res){
    console.log(req.files);
    var pictureUrls = [];
    var tempPaths = [];
    var fileNames = [];
    for(key in req.files){
        tp = req.files[key].path;
        fn = req.files[key].name;
        pictureUrls.push(fn);
        var targetPath = path.resolve('./public/images/'+fn);
        fs.rename(tp, targetPath, function(err,doc) {
            if (err) throw err;
            console.log("Upload completed!: ", doc);
        });
    }
    console.log("picture urls: ", pictureUrls);
    messageObject = {
        message: req.body.message,
        staytime: req.body.staytime,
        pictureurls: pictureUrls
    };
    var postMessage = new MessageModel(messageObject);
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
    MessageModel.find().sort({_id:-1}).exec(function(err, docs){
        if(!err){
            res.json(docs);
        }else{
            console.log("got error");
        }
    });
};
