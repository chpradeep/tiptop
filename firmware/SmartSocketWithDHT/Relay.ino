
void relaySetup(){
  pinMode(pins["switch1"],OUTPUT);
  digitalWrite(pins["switch1"],LOW);
  pinMode(pins["switch2"],OUTPUT);
  digitalWrite(pins["switch2"],LOW);
  pinMode(pins["switch3"],OUTPUT);
  digitalWrite(pins["switch3"],LOW);
  pinMode(pins["switch4"],OUTPUT);
  digitalWrite(pins["switch4"],LOW);
}

void switching(JsonObject& root){
  String Switch = root["Switch"].as<String>(); // "switch1"
  if (relay.containsKey(Switch)){
    if(relay[Switch]){
      digitalWrite(pins[Switch],LOW);
      relay[Switch] = false;
      relay["msg"] = Switch+" got off";
    }
    else{
      digitalWrite(pins[Switch],HIGH);
      relay[Switch] =  true;
      relay["msg"] = Switch+" got on";
    }
    String result;
    relay.prettyPrintTo(result);
    positiveActionResponcer(result);
  }
  else{
    negativeActionResponcer("Invalid switch");
    return;
  }
}

