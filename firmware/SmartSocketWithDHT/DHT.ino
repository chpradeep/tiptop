//Parsing program
const size_t bufferSize = JSON_OBJECT_SIZE(7) + 150;
DynamicJsonBuffer jsonBuffer(bufferSize);

const char* json = "{\"Temperature\":54,\"Humidity\":\"Number\",\"Frequency\":\"Number\",\"TempMaxLimit\":\"Number\",\"TempMinLimit\":\"Number\",\"HumiMaxLimit\":\"Number\",\"HuniMinLimit\":\"Number\"}";

JsonObject& dht = jsonBuffer.parseObject(json);

int Temperature = dht["Temperature"]; // 54
const char* Humidity = dht["Humidity"]; // "Number"
const char* Frequency = dht["Frequency"]; // "Number"
const char* TempMaxLimit = dht["TempMaxLimit"]; // "Number"
const char* TempMinLimit = dht["TempMinLimit"]; // "Number"
const char* HumiMaxLimit = dht["HumiMaxLimit"]; // "Number"
const char* HuniMinLimit = dht["HuniMinLimit"]; // "Number"

//Serializing program
const size_t bufferSize = JSON_OBJECT_SIZE(7);
DynamicJsonBuffer jsonBuffer(bufferSize);

JsonObject& dht = jsonBuffer.createObject();
dht["Temperature"] = 54;
dht["Humidity"] = "Number";
dht["Frequency"] = "Number";
dht["TempMaxLimit"] = "Number";
dht["TempMinLimit"] = "Number";
dht["HumiMaxLimit"] = "Number";
dht["HuniMinLimit"] = "Number";

dht.printTo(Serial);
