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

void positiveActionResponcer(String result){
  server.send(200, "text/plain", "{\"status\":true,\"response\":"+result+"}");
}

void negativeActionResponcer(String msg){
  server.send(200, "text/plain", "{\"status\":false,\"msg\":\""+msg+"\"}");
}

void localConfig(){
  String message = "Local config html page";
  /*message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";*/
  
  server.send(404, "text/plain", message);
}

