void JSON () {

  Serial.println("PASE POR JSON");

  OUT = "";

  const size_t capacity = JSON_OBJECT_SIZE(6);
  DynamicJsonDocument doc(capacity);


  doc["command"] = comando;
  doc["heating"] = Spelt;
  doc["inflating"] = Spump;
  doc["valve"] = Svalv;
  doc["state"] = Estado;
  doc["ver"] = VERSION;

  serializeJson(doc, OUT);
}
