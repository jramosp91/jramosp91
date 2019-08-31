void ApagadoPeltier() {

  if (digitalRead(Cale) == 1 ) {
     }


  if (digitalRead(Cale) == 1 and millis() > treq2) {

    digitalWrite(Cale, LOW);
    digitalWrite(LEDFAULT, LOW);
    Spelt=0;
    
  }

}
