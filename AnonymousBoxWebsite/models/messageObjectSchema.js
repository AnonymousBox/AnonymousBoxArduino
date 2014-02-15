var mongoose = require('mongoose');
exports.messageSchema = new mongoose.Schema({
        message: String,
        created: {type: Date, default: new Date()},
        staytime: String,
        pictureurl: String
});

