
void updateAltitude(float *_alt) {

  /* Get a new sensor event */
  static sensors_event_t event;
  
  bmp.getEvent(&event);

  /* Display the results (barometric pressure is measure in hPa) */
  if (event.pressure)
  {
    
    bmp.getTemperature(&att.temp);
    bmp.getPressure(&att.pressu);

    /* Then convert the atmospheric pressure, and SLP to altitude         */
    *_alt = bmp.pressureToAltitude(sea_level_pressure, att.pressu) - altOffset;
    
  }
  else
  {
    Serial.println(F("Sensor error"));
  }

}

void updateAltitudeOffset (float *_alt) {
  
  altOffset=0.;
  // pressure est mis a jour ici
  updateAltitude(_alt);
  // mise a jour de la pression au niveau de la mer
  sea_level_pressure = bmp.seaLevelForAltitude(att.gpsalt, att.pressu);
  // recalcul de falt avec la premiere mise a jour
  updateAltitude(_alt);
  // on set l'offset
  altOffset = att.alt - att.gpsalt;
  
}



