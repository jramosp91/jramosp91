void CompareInicio()
{

  if (interruptCounter == 0) {
    Estado = "A";
    Serial.println("ESTOY EN INICIO");
    WiFiClient client = server.available();           //Admite conexion
    client.setTimeout(200);
    if (client) {


      if (client.connected()) {

        peticion = client.readStringUntil('\r');
        client.flush();
        Serial.println(peticion);

        digitalWrite(LEDOK, HIGH);


        if (peticion.indexOf("/req/") != -1) {
          Serial.println("Parsee los datos en inicio");
          int index = peticion.indexOf('=');
          estado = peticion.substring(9, index);
          estado.toCharArray(estado_buffer, 10);
          tiempo = peticion.substring(index + 1).toInt();
          Serial.println(estado_buffer);
          Serial.println(tiempo);
          Serial.println(millis());
          delay (1);



          if (strcmp(estado_buffer, ini) == 0 or interruptCounter == 1) {

            Estado = "B";
            comando = 1;
            START = 1;
            inicie = 1;
            interruptCounter = 1;
            INICIO = 0;
            JSON();

            client.println("HTTP/1.1 200 OK");
            Serial.println("ENVIE DATO HTTP");
            client.println("Content-Type: application/json");
            client.println("");
            client.println(OUT);
            Serial.println(OUT);
            delay(2);
            OUT = "";

          }


          else if (strcmp(estado_buffer, sta) == 0) {
            comando = 1;
            JSON();
            client.println("HTTP/1.1 200 OK");
            Serial.println("ENVIE DATO HTTP");
            client.println("Content-Type: application/json");
            client.println(OUT);
            delay(2);
            OUT = "";



          }

          else if (interruptCounter == 0) {
            comando = 0;
            client.println("HTTP/1.1 200 OK");
            Serial.println("ENVIE DATO HTTP");
            client.println("Content-Type: application/json");
            client.println(OUT);
            delay(2);
            OUT = "";
          }
        }
      }
    }
  }
}
