
void relaySetup(){
  pinMode(pins[0],OUTPUT);
  digitalWrite(pins[0],HIGH);
  pinMode(pins[1],OUTPUT);
  digitalWrite(pins[1],HIGH);
  pinMode(pins[2],OUTPUT);
  digitalWrite(pins[2],HIGH);
  pinMode(pins[3],OUTPUT);
  digitalWrite(pins[3],HIGH);
}

void switching(int i){
  Serial.println(i);
  Serial.println(relay[i].as<bool>());
  if(relay[i]){
    digitalWrite(pins[i],HIGH);
    relay.set(i , false);
  }
  else{
    digitalWrite(pins[i],LOW);
    relay.set(i , true);
  }
  Serial.println(relay[i].as<bool>());
  String result;
  relay.prettyPrintTo(result);
  server.send(200, "text/plain", result);
}

