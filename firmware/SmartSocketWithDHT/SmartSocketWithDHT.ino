#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <Ticker.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN            14      
#define DHTTYPE           DHT11     
DHT_Unified dht(DHTPIN, DHTTYPE);

Ticker restartTicker;
Ticker wifiConnect;

bool netConfMode=false;
bool dhtflag = false;
String tempSSID;
String tempPWD;

const size_t netBufferSize = JSON_OBJECT_SIZE(4);
DynamicJsonBuffer netJsonBuffer(netBufferSize);
JsonObject& net = netJsonBuffer.createObject();

const size_t tempNetBufferSize = JSON_OBJECT_SIZE(3);
DynamicJsonBuffer tempNetJsonBuffer(tempNetBufferSize);
JsonObject& tempNet = tempNetJsonBuffer.createObject();

const size_t relayBufferSize = JSON_OBJECT_SIZE(5);
DynamicJsonBuffer relayJsonBuffer(relayBufferSize);
const char* relayJson = "{\"switch1\":false,\"switch2\":false,\"switch3\":false,\"switch4\":false}";
JsonObject& relay = relayJsonBuffer.parseObject(relayJson);

const size_t pinsBufferSize = JSON_OBJECT_SIZE(4) + 50;
DynamicJsonBuffer pinsJsonBuffer(pinsBufferSize);
const char* pinsJson = "{\"switch1\":5,\"switch2\":4,\"switch3\":0,\"switch4\":2}";
JsonObject& pins = pinsJsonBuffer.parseObject(pinsJson);

const size_t dhtSettingsBufferSize = JSON_OBJECT_SIZE(5);
DynamicJsonBuffer dhtSettingsJsonBuffer(dhtSettingsBufferSize);
JsonObject& dhtSettings = dhtSettingsJsonBuffer.createObject();

const size_t dhtReadingsBufferSize = JSON_OBJECT_SIZE(2);
DynamicJsonBuffer dhtReadingsJsonBuffer(dhtReadingsBufferSize);
JsonObject& dhtReadings = dhtReadingsJsonBuffer.createObject();

const size_t mqttBufferSize = JSON_OBJECT_SIZE(5);
DynamicJsonBuffer mqttJsonBuffer(mqttBufferSize);
JsonObject& mqttConfigs = mqttJsonBuffer.createObject();

const size_t devBufferSize = JSON_OBJECT_SIZE(4);
DynamicJsonBuffer devJsonBuffer(devBufferSize);
JsonObject& dev = devJsonBuffer.createObject();

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;

int retryCount;

ESP8266WebServer server(80);

const int led = LED_BUILTIN;

void positiveActionResponcer(String result){
  server.send(200, "text/plain", "{\"status\":true,\"response\":"+result+"}");
}

void negativeActionResponcer(String msg){
  server.send(200, "text/plain", "{\"status\":false,\"msg\":\""+msg+"\"}");
}

void actionAvalyzer(String input){
  const size_t bufferSize = JSON_OBJECT_SIZE(8);
  DynamicJsonBuffer jsonBuffer(bufferSize);
  const char* json = input.c_str();
  Serial.println(json);
  JsonObject& root = jsonBuffer.parseObject(json);
  String type = root["type"].as<String>(); // "SmartSocketv0.0Switching"
    if(type == "Switching"){
      switching(root);
    }
    /*else if(type == "SmartSocketv0.0ConfigProtocol"){
      
    }*/    
    else if(type == "ConfigNetwork"){
      root["msg"] = "Device is trying to connect...";
      String result;
      root.printTo(result);
      positiveActionResponcer(result);
      modifyWiFi(root);
    } 
    else if(type == "ConfigureDHT"){
      updateSettings(root);
    }
    else{
     negativeActionResponcer("Invalid Action");
    }
}

void action(){
  actionAvalyzer(server.arg(0));
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
  Serial.begin(115200);
  SPIFFS.begin();
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  if(setConfigs()){
    connectWifi();
  }
  server.on("/action", action);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
  if(dhtflag){
    // Get temperature event and print its value.
    sensors_event_t event;  
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
      Serial.println("Error reading temperature!");
    }
    else {
      Serial.print("Temperature: ");
      Serial.print(event.temperature);
      Serial.println(" *C");
      if(dhtSettings["TempMaxLimit"] == 0 && dhtSettings["TempMinLimit"] ==0){
        dhtReadings["Temperature"] = event.temperature; 
        dhtReadings.printTo(Serial);
        
      }
      else if(dhtSettings["TempMaxLimit"]<=event.temperature || dhtSettings["TempMinLimit"]>=event.temperature ){
        dhtReadings["Temperature"] = event.temperature; 
        dhtReadings.printTo(Serial);
      }
    }
    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
      Serial.println("Error reading humidity!");
    }
    else {
      Serial.print("Humidity: ");
      Serial.print(event.relative_humidity);
      Serial.println("%");
      if(dhtSettings["HumiMaxLimit"] ==0&& dhtSettings["HumiMinLimit"] ==0){
        dhtReadings["Humidity"] = event.relative_humidity; 
        dhtReadings.printTo(Serial);
      }
      else if(dhtSettings["HumiMaxLimit"]<=event.relative_humidity || dhtSettings["HumiMinLimit"]>=event.relative_humidity ){
        dhtReadings["Humidity"] = event.relative_humidity; 
        dhtReadings.printTo(Serial);
      }
    }
    int fre = dhtSettings["Frequency"].as<int>();
    if(fre<1 || fre>3600)
      dhtflag=false;
    else{
      delay(fre*1000);
      Serial.println(fre);
    }
  }
}

