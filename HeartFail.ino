#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <WiFiUDP.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <ESP8266httpUpdate.h>
#define DEBUG_HTTP_UPDATE
#define DEBUG_ESP_HTTP_UPDATE

///Pines Digitales

const int Aire = D5;                                     // Bomba de aire
const int Cale = D8;                                    // Peltier
const int Valv = D6;                                   // Electrov
const int RESET = D4;                                 // Boton RESET
const byte Panic = D7;                                //Boton del Panico
const int LEDOK = D3;                                 // LED OK
const int LEDFAULT = D1 ;                             // LED FALLA
const int TEMPSENS = D2;                              // SENSOR DE TEMP

//Strings de datos

String peticion;                                    // PETICION DEL CLIENTE
String estado;                                      // DATO QUE ENVIA EL UNITY
int tiempo;                                        // TIEMPO QUE ENVIA EL UNITY
String ret;                                        // SERVER HTTP
String IND;                                       //String status
String OUT;                                       //String de Ouput JSON
String Estado;                                  //String de Stado
const int VERSION = 0.9114;                             //VERSION

//interrupciones

volatile byte interruptCounter = 0;               // contador de interrupcion

//flags

int inicie = 0;                                   // FLAG INICIALIZACION
int finish = 0;                                   // FLAG FINAL DEL JUEGO
int Spump = 0;                                    // FLAG ESTADO BOMBA, 0 OFF 1 ON
int Spelt = 0;                                    // FLAG ESTADO PELT, 0 OFF 1 ON
int Svalv = 0;                                    // FLAG ESTADO VALV, 0 OFF 1 ON
int RST = 0;                                      // FLAG RESET
int PANIC = 0;                                    //FLAG BOTON PANICO
int comando = 0;                                  //FLAG QUE SE INGRESO EL COMANDO CORRECTAMENTE 1 SI 0 NO
int DEBUGMODE = 1 ;                                //MODO DEBUG 1 ON 0 OFF


//Temporizadores

unsigned long Tref_bomba = 0;                     // T REF PARA BOMBA
unsigned long Tref_pelt = 0;                      //T REF PARA PELTIER
unsigned long treq1 = 0;                          // BUFFER DE TIEMPO BOMBA
unsigned long treq2 = 0;                          // BUFFER DE TIEMPO PELTIER
long tbomb = 0;                                   //TIEMPO DE ENCENDIDO BOMBA
long tpelt = 0;                                   // TIEMPO ENCENDIDO BOMBA
unsigned long treset = 0;                         //TIEMPO REF RESET
unsigned long tpress = 2000;                      //TIEMPO PARA APRETAR Y RESET EN ms

//COOLDOWN

unsigned long CDOn = 0;                           // CD TEMP OFF
unsigned long CDLim = 0;                          // CD TEMP OFF
const int CDMult = 2;                             //Multiplicador CD

//blink slow

unsigned long blinkS = 0;                       //REFERENCIA INICIAL BLINK
const long BLINK = 500;                        // TEMPORIZADO BLINK LED STATUS
unsigned long blinkE = 0;                       ///REFERENCIA DE TIEMPO PARA BLINK

//blink fast

unsigned long blinkFS = 0;                      // REFERENCIA INICIAL BLINK FAST
const long BLINKF = 50;                        // TEMPORIZADO INICIAL BLINK FAST
unsigned long blinkFE = 0;                     // REFENCIA DE TIEMPO PARA BLINK FAST


//Temperatura maxima

int temp_max = 150 ;                                  // INDICE DE TEMP MAX

//debouncer

const int timeThreshold = 400;                        // REF PARA DEBOUNCER
long starttime = 0;                                  // TIEMPO INICIO DEBOUNCE


//////////////////////////
/////////WIFI AP//////////
//////////////////////////
const char* ssidAP = "INTERATICA19002004";                 //Definimos la SSDI de nuestro servidor WiFi -nombre de red-
const char* passwordAP = "interatica-4DVR";             //Definimos la contraseña de nuestro servidor
ESP8266WebServer server(80);                            //Definimos el puerto de comunicaciones

