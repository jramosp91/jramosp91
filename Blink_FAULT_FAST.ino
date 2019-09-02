void Blink_FAULT_FAST() {

  if (digitalRead(Cale) == 1 or Spelt ==1) {

    if (blinkFS - blinkFE >= BLINKF) {

      blinkFE = blinkFS;

      if (digitalRead(LEDFAULT) == HIGH) {
        digitalWrite(LEDFAULT, LOW);

      } else {

        digitalWrite(LEDFAULT, HIGH);
      }
    }


  }
}
