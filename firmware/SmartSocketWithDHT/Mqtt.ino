//Parsing program
const size_t bufferSize = JSON_OBJECT_SIZE(5) + 100;
DynamicJsonBuffer jsonBuffer(bufferSize);

const char* json = "{\"BROKER_URL\":\"String\",\"PUB_TOPIC\":\"String\",\"SUB_TOPIC\":\"String\",\"USERNAME\":\"String\",\"PASSWORD\":\"String\"}";

JsonObject& mqtt = jsonBuffer.parseObject(json);

const char* BROKER_URL = mqtt["BROKER_URL"]; // "String"
const char* PUB_TOPIC = mqtt["PUB_TOPIC"]; // "String"
const char* SUB_TOPIC = mqtt["SUB_TOPIC"]; // "String"
const char* USERNAME = mqtt["USERNAME"]; // "String"
const char* PASSWORD = mqtt["PASSWORD"]; // "String"


//Serializing program
const size_t bufferSize = JSON_OBJECT_SIZE(5);
DynamicJsonBuffer jsonBuffer(bufferSize);

JsonObject& mqtt = jsonBuffer.createObject();
mqtt["BROKER_URL"] = "String";
mqtt["PUB_TOPIC"] = "String";
mqtt["SUB_TOPIC"] = "String";
mqtt["USERNAME"] = "String";
mqtt["PASSWORD"] = "String";

mqtt.printTo(Serial);