//////////////////////
//////WIFI STA///////
////////////////////

const char ssidSTA[] = "DEBUG19002004";
const char passwordSTA[] = "interatica-4DVR";


//Funciones

void Temp();                                      //Proteccion de Temperatura por Sensor
void Boton_Panico();                              //Boton de Start/STOP
void Bomba_Apagado();                             //Funcion Apagado Bomba
void ApagadoPeltier();                            //Funcion Apagado Peltier
void Reset();                                     // Funcion Pin RESET
void PINES();                                     // Declaracion de pines
void Blink_FAULT_FAST();                          //Funcion de blink rapido para led rojo
void Blink_Fault_Slow ();                         //Funcion de blink lento para led rojo
void Blink_OK_SLOW();                             // Funcion de blink rapido para led amarillo
void Blink_OK_FAST();                             //Funcion de blink lento para led amarillo
void JSON();
void WIFI_SETUP_AP();
void WIFI_SETUP_STA();


//===============================================================
//                  SETUP
//===============================================================
void setup(void) {
  Serial.begin(9600);
  Serial.println("");

  //Declaracion de Pines
  Serial.println("Pines() ->");
  PINES();

  WIFI_SETUP_STA();

  WIFI_SETUP_AP();

  millis();
}


//==============================================================
//     This rutine is exicuted when you open its IP in browser
//==============================================================
void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266!");
}

//==============================================================
//     This rutine is exicuted when you open its IP in browser
//==============================================================
void handleStatus() {
  //  String message = "";
  //  for (int i = 0; i < server.args(); i++) {
  //    message += "Arg nº" + (String)i + " –> ";
  //    message += server.argName(i) + ": ";
  //    message += server.arg(i) + "\n";
  //  }
  //  Serial.println(message);

  // Chequear si hay al menos un arg
  //  tiempo = server.arg(0).toInt();
  //  Serial.println("Tiempo: " + (String)tiempo);

  JSON();
  server.send(200, "text/plain", OUT);
}

void handlePump() {
  if (inicie >= 1) {

    tiempo = server.arg(0).toInt();
    Serial.println("Pump con tiempo = " + (String)tiempo);
    comando = 1;
    Serial.println("Pase por Encendido Bomba");
    tbomb = tiempo * 1000;                                                            //Conversion del tiempo en segundos a milisegundos
    digitalWrite(Aire, HIGH);                                                         //Enciende Bomba
    digitalWrite(Valv, HIGH);
    Tref_bomba = millis();                                                            //Activa le referencia del tiempo
    Serial.println(Tref_bomba);
    Serial.println ("Encendiendo Bomba");
    Serial.println (tbomb);
    treq1 = tbomb + Tref_bomba;                                                      //Suma la referencia del tiempo + el tiempo que se quiere tener encendido la bomba
    Serial.print("TIEMPO DE ENCENDIDO: ");
    Serial.println(treq1);
    tiempo = 0;                                                                      //Vacio la variable tiempo
    Spump = 1;                                                                       //Activo FLAG para encendido de bomba
    Svalv = 1;
    JSON();
    server.send(200, "text/plain", OUT);
  }
  else {
    comando = 0;
    JSON();
    server.send(200, "text/plain", OUT);
  }
}

