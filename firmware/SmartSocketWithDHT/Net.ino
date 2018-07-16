void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  if(netConfMode){
    Serial.println("WiFi connected to "+WiFi.SSID());
    net["SSID"] = tempSSID;
    net["PASSWORD"] = tempPWD;
    saveConfig();
    return;
  }
  if(WiFi.getMode()!=3){
    //server.close();
    WiFi.mode(WIFI_STA);
  }
  Serial.println("Connected to Wi-Fi.");
  net["IP"] = WiFi.localIP().toString();
  net["STATUS"] = true;
  digitalWrite(led, 1);
  net.printTo(Serial);
  Serial.println();
  if (MDNS.begin("tiptop")) {
    Serial.println("MDNS responder started");
  }
  relaySetup();
  setupDHT();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  if(netConfMode){
    if(retryCount >0){
      retryCount--;
      return;
    }
    Serial.println("WiFi not connected"+WiFi.SSID());
    netConfMode = false;
    const char* ssid = net["SSID"];
    const char* pwd = net["PASSWORD"];
    WiFi.begin(ssid, pwd);
    return;
  }
  Serial.println("Disconnected from Wi-Fi.");
  if(net["STATUS"]){
    dhtflag= false;
    net["IP"] = WiFi.localIP().toString();
    net["STATUS"] = false;  
    digitalWrite(led, 0);
    net.printTo(Serial);
    WiFi.mode(WIFI_AP_STA);
    int id = dev["CHIPID"];
    String pwd = String(id);
    Serial.println(pwd);
    WiFi.softAP(dev["NAME"] , pwd.c_str());
  } 
}

void connectWifi(){
  netConfMode=false;
  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);
  WiFi.begin(net["SSID"], net["PASSWORD"].as<const char*>());
}


void modifyWiFi(){
  netConfMode = true;
  retryCount = 5;
  String input = server.arg(0);
  const size_t bufferSize = JSON_OBJECT_SIZE(2);
  DynamicJsonBuffer jsonBuffer(bufferSize);
  const char* json = input.c_str();
  Serial.println(json);
  JsonObject& newwifi = jsonBuffer.parseObject(json);
  tempSSID = String(newwifi["SSID"].as<String>());
  tempPWD = newwifi["PASSWORD"].as<String>();
  server.send(200, "text/plain",  "{\"status\":true,\"msg\":\"Device trying to connect....\"}");
  WiFi.disconnect();
  WiFi.begin(newwifi["SSID"], newwifi["PASSWORD"].as<const char*>());
}
