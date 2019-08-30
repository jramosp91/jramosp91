void Blink_Fault_Slow () {


  if (blinkS - blinkE >= BLINK) {

    blinkE = blinkS;

    if (digitalRead(LEDFAULT) == HIGH) {
      digitalWrite(LEDFAULT, LOW);

    } else {

      digitalWrite(LEDFAULT, HIGH);
    }
  }
  digitalWrite(LEDFAULT, LOW);



}
