void Bomba_Apagado () {

  if (digitalRead(Aire) == 1 ) {
    Serial.println("Pase por Apagado Bomba");
    Serial.println("Conteo para apagar bomba");
    Serial.print("LA BOMBA SE APAGA EN: ");
    Serial.println(treq1);
  }


  if (digitalRead(Aire) == 1 and millis() > treq1) {

    digitalWrite(Aire, LOW);
    Serial.println("Apago BOmba"); //
    Serial.println(millis());
    Serial.println(Tref_bomba + tbomb);
    Serial.println(millis() - (Tref_bomba + tbomb));
    Spump = 0;
   // ACTB= 0;
  }

}
