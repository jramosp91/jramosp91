void Blink_OK_FAST () {

  if (digitalRead(Aire) == 1 or Spelt == 1) {
    if (blinkFS - blinkFE >= BLINKF) {

      blinkFE = blinkFS;

      if (digitalRead(LEDOK) == HIGH) {
        digitalWrite(LEDOK, LOW);

      } else {

        digitalWrite(LEDOK, HIGH);
      }
    }

  }

}
