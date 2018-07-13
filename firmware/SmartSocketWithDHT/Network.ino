//Parsing program
const size_t bufferSize = JSON_OBJECT_SIZE(3) + 50;
DynamicJsonBuffer jsonBuffer(bufferSize);

const char* json = "{\"IP\":\"String\",\"SSID\":\"String\",\"PASSWORD\":\"fdsfds\"}";

JsonObject& net = jsonBuffer.parseObject(json);

const char* IP = net["IP"]; // "String"
const char* SSID = net["SSID"]; // "String"
const char* PASSWORD = net["PASSWORD"]; // "fdsfds"


//Serializing program
const size_t bufferSize = JSON_OBJECT_SIZE(3);
DynamicJsonBuffer jsonBuffer(bufferSize);

JsonObject& net = jsonBuffer.createObject();
net["IP"] = "String";
net["SSID"] = "String";
net["PASSWORD"] = "fdsfds";

net.printTo(Serial);