void handlePeltier() {
  if (inicie >= 1) {
    tiempo = server.arg(0).toInt();

    Serial.println("Se ingreso:");
    Serial.println(tiempo);
    Serial.println("TIEMPO DE ENCENDIDO:");
    Serial.println(treq2 / 1000);

    Serial.println("Pasaron");
    Serial.println(millis() / 1000);

    Serial.println("Tiempo de cooldown");
    Serial.println ((treq2*2) / 1000);
    Serial.println("FALTA PARA APAGARSE:");
    Serial.println(((treq2*2)-treq2)/1000);
    
    if (millis() - Tref_pelt > ((treq2 - Tref_pelt) * 2)) {

      if (tiempo > 7) tiempo = 7;

      comando = 1;
      Serial.println("Pase por Encendido Peltier");
      tpelt = tiempo * 1000;
      Tref_pelt = millis();
      digitalWrite(Cale, HIGH);                                                        //Enciende Peltier
      Serial.println ("Encendiendo Peltier");
      Serial.println (tiempo);
      treq2 = tpelt + Tref_pelt;                                                      //Suma la referencia del tiempo + el tiempo que se quiere tener encendido el peltier
      tiempo = 0;
      Spelt = 1;                                                                      //Activo FLAG para encendido de Peltier
      JSON();
      server.send(200, "text/plain", OUT);
    }  else {
      comando = 0;
      JSON();
      server.send(200, "text/plain", OUT);
    }
  }
}


void handleValve() {

  if (inicie >= 1) {
    tiempo = server.arg(0).toInt();

    if (tiempo == 1) {                            //compara que el buffer sea igual a la constante valv (para la valvula) y que TIEMPO sea 1 para ON

      comando = 1;
      digitalWrite(Valv, HIGH);
      Serial.println("Enciendo Valvula");
      Svalv = 1;                                                                       //Flag de Valvula Encendida
      JSON();
      server.send(200, "text/plain", OUT);

    }

    else if (tiempo == 0) {                            // compara que el buffer sea igual a la constante valv (para la valvula) y que TIEMPO sea 0 para OFF
      comando = 1;
      digitalWrite(Valv, LOW);
      Serial.println("Apagado Valvula");
      Svalv = 0;                                                                        //Flag de Valvula Apagada
      JSON();
      server.send(200, "text/plain", OUT);
    }

  }

  else {
    comando = 0;
    JSON();
    server.send(200, "text/plain", OUT);
  }
}


void handleInicio() {
 
  if (PANIC <  1) {
    Estado = "B";
    comando = 1;
    inicie = 1;
    interruptCounter = 1;
    JSON();
    server.send(200, "text/plain", OUT);

  }

  else {
    JSON();
    server.send(200, "text/plain", OUT);
  }

}

void handleFin() {

  if (inicie >= 1 and PANIC < 1) {
    Estado = "A";
    comando = 1;
    digitalWrite(Aire, LOW);                                                        //Apago Bomba
    digitalWrite(Cale, LOW);                                                        //Apago peltier
    digitalWrite(Valv, LOW);                                                        //Abro valvula
    Serial.println("FIN DEL JUEGO");
    interruptCounter = 0;                                                           //Borro conteo del boton START/STOP
    inicie = 0;                                                                     //Vacio Flag de que esta iniciado
    JSON();
    server.send(200, "text/plain", OUT);

  }
}

//////////////////////////////////////
///////////CONTADOR INTERRUPCION//////
//////////////////////////////////////

void handleInterrupt() {
  if (millis() - starttime > timeThreshold)
  {
    starttime = millis();
    interruptCounter++;
    Serial.println(interruptCounter);
  }


}

//===============================================================
//                     LOOP
//===============================================================
void loop(void) {
  server.handleClient();          //Handle client requests

  blinkS = millis();
  blinkFS = millis();

  ArduinoOTA.handle();                                                              //invoca si hay que flashear online
  //  return;


  Reset();                                                                      // Funcion del boton reset
  Boton_Panico();                                                               //Boton de Start/STOP
  Bomba_Apagado();                                                              //Funcion Apagado Bomba
  ApagadoPeltier();                                                             //Funcion Apagado Peltier
  Blink_FAULT_FAST();                                                           // Funcion de blinking rapido para led rojo
  Blink_OK_FAST();                                                              //Funcion de blinking rapido para led amarillo

  if (interruptCounter == 0) {
    Estado = "A";
    digitalWrite(LEDOK, HIGH);
  }

  else if (interruptCounter == 1) {
    Estado = "B";
    inicie = 1;
  }

  if (inicie >= 1) {
    Blink_OK_SLOW();
  }

}
