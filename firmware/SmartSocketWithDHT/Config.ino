void reset(){
  ESP.restart();
}

bool setConfigs(){
  //dev["DeviceId"] = "5b44404ba43e940af037b110";
  dev["MAC"] = WiFi.macAddress();
  dev["PROTOCOL"] = "REST";
  dev["PLATFORM"] = "";
  dev["NAME"] = "mydevice";
  dev["CHIPID"] = ESP.getChipId();

  net["SSID"] = "abcd";
  net["PASSWORD"] = "asdf1234";
  net["STATUS"] = false;
  net["IP"] = "0.0.0.0";

  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }
  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }
  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);
  StaticJsonBuffer<800> jsonBuffer;
  JsonObject& con = jsonBuffer.parseObject(buf.get());
  Serial.println("configs are");
  con.printTo(Serial);
  if (!con.success()) {
    Serial.println("Failed to parse config file");
    return false;
  }
  dev["DeviceId"] = con["DeviceId"].as<String>();
  dev["PROTOCOL"] = con["PROTOCOL"].as<String>();
  dev["NAME"] = con["NAME"].as<String>();
  net["SSID"] = con["SSID"].as<String>();
  net["PASSWORD"] = con["PASSWORD"].as<String>();
  
  net.printTo(Serial);
  Serial.println();
  dev.printTo(Serial);
  Serial.println();  
  return true;
}

void saveConfig(){
  Serial.println("Write to flash memory");
  const size_t configBufferSize = JSON_OBJECT_SIZE(5)+ 100;;
  DynamicJsonBuffer configJsonBuffer(configBufferSize);
  JsonObject& con = configJsonBuffer.createObject();

  con["PROTOCOL"] = "REST";
  con["NAME"] = dev["NAME"];
  con["SSID"] = net["SSID"];
  con["PASSWORD"] = net["PASSWORD"];
  con["DeviceId"] = "5b44404ba43e940af037b110";

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return;
  }
  con.printTo(Serial);
  con.printTo(configFile);  
  restartTicker.once(2,reset);
}

