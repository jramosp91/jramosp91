#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoOTA.h>
#include <WiFiUDP.h>
#include <ESP8266mDNS.h>
#include <ESP8266httpUpdate.h>
#define DEBUG_HTTP_UPDATE
#define DEBUG_ESP_HTTP_UPDATE

///Pines Digitales

const int Aire = D5; // Bomba de aire
const int Cale = D8; // Peltier
const int Valv = D6; // Electrov
const int RESET = D4; // Boton RESET
const byte Panic = D7; //Boton del Panico
const int LEDOK = D3; // LED OK
const int LEDFAULT = D1 ; // LED FALLA
const int TEMPSENS = D2; // SENSOR DE TEMP

//Strings de datos

String peticion; // PETICION DEL CLIENTE
String estado; // DATO QUE ENVIA EL UNITY
int tiempo; // TIEMPO QUE ENVIA EL UNITY
char estado_buffer[7]; //BUFFER DATOS CHAR
String ret; /// SERVER HTTP

//Chars comparativos

char pump[5] = "pump"; //NOMBRE PETICION BOMBA
char valv[5] = "valv"; // NOMBRE PETICION VALVULA
char pelt[5] = "pelt"; // NOMBRE PETICION PELTIER
char fin[4] = "fin"; // NOMBRE PETICION FINAL
char ini[4] = "ini"; // NOOMBRE PETICION INICIO
char sta[4] = "sta"; // NOMBRE STATUS

//interrupcione

volatile byte interruptCounter = 0; // contador de interrupcion

//flags

int inicie = 0; // FLAG INICIALIZACION
int finish = 0; // FLAG FINAL DEL JUEGO
int START; /// FLAG QUE EL JUEGO EMPEZO
int Spump = 0; /// FLAG ESTADO BOMBA, 0 OFF 1 ON
int Spelt = 0; /// FLAG ESTADO PELT, 0 OFF 1 ON
int Svalv = 0; /// FLAG ESTADO VALV, 0 OFF 1 ON
int RST = 0; /// FLAG RESET
int INICIO = 0; /// FLAG DE INICIO
int PANIC = 0; ///FLAG BOTON PANICO

//Temporizadores

unsigned long Tref_bomba = 0; //// T REF PARA BOMBA
unsigned long Tref_pelt = 0; //T REF PARA PELTIER
unsigned long treq1 = 0; // BUFFER DE TIEMPO BOMBA
unsigned long treq2 = 0; // BUFFER DE TIEMPO PELTIER
long tbomb = 0; //TIEMPO DE ENCENDIDO BOMBA
long tpelt = 0; // TIEMPO ENCENDIDO BOMBA
unsigned long treset = 0; //TIEMPO REF RESET
unsigned long tpress = 3000; //TIEMPO PARA APRETAR Y RESET EN ms

unsigned long CDOn = 0;
unsigned long CDoff = 0;

//Temperatura maxima

int temp_max = 45 ;// INDICE DE TEMP MAX

//debouncer

const int timeThreshold = 150; // REF PARA DEBOUNCER
long starttime = 0; // TIEMPO INICIO DEBOUNCE


//////////////////////////
/////////WIFI AP//////////
//////////////////////////
const char ssidAP[] = "Prueba 1";    //Definimos la SSDI de nuestro servidor WiFi -nombre de red-
const char passwordAP[] = "12345678";       //Definimos la contraseña de nuestro servidor
WiFiServer server(80);                    //Definimos el puerto de comunicaciones

//////////////////////
//////WIFI STA///////
////////////////////

const char ssidSTA[] = "DEBUG";
const char passwordSTA[] = "debug1234";

//////////////////////////////
/////////ONE WIRE SETUP///////
/////////////////////////////

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(TEMPSENS);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);


//Funciones

void Temp(); //Proteccion de Temperatura por Sensor
void Boton_Panico(); //Boton de Start/STOP
void CompareInicio(); // Inicio
void Acciones(); //Funcion Encendido Bomba
void Bomba_Apagado(); //Funcion Apagado Bomba
void ApagadoPeltier(); //Funcion Apagado Peltier
void OTA();
void Reset();
//void SERVER_ON();
void PINES();


void setup() {

  //Declaracion de Pines
  PINES();

  //SENSOR ON
  sensors.begin();

  //COMUNICACION AP Y STA

  server.begin();                         //inicializamos el servidor
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssidAP, passwordAP);            //Red con clave, en el canal 1 y visible
  //WiFi.softAP(ssid, password,3,1);      //Red con clave, en el canal 3 y visible

  //WiFi.softAP(ssid);                    //Red abierta
  //  Serial.print("Direccion IP Access Point - por defecto: ");      //Imprime la dirección IP
  //  Serial.println(WiFi.softAPIP());
  Serial.print("Direccion MAC Access Point: ");                   //Imprime la dirección MAC
  Serial.println(WiFi.softAPmacAddress());

  IPAddress local_ip(192, 168, 1, 1);                           //Modifica la dirección IP
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  Serial.println();
  Serial.print("Access Point - Nueva direccion IP: ");
  Serial.println(WiFi.softAPIP());
  millis();

  WiFi.begin(ssidSTA, passwordSTA);
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {
    Serial.println("Connection Ok");
    Serial.println(WiFi.localIP());

    t_httpUpdate_return ret = ESPhttpUpdate.update("https://github.com/jramosp91/INTERATICAHF/blob/Modificaciones/HeartFail.ino.nodemcu.bin", "","CA 06 F5 6B 25 8B 7A 0D 4F 2B 05 47 09 39 47 86 51 15 19 84");

    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s",  ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;
    }



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
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

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

void loop() {

  ArduinoOTA.handle();   ///invoca si hay que flashear online



  CompareInicio(); // Inicio
  Reset(); // Funcion del boton reset
  Temp(); //Proteccion de Temperatura por Sensor
  Boton_Panico(); //Boton de Start/STOP
  Acciones(); //Funcion Encendido Bomba
  Bomba_Apagado(); //Funcion Apagado Bomba
  ApagadoPeltier(); //Funcion Apagado Peltier

  memset(estado_buffer, 0, sizeof(estado_buffer)); //vacia buffer




}
