var createError = require('http-errors');
var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');

var indexRouter = require('./routes/index');
var devRouter = require('./routes/device');
var userRouter = require("./routes/user");
var mqttHandler = require('./routes/mqtt');

const mongoose = require('mongoose');

//  Requiring credentials file
const cred = require('./config/credentials');

// -------- MongoDB operations----------------
mongoose.connect(cred.database);

mongoose.connection.on('connected', () => {
    console.log('connected to Database');
});

mongoose.connection.on('error', (err) => {
    console.log('Database error: ' + err);
});

//--------------------------------------------

var mqttClient = new mqttHandler(cred.mqttSettings);

mqttClient.connect();
//---------------------------------------------

var app = express();

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'jade');

app.use(logger('dev'));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.use('/', indexRouter);
app.use('/device' , devRouter);
app.use('/user' , userRouter);

// catch 404 and forward to error handler
app.use(function(req, res, next) {
  next(createError(404));
});

// error handler
app.use(function(err, req, res, next) {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get('env') === 'development' ? err : {};

  // render the error page
  res.status(err.status || 500);
  res.render('error');
});

module.exports = app;
