void CompareInicio()
{

  if (interruptCounter == 0) {

    Serial.println("ESTOY EN INICIO");
    WiFiClient client = server.available();
    client.setTimeout(10);
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

    }

    if (strcmp(estado_buffer, ini) == 0 or interruptCounter == 1) {

      START = 1;
      inicie = 1;
      interruptCounter = 1;
      INICIO = 0;
      client.println("HTTP/1.1 200 OK");
      client.println("");                                     //No olvidar esta línea de separación
      client.println("<!DOCTYPE HTML>");
      client.println("<meta charset='UTF-8'>");
      client.println("<html>");
      client.print("<h1> B - EMPEZO EL JUEGO");
      client.print("  </h1>");
      client.flush();
      memset(estado_buffer, 0, sizeof(estado_buffer));

      IND = "B";
    }

    if (inicie == 0 and strcmp(estado_buffer, sta) >= 1)    {

      client.println("HTTP/1.1 200 OK");
      client.println("");                                     //No olvidar esta línea de separación
      client.println("<!DOCTYPE HTML>");
      client.println("<meta charset='UTF-8'>");
      client.println("<html>");
      client.print("<h1> A - ESPERANDO INICIO");
      client.print("  </h1>");


      memset(estado_buffer, 0, sizeof(estado_buffer));
    }
    if (strcmp(estado_buffer, sta) == 0) {

      client.println("HTTP/1.1 200 OK");
      client.println("");                                     //No olvidar esta línea de separación
      client.println("<!DOCTYPE HTML>");
      client.println("<meta charset='UTF-8'>");
      client.println("<html>");
      client.print("<h1>S-STATUS");
      client.print("</h1>");
      client.print("<hl>");
      client.print("PELTIER: ");
      client.println(Spelt);
      client.print("BOMBA: ");
      client.println(Spump);
      client.print("VALVULA: ");
      client.println(Svalv);
      client.print("STATUS ACTUAL DEL JUEGO: ");
      client.print("A");
      client.print("</h1>");
      memset(estado_buffer, 0, sizeof(estado_buffer));

    }
  }
}
