void WIFI_SETUP_STA() {

  Serial.println("Starting STATION");
  WiFi.mode(WIFI_STA);                                                 //Modo de Operacion AP para Access Point/ STA para Station
  WiFi.softAP(ssidAP, passwordAP);                                        //Red con clave, en el canal 1 y visible
  WiFi.begin(ssidSTA, passwordSTA);
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {


    Serial.println("Connection Ok");
    Serial.println(WiFi.localIP());
    ESPhttpUpdate.rebootOnUpdate (false);
    t_httpUpdate_return ret = ESPhttpUpdate.update("https://jramosp91.github.io/HeartFail.ino.INTERATICA19002004.bin", "", "70 0B 6F 62 4F 41 EB 1A 42 3F 73 5A DA 96 98 2D 7F 2B 75 6F");

    Serial.println("llegue al switch");
    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s",  ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        digitalWrite(LEDOK, HIGH);
        digitalWrite(LEDFAULT, HIGH);
        delay(10000);
        digitalWrite(LEDOK, LOW);
        digitalWrite(LEDFAULT, LOW);
        break;

      case HTTP_UPDATE_OK:
        Serial.println("UPDATE OK");
        digitalWrite(LEDOK, HIGH);
        delay(50);
        digitalWrite(LEDFAULT, HIGH);
        digitalWrite(LEDOK, LOW);
        delay(150);
        digitalWrite(LEDFAULT, LOW);
        digitalWrite(LEDOK, HIGH);
        delay(1000);
        ESP.restart();
        break;

    }
  }
}
