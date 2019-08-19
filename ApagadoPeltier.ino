void ApagadoPeltier() {

  if (digitalRead(Cale) == 1 ) {
    Serial.println("Conteo para apagar peltier");
    Serial.println("apagado Peltier");
    Serial.print("EL PELTIER SE APAGA EN: ");
    Serial.println(treq2);
  }


  if (digitalRead(Cale) == 1 and millis() > treq2) {

    digitalWrite(Cale, LOW);
    Serial.println("Apago Peltier"); //
    Serial.println(millis());
    Serial.println(Tref_pelt + tpelt);
    Serial.println(millis() - (Tref_pelt + tpelt));
    Spelt=0;
   // ACTP= 0;
  }

}
