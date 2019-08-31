void WIFI_SETUP_AP() {

WiFi.mode(WIFI_AP);           //Only Access point
  WiFi.softAP(ssidAP, passwordAP);  //Start HOTspot removing password will disable security

  IPAddress local_ip(192, 168, 1, 1);                                    //Modifica la dirección IP en modo AP de la caja
  IPAddress gateway(192, 168, 1, 1);                                     // Gateway de AP la caja
  IPAddress subnet(255, 255, 255, 0);                                    // Subnet del AP la caja
  WiFi.softAPConfig(local_ip, gateway, subnet);
  IPAddress myIP = WiFi.softAPIP(); //Get IP address
  Serial.print("HotSpt IP:");
  Serial.println(myIP);

  server.on("/", handleRoot);      //Which routine to handle at root location
  server.on("/req/sta", handleStatus);      //Which routine to handle at root location
  server.on("/req/pump", handlePump);      //Which routine to handle at root location
  server.on("/req/valv", handleValve);      //Which routine to handle at root location
  server.on("/req/pelt", handlePeltier);      //Which routine to handle at root location
  server.on("/req/end", handleFin);      //Which routine to handle at root location
  server.on("/req/ini", handleInicio);      //Which routine to handle at root location

  server.begin();                  //Start server
  Serial.println("HTTP server started");
}
