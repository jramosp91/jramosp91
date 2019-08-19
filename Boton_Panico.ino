void Boton_Panico() {

 
  if(interruptCounter >= 2 and START >= 1) {
    PANIC=1;
  }
  
  if(PANIC==1){
    
    WiFiClient client = server.available();
  client.setTimeout(30);

 Serial.println("Pase por boton del panico");
    digitalWrite(Aire, LOW);
    digitalWrite(Cale, LOW);
    digitalWrite(Valv, LOW);
    digitalWrite(LEDOK, LOW);
    digitalWrite(LEDFAULT, HIGH);
    Serial.println("");
   

    client.println("HTTP/1.1 200 OK");
    client.println("");                                     //No olvidar esta línea de separación
    client.println("<!DOCTYPE HTML>");
    client.println("<meta charset='UTF-8'>");
    client.println("<html>");
    client.print("<h1>D - PANIC");
    client.print("</h1>");

  }

}
