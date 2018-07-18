void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  if(netConfMode){
    Serial.println("WiFi connected to "+WiFi.SSID());
    net["SSID"] = tempSSID;
    net["PASSWORD"] = tempPWD;
    saveConfig();
    return;
  }
  WiFi.mode(WIFI_STA);
  Serial.println("Connected to Wi-Fi.");
  net["IP"] = WiFi.localIP().toString();
  net["STATUS"] = true;
  digitalWrite(led, 1);
  net.printTo(Serial);
  Serial.println();
  if (MDNS.begin("tiptop")) {
    Serial.println("MDNS responder started");
  }
  if(serverUp){
    //server.close();
    if(dev["PROTOCOL"] == "REST"){
      server.on("/action" , action);
    }
    else if (dev["PROTOCOL"] = "MQTT"){
      serverUp = false;
      server.close();
    }
    else{
      Serial.println("Unknown protocol");
    }
  }
  else{
    if(dev["PROTOCOL"] == "REST"){
      server.on("/action", action);
      server.onNotFound(handleNotFound);
      server.begin();
      Serial.println("HTTP server started");
      serverUp = true;
    }
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
    if(serverUp){
        server.on("/action" , localConfig);
    }
    else{
        server.on("/action", localConfig);
        server.onNotFound(handleNotFound);
        server.begin();
        Serial.println("HTTP server started");
        serverUp = true;
    }
  } 
  else{
    if(!serverUp){
        server.on("/action", localConfig);
        server.onNotFound(handleNotFound);
        server.begin();
        Serial.println("HTTP server started");
        serverUp = true;
    }
  }
}

void connectWifi(){
  netConfMode=false;
  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);
  WiFi.begin(net["SSID"], net["PASSWORD"].as<const char*>());
}

void connectTo(){
   WiFi.disconnect();
  WiFi.begin(tempSSID.c_str(), tempPWD.c_str());
}

void modifyWiFi(JsonObject& newwifi){
  netConfMode = true;
  retryCount = 5;
  newwifi.printTo(Serial);
  tempSSID = newwifi["SSID"].as<String>();
  tempPWD = newwifi["PASSWORD"].as<String>();  
  wifiConnect.once(5,connectTo);
}
