var mongoose = require('mongoose');
exports.messageSchema = new mongoose.Schema({
        message: String,
        time: String,
        staytime: String
});

