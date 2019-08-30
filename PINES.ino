void PINES() {
  
    /////////////////////////////////////
  ///////DECLARACIONES DE LOS PINES//////
  ///////////////////////////////////////

  //Boton del Panico
  pinMode (Panic, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(Panic), handleInterrupt, FALLING);

  //Boton de Reset
  pinMode(RESET, INPUT_PULLUP);


  // Pin Bomba
  pinMode(Aire, OUTPUT);
  digitalWrite(Aire, LOW);

  // Pin Peltier
  pinMode(Cale, OUTPUT);
  digitalWrite(Cale, LOW);

  // Pin Valv
  pinMode(Valv, OUTPUT);
  digitalWrite(Valv, LOW);

//SERIAL ON
Serial.begin(9600);

   //LEDOK
  pinMode(LEDOK, OUTPUT);
  digitalWrite(LEDOK, LOW);

  //LED FAULT
  pinMode(LEDFAULT, OUTPUT);
  digitalWrite(LEDFAULT, LOW);
}
