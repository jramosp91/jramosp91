void Boton_Panico() {


  if (interruptCounter >= 2 and START >= 1) {
    PANIC = 1;
  }

  if (PANIC == 1) {
    Estado = 'D';
    inicie = 0;
    START = 0;

    WiFiClient client = server.available();
    client.setTimeout(30);

    Serial.println("Pase por boton del panico");
    digitalWrite(Aire, LOW);
    digitalWrite(Cale, LOW);
    digitalWrite(Valv, LOW);
    digitalWrite(LEDOK, LOW);
    digitalWrite(LEDFAULT, HIGH);
    Serial.println("");
    JSON();
    client.println("HTTP/1.1 200 OK");
    Serial.println("ENVIE DATO HTTP");
    client.println("Content-Type: application/json");
    client.println("");
    client.println(OUT);
    delay(5);
    client.stop();
    memset(estado_buffer, 0, sizeof(estado_buffer));
    OUT = "";


  }

}
