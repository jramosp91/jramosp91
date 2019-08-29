void Acciones()
{

  if (inicie >= 1) {                                                                     // Condicional que limita entrar a esta seccion si no se inicio el juego

    Blink_OK_SLOW();


    //Chequeo de cliente

    WiFiClient client = server.available();                                               //Admite conexion
    client.setTimeout(10);                                                                //Timeout del cliente en ms
    peticion = client.readStringUntil('\r');                                              //Lectura del String
    Serial.println(peticion);
    client.flush();                                                                       //Flush de datos

    if (peticion.indexOf("/req/") != -1)      {                                          //Condiciono que la instruccion lleve un /req/ para evitar ruidos y falsos positivos


      //Parseo de datos

      Serial.println("Parsee los datos en Acciones");
      int index = peticion.indexOf('=');                                                  //INDEX para separar el String en CHAR e INT
      estado = peticion.substring(9, index);                                              //Separacion del STRING que quiero que sea CHAR
      estado.toCharArray(estado_buffer, 10);                                              //Manda el Sring a un buffer del tipo CHAR
      tiempo = peticion.substring(index + 1).toInt();                                     //Convierte despues del index en INT
      Serial.println(estado_buffer);
      Serial.println(tiempo);
      Serial.println(millis());
      delay (1);

    }

    //Bomba
    if (strcmp(estado_buffer, pump) == 0)                                               //compara que el buffer sea igual a la constante pump (para la bomba)

    {
      Serial.println("Pase por Encendido Bomba");
      tbomb = tiempo * 1000;                                                            //Conversion del tiempo en segundos a milisegundos
      digitalWrite(Aire, HIGH);                                                         //Enciende Bomba
      Tref_bomba = millis();                                                            //Activa le referencia del tiempo
      Serial.println(Tref_bomba);
      Serial.println ("Encendiendo Bomba");
      Serial.println (tbomb);
      treq1 = tbomb + Tref_bomba;                                                      //Suma la referencia del tiempo + el tiempo que se quiere tener encendido la bomba
      Serial.print("TIEMPO DE ENCENDIDO: ");
      Serial.println(treq1);
      tiempo = 0;                                                                      //Vacio la variable tiempo
      Spump = 1;                                                                       //Activo FLAG para encendido de bomba
      client.println("HTTP/1.1 200 OK");                                               //Devuelvo un codigo HTTP 200 OK al cliente
      client.stop();
      memset(estado_buffer, 0, sizeof(estado_buffer));                                 //Limpio buffer

    }
    Serial.println("Antes del IF");
    Serial.println(millis()) ;
      Serial.println(treq2 *2 );
      
    if (treq2 * 2 <= millis()) {
      
    Serial.println("Despues del IF");
      Serial.println(millis()) ;
      Serial.println(treq2 *2 );
      
      if (strcmp(estado_buffer, pelt) == 0 and tiempo <= 10)                             //compara que el buffer sea igual a la constante pelt (para el peltier)

      {

        Serial.println("Pase por Encendido Peltier");
        tpelt = tiempo * 1000;
        Tref_pelt = millis();
        digitalWrite(Cale, HIGH);                                                        //Enciende Peltier
        Serial.println ("Encendiendo Peltier");
        Serial.println (tiempo);
        treq2 = tpelt + Tref_pelt;                                                      //Suma la referencia del tiempo + el tiempo que se quiere tener encendido el peltier
        tiempo = 0;
        Spelt = 1;                                                                      //Activo FLAG para encendido de Peltier
        client.println("HTTP/1.1 200 OK");
        client.stop();
        memset(estado_buffer, 0, sizeof(estado_buffer));

      }
      ///Peltier Tempo
      if (strcmp(estado_buffer, pelt) == 0 and tiempo > 7) {                            //Limita que no se pueda poner mas de XX Segundos el peltier
        CDOn = millis();
        Serial.println("Pase por Encendido Peltier con Limite");
        Tref_pelt = millis();
        digitalWrite(Cale, HIGH);                                                       //Enciendo Peltier
        Serial.println("No se puede mas de 10 Segundos");
        Serial.println("Encendiendo Peltier X Segundos");
        treq2 = (7 * 1000) + Tref_pelt;
        tiempo = 0;
        Spelt = 1;
        client.println("HTTP/1.1 200 OK");
        client.stop();
        memset(estado_buffer, 0, sizeof(estado_buffer));

      }
    }
    //Valv
    if (strcmp(estado_buffer, valv) == 0 and tiempo == 1) {                            //compara que el buffer sea igual a la constante valv (para la valvula) y que TIEMPO sea 1 para ON

      digitalWrite(Valv, HIGH);
      Serial.println("Enciendo Valvula");
      Svalv = 1;                                                                       //Flag de Valvula Encendida
      client.println("HTTP/1.1 200 OK");
      client.stop();
      memset(estado_buffer, 0, sizeof(estado_buffer));

    }

    if (strcmp(estado_buffer, valv) == 0 and tiempo == 0) {                             // compara que el buffer sea igual a la constante valv (para la valvula) y que TIEMPO sea 0 para OFF
      digitalWrite(Valv, LOW);
      Serial.println("Apagado Valvula");
      Svalv = 0;                                                                        //Flag de Valvula Apagada
      client.println("HTTP/1.1 200 OK");
      client.stop();
      memset(estado_buffer, 0, sizeof(estado_buffer));
    }

    if (strcmp(estado_buffer, END) == 0) {                             //Compara que el buffer sea igual a la constante fin para dar por finalizado el juego y apagar todo.

      digitalWrite(Aire, LOW);                                                        //Apago Bomba
      digitalWrite(Cale, LOW);                                                        //Apago peltier
      digitalWrite(Valv, LOW);                                                        //Abro valvula
      Serial.println("FIN DEL JUEGO");

      interruptCounter = 0;                                                           //Borro conteo del boton START/STOP
      inicie = 0;                                                                     //Vacio Flag de que esta iniciado
      START = 0;                                                                      //Vacio Flag que el juego esta corriendo
      memset(estado_buffer, 0, sizeof(estado_buffer));                                //Vacio Buffer

      ////MENSAJE HTTP

      client.println("HTTP/1.1 200 OK");
      client.println("");                                     //No olvidar esta línea de separación
      client.println("<!DOCTYPE HTML>");
      client.println("<meta charset='UTF-8'>");
      client.println("<html>");
      client.print("<h1>C -FIN DEL JUEGO");
      client.print("</h1>");
      client.print("<h1></hl>");

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
      client.print(IND);
      client.print("</h1>");
      memset(estado_buffer, 0, sizeof(estado_buffer));


    }
  }

}
