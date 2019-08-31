void Boton_Panico() {


  if (interruptCounter >= 2 and inicie>= 1) {
    PANIC = 1;
  }

  if (PANIC == 1) {
    Estado = "D";
    inicie = 0;
    interruptCounter=3;
    Serial.println("Pase por boton del panico");
    digitalWrite(Aire, LOW);
    digitalWrite(Cale, LOW);
    digitalWrite(Valv, LOW);
    digitalWrite(LEDOK, LOW);
    digitalWrite(LEDFAULT, HIGH);
    }

}
