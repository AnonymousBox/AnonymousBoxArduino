var mongoose = require('mongoose');
exports.messageSchema = new mongoose.Schema({
        message: String,
        created: {type: Date, default: Date.now},
        staytime: String,
        pictureurl: String
});

