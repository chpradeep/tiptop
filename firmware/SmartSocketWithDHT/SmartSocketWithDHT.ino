#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>



const char* ssid = "abcd";
const char* password = "asdf1234";

int Frequency = 5;
int TempMaxLimit = 30;
int TempMinLimit = 0;
int HumiMaxLimit = 70;
int HuniMinLimit = 40;

float Temperature = 0;
float Humidity = 0;

/*var BROKER_URL = "";
var PUB_TOPIC = "";
var SUB_TOPIC = "";
var USERNAME = "";
var PASSWORD = "";


function onConnect(){
    msg.payload = {"SSID":SSID , "IP":"fdsd" , "STATUS":true}
}
function onDisconnect(){
    msg.payload = {"STATUS":false}
}
function onUpdateOfTemperaturesAndHumidity(){
    node.log("fdsds");
    msg.payload = {"Temp":Temperature , "Humi":Humidity}
}
function onUpdateOfDeviceConfiguration(){
    msg.payload = {
        "Frequency": Frequency,
        "TempMaxLimit": TempMaxLimit,
        "TempMinLimit": TempMinLimit,
        "HumiMaxLimit": HumiMaxLimit,
        "HuniMinLimit": HuniMinLimit
    }
}
function onSwitching(){
    msg.payload = [switch1 ,switch2 ,switch3 ,switch4];
}*/

/*
var input = msg.payload;
if(input.type == "SmartSocketv0.0Switching"){
    var buttonNo = input['Switch'];
    node.log(buttonNo)
    if(buttonNo){
        console.log("Its switching button "+buttonNo);
        if(buttonNo == "switch1")
            switch1 = !switch1;
        if(buttonNo == "switch2")
            switch2 = !switch2;
        if(buttonNo == "switch3")
            switch3 = !switch3;
        if(buttonNo == "switch4")
            switch4 = !switch4;
        onSwitching();
    }
}
else if(input.type == "SmartSocketv0.0ConfigMQTTProtocol"){
    BROKER_URL = input['BROKER_URL'];
    PUB_TOPIC = input['PUB_TOPIC'];
    SUB_TOPIC = input['SUB_TOPIC'];
    USERNAME = input['USERNAME'];
    PASSWORD = input['PASSWORD'];
    onConnect();
}
else if(input.type == "SmartSocketv0.0ReadTemperaturesAndHumidity"){
    onUpdateOfTemperaturesAndHumidity();
}
else if(input['type'] == "SmartSocketv0.0ConfigureDevice"){
    Frequency = input['Frequency'];
    TempMaxLimit = input['TempMaxLimit'];
    TempMinLimit = input['TempMinLimit'];
    HumiMaxLimit = input['HumiMaxLimit'];
    HuniMinLimit = input['HuniMinLimit'];
    onUpdateOfDeviceConfiguration();
}
else if(input.type == "SmartSocketv0.0ConfigNetwork"){
    SSID = input['SSID'];
    PASSWORD = input['PASSWORD'];
    onConnect();
}
return msg;
*/
ESP8266WebServer server(80);

const int led = LED_BUILTIN;

void action(){
  Serial.println(server.args());
  String input = server.arg(0);
  const size_t bufferSize = JSON_OBJECT_SIZE(2) + 50;
  DynamicJsonBuffer jsonBuffer(bufferSize);
  const char* json = input.c_str();
  Serial.println(json);
  JsonObject& root = jsonBuffer.parseObject(json);
  String type = root["type"].as<String>(); // "SmartSocketv0.0Switching"
  String Switch = root["Switch"].as<String>(); // "switch1"
    if(type == "SmartSocketv0.0Switching"){
        if(Switch == "switch1")
            switching(0);
        else if(Switch == "switch2")
            switching(1);
        else if(Switch == "switch3")
            switching(2);
        else if(Switch == "switch4")
            switching(3);
        else
          server.send(200, "text/plain", "Invalid switch");
        //String result;
        //relay.prettyPrintTo(result);
        server.send(200, "text/plain", "{}");
    }
    else if(type == "SmartSocketv0.0ReadTemperaturesAndHumidity"){
      
    }
    else if(type == "SmartSocketv0.0ConfigMQTTProtocol"){
      
    }    
    else if(type == "SmartSocketv0.0ConfigNetwork"){
      
    } 
    else if(type == "SmartSocketv0.0ConfigureDevice"){
      
    }
    else{
      server.send(200, "text/plain", "Invalid action");
    }
  
}

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  Serial.println("yup working");
  digitalWrite(led, 0);
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  setRelay();
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/action", action);
  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
