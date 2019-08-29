#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <WiFiUDP.h>
#include <ESP8266mDNS.h>
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
char estado_buffer[7];                             //BUFFER DATOS CHAR
String ret;                                        // SERVER HTTP
String IND;                                       //String status

//Chars comparativos

char pump[5] = "pump";                             //NOMBRE PETICION BOMBA
char valv[5] = "valv";                             // NOMBRE PETICION VALVULA
char pelt[5] = "pelt";                             // NOMBRE PETICION PELTIER
char END[4] = "end";                               // NOMBRE PETICION FINAL
char ini[4] = "ini";                              // NOOMBRE PETICION INICIO
char sta[4] = "sta";                              // NOMBRE STATUS


//interrupcione

volatile byte interruptCounter = 0;               // contador de interrupcion

//flags

int inicie = 0;                                   // FLAG INICIALIZACION
int finish = 0;                                   // FLAG FINAL DEL JUEGO
int START;                                        // FLAG QUE EL JUEGO EMPEZO
int Spump = 0;                                    // FLAG ESTADO BOMBA, 0 OFF 1 ON
int Spelt = 0;                                    // FLAG ESTADO PELT, 0 OFF 1 ON
int Svalv = 0;                                    // FLAG ESTADO VALV, 0 OFF 1 ON
int RST = 0;                                      // FLAG RESET
int INICIO = 0;                                   // FLAG DE INICIO
int PANIC = 0;                                    //FLAG BOTON PANICO

//Temporizadores

unsigned long Tref_bomba = 0;                     // T REF PARA BOMBA
unsigned long Tref_pelt = 0;                      //T REF PARA PELTIER
unsigned long treq1 = 0;                          // BUFFER DE TIEMPO BOMBA
unsigned long treq2 = 0;                          // BUFFER DE TIEMPO PELTIER
long tbomb = 0;                                   //TIEMPO DE ENCENDIDO BOMBA
long tpelt = 0;                                   // TIEMPO ENCENDIDO BOMBA
unsigned long treset = 0;                         //TIEMPO REF RESET
unsigned long tpress = 2000;
//TIEMPO PARA APRETAR Y RESET EN ms

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
const char ssidAP[] = "Prueba 3";                 //Definimos la SSDI de nuestro servidor WiFi -nombre de red-
const char passwordAP[] = "12345678";             //Definimos la contraseña de nuestro servidor
WiFiServer server(80);                            //Definimos el puerto de comunicaciones

//////////////////////
//////WIFI STA///////
////////////////////

const char ssidSTA[] = "DEBUG";
const char passwordSTA[] = "debug1234";


//Funciones

void Temp();                                      //Proteccion de Temperatura por Sensor
void Boton_Panico();                              //Boton de Start/STOP
void CompareInicio();                             // Inicio
void Acciones();                                  //Funcion Encendido Bomba
void Bomba_Apagado();                             //Funcion Apagado Bomba
void ApagadoPeltier();                            //Funcion Apagado Peltier
void Reset();                                     // Funcion Pin RESET
void PINES();                                     // Declaracion de pines
void Blink_FAULT_FAST();                          //Funcion de blink rapido para led rojo
void Blink_Fault_Slow ();                         //Funcion de blink lento para led rojo
void Blink_OK_SLOW();                             // Funcion de blink rapido para led amarillo
void Blink_OK_FAST();                             //Funcion de blink lento para led amarillo

void setup() {


  //Declaracion de Pines
  PINES();


  //COMUNICACION AP Y STA

  server.begin();                                                         //Inicializacion de Servidor
  WiFi.mode(WIFI_AP_STA);                                                 //Modo de Operacion AP para Access Point/ STA para Station
  WiFi.softAP(ssidAP, passwordAP);                                        //Red con clave, en el canal 1 y visible


  IPAddress local_ip(192, 168, 1, 1);                                    //Modifica la dirección IP en modo AP de la caja
  IPAddress gateway(192, 168, 1, 1);                                     // Gateway de AP la caja
  IPAddress subnet(255, 255, 255, 0);                                    // Subnet del AP la caja
  WiFi.softAPConfig(local_ip, gateway, subnet);


  WiFi.begin(ssidSTA, passwordSTA);
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {


    Serial.println("Connection Ok");
    Serial.println(WiFi.localIP());

    t_httpUpdate_return ret = ESPhttpUpdate.update("https://jramosp91.github.io/HeartFail.ino.nodemcu.bin", "", "70 0B 6F 62 4F 41 EB 1A 42 3F 73 5A DA 96 98 2D 7F 2B 75 6F");


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

        digitalWrite(LEDOK, HIGH);
        delay(50);
        digitalWrite(LEDFAULT, HIGH);
        digitalWrite(LEDOK, LOW);
        delay(150);
        digitalWrite(LEDFAULT, LOW);
        digitalWrite(LEDOK, HIGH);
        delay(1000);

        break;



    }


    //FLASH POR WIFI

    ArduinoOTA.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else { // U_SPIFFS
        type = "filesystem";
      }

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
        Serial.println("Auth Failed");
      } else if (error == OTA_BEGIN_ERROR) {
        Serial.println("Begin Failed");
      } else if (error == OTA_CONNECT_ERROR) {
        Serial.println("Connect Failed");
      } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println("Receive Failed");
      } else if (error == OTA_END_ERROR) {
        Serial.println("End Failed");
      }
    });

    ArduinoOTA.begin();

    millis();
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

void loop() {                                                               //INICIALIZACION  MILLIS

  blinkS = millis();
  blinkFS = millis();

  ArduinoOTA.handle();                                                         ///invoca si hay que flashear online

  CompareInicio();                                                              // Inicio
  Reset();                                                                      // Funcion del boton reset
  Temp();                                                                       //Proteccion de Temperatura por Sensor
  Boton_Panico();                                                                //Boton de Start/STOP
  Acciones();                                                                    //Funcion Encendido Bomba
  Bomba_Apagado();                                                              //Funcion Apagado Bomba
  ApagadoPeltier();                                                            //Funcion Apagado Peltier
  Blink_FAULT_FAST();                                                          // Funcion de blinking rapido para led rojo
  Blink_OK_FAST();                                                            //Funcion de blinking rapido para led amarillo

  memset(estado_buffer, 0, sizeof(estado_buffer));                          //vacia buffer




}
