
void setupDHT(){
  // Initialize device.
  dht.begin();
  Serial.println("DHT11 Unified Sensor");
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Temperature");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");  
  Serial.println("------------------------------------");
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Humidity");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");  
  Serial.println("------------------------------------");
  
  dhtSettings["Frequency"] = 5;
  dhtSettings["TempMaxLimit"] = 0;
  dhtSettings["TempMinLimit"] = 0;
  dhtSettings["HumiMaxLimit"] = 0;
  dhtSettings["HuniMinLimit"] = 0;
  dhtflag= true;
}

void updateSettings(){
  String input = server.arg(0);
  const size_t bufferSize = JSON_OBJECT_SIZE(5);
  DynamicJsonBuffer jsonBuffer(bufferSize);
  const char* json = input.c_str();
  Serial.println(json);
  JsonObject& newSettings = jsonBuffer.parseObject(json);
  dhtSettings["Frequency"] = newSettings["Frequency"].as<int>();
  dhtSettings["TempMaxLimit"] = newSettings["TempMaxLimit"].as<float>();
  dhtSettings["TempMinLimit"] = newSettings["TempMinLimit"].as<float>();
  dhtSettings["HumiMaxLimit"] = newSettings["HumiMaxLimit"].as<float>();
  dhtSettings["HuniMinLimit"] = newSettings["HuniMinLimit"].as<float>();
  server.send(200, "text/plain",  "{\"status\":true,\"msg\":\"Settings updated\"}");
  dhtflag = true;
}
