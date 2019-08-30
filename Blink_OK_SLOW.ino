void Blink_OK_SLOW() {

  if (blinkS - blinkE >= BLINK) {

    blinkE = blinkS;

    if (digitalRead(LEDOK) == HIGH) {
      digitalWrite(LEDOK, LOW);

    } else {

      digitalWrite(LEDOK, HIGH);
    }
  }

}
