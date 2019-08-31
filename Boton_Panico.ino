void Boton_Panico() {


  if (interruptCounter >= 2 and START >= 1) {
    PANIC = 1;
  }

  if (PANIC == 1) {
    Estado = "D";
    inicie = 0;
    START = 0;

    Serial.println("Pase por boton del panico");
    digitalWrite(Aire, LOW);
    digitalWrite(Cale, LOW);
    digitalWrite(Valv, LOW);
    digitalWrite(LEDOK, LOW);
    digitalWrite(LEDFAULT, HIGH);
    }

}
