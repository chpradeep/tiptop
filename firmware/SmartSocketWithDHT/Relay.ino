void setRelay(){
  const size_t relayBufferSize = JSON_ARRAY_SIZE(4) + 30;
  DynamicJsonBuffer relayJsonBuffer(relayBufferSize);
  const char* relayJson = "[true,true,false,false]";
  relay = relayJsonBuffer.parseArray(relayJson);
}

void switching(int i){
  relay.add(i , !relay[i]);
}

