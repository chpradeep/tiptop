module.exports = {
    // mongoDB database to be connected
    //database: 'mongodb://localhost:27017/cdaciot'
    database: 'mongodb://admin:admin123@ds147390.mlab.com:47390/platform',
    mqttSettings : {
        BROKER_URL : "iot.eclipse.org",
        PUB_TOPIC : "action",
        SUB_TOPIC : "event",
        USERNAME : "username",
        PASSWORD : "password"
    }
}