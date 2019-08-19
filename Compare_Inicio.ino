void CompareInicio() 
{

  if (interruptCounter == 0 or finish == 1)   {

       finish = 0;
    INICIO = 1;
  }

  if (INICIO == 1)    {

    Serial.println("ESTOY EN INICIO");
    WiFiClient client = server.available();
    client.setTimeout(30);
    peticion = client.readStringUntil('\r');
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

      START=1;
      inicie = 1;
      interruptCounter = 1;
      client.println("HTTP/1.1 200 OK");
      client.println("");                                     //No olvidar esta línea de separación
      client.println("<!DOCTYPE HTML>");
      client.println("<meta charset='UTF-8'>");
      client.println("<html>");
      client.print("<h1> B - EMPEZO EL JUEGO");
      client.print("  </h1>");
      client.stop();
      memset(estado_buffer, 0, sizeof(estado_buffer));
      INICIO = 0;
    }

    if (inicie == 0)    {

      client.println("HTTP/1.1 200 OK");
      client.println("");                                     //No olvidar esta línea de separación
      client.println("<!DOCTYPE HTML>");
      client.println("<meta charset='UTF-8'>");
      client.println("<html>");
      client.print("<h1> A - ESPERANDO INICIO");
      client.print("  </h1>");
      client.stop();
      memset(estado_buffer, 0, sizeof(estado_buffer));
    }

  }
}
