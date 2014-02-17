var mongoose = require('mongoose');
var moment = require('moment');
var MessageSchema = new mongoose.Schema({
        message: String,
        created: {type: Date, default: new Date()},
        staytime: Number,
        pictureurl: String
    },
    {
        toObject: {virtuals: true},
        toJSON: {virtuals: true}

});
MessageSchema
    .virtual('getDate').get(function(){
        return moment(this.created).format('MMMM Do YYYY. h:mm:ss a');
    });

exports.MessageSchema = MessageSchema;
