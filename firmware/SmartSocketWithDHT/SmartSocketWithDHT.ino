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
#include <ESP8266httpUpdate.h>
#include <ESP8266HTTPClient.h>

#define DHTPIN            14      
#define DHTTYPE           DHT11     
DHT_Unified dht(DHTPIN, DHTTYPE);

Ticker restartTicker;
Ticker wifiConnect;

bool serverUp = false;
bool netConfMode=false;
bool dhtflag = false;

String tempSSID;
String tempPWD;

HTTPClient http;    //Declare object of class HTTPClient

const size_t netBufferSize = JSON_OBJECT_SIZE(4);
DynamicJsonBuffer netJsonBuffer(netBufferSize);
JsonObject& net = netJsonBuffer.createObject();

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

void makeRequest(bool status, String msg){
  http.begin("https://tiptopgit-pradeepch.c9users.io/device/event");      //Specify request destination
  http.addHeader("Content-Type", "application/json");  //Specify content-type header
  int httpCode;
  if(status)
    httpCode = http.POST("{\"status\":true,\"event\":"+msg+"}");   //Send the request
  else
    httpCode = http.POST("{\"status\":false,\"msg\":\""+msg+"\"}");   //Send the request
  String payload = http.getString();                  //Get the response payload
  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload
  http.end();  //Close connection
}

void auth(){
  http.begin("https://tiptopgit-pradeepch.c9users.io/device/auth");      //Specify request destination
  http.addHeader("Content-Type", "application/json");  //Specify content-type header
  int httpCode;
  String msg;
  dev.printTo(msg);
  httpCode = http.POST("{\"status\":true,\"device\":"+msg+"}");   //Send the request
  String payload = http.getString();                  //Get the response payload
  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload
  http.end();  //Close connection
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

void setup(void){
  Serial.begin(115200);
  SPIFFS.begin();
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  if(setConfigs()){
    connectWifi();
  }
}

void loop(void){
  if(serverUp)
    server.handleClient();
  if(dhtflag){
    // Get temperature event and print its value.
    sensors_event_t tEvent;
    sensors_event_t event;  
    dht.temperature().getEvent(&tEvent);
    dht.humidity().getEvent(&event);
    if (isnan(tEvent.temperature  ) || isnan(event.relative_humidity)) {
      makeRequest(false,"Error reading temperature and humidity!");
    }
    else {
      if(dhtSettings["TempMaxLimit"] == 0 && dhtSettings["TempMinLimit"] ==0 && dhtSettings["HumiMaxLimit"] ==0 && dhtSettings["HumiMinLimit"] ==0){
        dhtReadings["Temperature"] = tEvent.temperature; 
        dhtReadings["Humidity"] = event.relative_humidity; 
        dhtReadings["msg"] = "Device is polling for every "+String(dhtSettings["Frequency"].as<int>())+" seconds.";
        String result;
        dhtReadings.printTo(result);
        makeRequest(true, result);
      }
      else if(dhtSettings["TempMaxLimit"]<=tEvent.temperature || dhtSettings["TempMinLimit"]>=tEvent.temperature || dhtSettings["HumiMaxLimit"]<=event.relative_humidity || dhtSettings["HumiMinLimit"]>=event.relative_humidity){
        dhtReadings["Temperature"] = tEvent.temperature; 
        dhtReadings["Humidity"] = event.relative_humidity; 
        dhtReadings["msg"] = "Threashold limit exceeded.";
        String result;
        dhtReadings.printTo(result);
        makeRequest(true,result);
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
