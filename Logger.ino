
uint32_t getLastAddress() {

  uint8_t i;
  SWhiteBox whitebox;
  memset(&whitebox, 0, sizeof(SInitTable));

  sst.readArray(TABLE, whitebox.tab, sizeof(SInitTable));

  if (whitebox.sinit.secret_code != SECRET_CODE) {
    Serial.print("Formattage SST26VF: ");
    Serial.println(whitebox.sinit.secret_code, HEX);

    for (i=0; i <= NB_SECTORS_REC; i++) {
      sst.block64Erase(i);
    }
    //sst.totalErase();
    delay(5);

    memset(&whitebox, 0, sizeof(SInitTable));
    whitebox.sinit.secret_code = SECRET_CODE;
    whitebox.sinit.last_data = SST_RECORDING_START;

    sst.writeArray(TABLE, whitebox.tab, sizeof(SInitTable));

    return SST_RECORDING_START;
  } else {
    return whitebox.sinit.last_data;
  }

}

void incrementeAddress () {

  SWhiteBox whitebox;
  memset(&whitebox, 0, sizeof(SInitTable));

  //sst.dumpArray(TABLE, sizeof(SInitTable));
  sst.readArray(TABLE, whitebox.tab, sizeof(SInitTable));

  if (whitebox.sinit.secret_code != SECRET_CODE) {
    Serial.print("Erreur code SST26VF: ");
    Serial.println(whitebox.sinit.secret_code, HEX);
    return;
  } else {
    whitebox.sinit.secret_code = SECRET_CODE;
    whitebox.sinit.last_data += sizeof(SAttitude);
    whitebox.sinit.nb_data += 1;
    sst.sectorErase(0);
    delay(1);
    sst.writeArray(TABLE, whitebox.tab, sizeof(SInitTable));
    //sst.dumpArray(TABLE, sizeof(SInitTable));
  }

}


void dumpLogGPS() {

  uint32_t i, adr_log;
  SWhiteBox whitebox;
  SBlackBox bbox;
  memset(&whitebox, 0, sizeof(SInitTable));

  sst.readArray(TABLE, whitebox.tab, sizeof(SInitTable));

  if (whitebox.sinit.secret_code != SECRET_CODE) return;

  Serial.println("Dump du log:");

  for (i = 0; i < whitebox.sinit.nb_data; i++) {

    adr_log = SST_RECORDING_START + i * sizeof(SAttitude);

    sst.readArray(adr_log, bbox.tab, sizeof(SAttitude));

    Serial.print("Points actifs: 0x");
    Serial.print(adr_log, HEX);
    Serial.print("  ");
    Serial.println(bbox.satt.nbpts);
    
  }
}

void loggerMsg(const char *msg_) {

  Serial.println(msg_);

  if (gpx.isOpen()) gpx.close();
  if (gpx.open("errlog.txt", O_WRITE | O_CREAT | O_APPEND)) {
    gpx.println(msg_);
    // fclose
    gpx.close();
  }

}

void loggerMsg(int val_) {

  Serial.println(val_);

  if (gpx.isOpen()) gpx.close();
  if (gpx.open("errlog.txt", O_WRITE | O_CREAT | O_APPEND)) {
    gpx.println(val_);
    // fclose
    gpx.close();
  }

}

void loggerMsg(float val_1, float val_2) {

  char dataTW[23];

  if (gpx.isOpen()) gpx.close();
  if (gpx.open("errlog.txt", O_WRITE | O_CREAT | O_APPEND)) {
    mondtostrf(val_1, 11, 7, dataTW);
    dataTW[11] = '\0';
    gpx.print(dataTW);

    gpx.print("@");

    mondtostrf(val_2, 11, 7, dataTW);
    dataTW[11] = '\0';
    gpx.println(dataTW);
    // fclose
    gpx.close();
  }

}


void loggerHT() {

  static int header_ecrit = 0;

  if (gpx.isOpen()) gpx.close();
  if (gpx.open("ht.csv", O_WRITE | O_CREAT | O_APPEND)) {

    // en tete
    if (header_ecrit == 0) {
      gpx.println(F("HRM,CAD,PW,VBatt"));
      header_ecrit = 1;
    }

    // corps
    gpx.print(att.bpm); gpx.print(",");

    gpx.print(att.cad_rpm); gpx.print(",");

    //TODO
    //gpx.print(calcul_puissance());gpx.print(",");

    gpx.println(att.vbatt * 1000.);

    // fclose
    gpx.close();

  }

}



void loggerRR() {

  static int header_ecrit = 0;

  if (gpx.isOpen()) gpx.close();
  if (gpx.open("rr.csv", O_WRITE | O_CREAT | O_APPEND)) {

    // en tete
    if (header_ecrit == 0) {
      gpx.println(F("HRM,RR,VBatt"));
      header_ecrit = 1;
    }

    // corps
    gpx.print(att.bpm); gpx.print(",");

    gpx.print(att.rrint); gpx.print(",");

    gpx.println(att.vbatt * 1000.);

    // fclose
    gpx.close();

  }

}

void loggerData() {

#ifdef __DEBUG__
  uint32_t address_write;
  SBlackBox bbox;

  address_write = getLastAddress();

  memcpy(&bbox.satt, &att, sizeof(SAttitude));
  sst.writeArray(address_write, bbox.tab, sizeof(SAttitude));

  // update current address
  incrementeAddress();


  return;
#endif

  static int header_ecrit = 0;

  if (gpx.isOpen()) gpx.close();
  if (gpx.open("today.csv", O_WRITE | O_CREAT | O_APPEND)) {

    // en tete
    if (header_ecrit == 0) {
      ecrireHeader ();
      header_ecrit = 1;
    }

    // corps
    positionEcrit();

    // fclose
    gpx.close();

  }

}

void ecrireHeader () {

  gpx.print(F("Latitude,Longitude,Elevation,SecJour,HRM,"));
  gpx.println(F("CAD,Temp,GPSElevation,NsegActifs,PVBatt,Loop"));

}

void positionEcrit () {

  char dataTW[20];

  mondtostrf(att.lat, 11, 7, dataTW);
  dataTW[11] = '\0';
  gpx.print(dataTW); gpx.print(",");

  mondtostrf(att.lon, 11, 7, dataTW);
  dataTW[11] = '\0';
  gpx.print(dataTW); gpx.print(",");

  mondtostrf(att.alt, 7, 2, dataTW);
  dataTW[11] = '\0';
  gpx.print(dataTW); gpx.print(",");

  mondtostrf(att.secj, 9, 3, dataTW);
  dataTW[11] = '\0';
  gpx.print(dataTW); gpx.print(",");

  gpx.print(att.bpm); gpx.print(",");

  gpx.print(att.cad_rpm); gpx.print(",");

  mondtostrf(att.temp, 7, 2, dataTW);
  dataTW[11] = '\0';
  gpx.print(dataTW); gpx.print(",");

  mondtostrf(att.gpsalt, 7, 2, dataTW);
  dataTW[11] = '\0';
  gpx.print(dataTW); gpx.print(",");

  mondtostrf(att.vbatt, 6, 3, dataTW);
  dataTW[11] = '\0';
  gpx.print(dataTW); gpx.print(",");

  mondtostrf(att.cbatt, 6, 3, dataTW);
  dataTW[11] = '\0';
  gpx.print(dataTW); gpx.print(",");

  gpx.println(time_c);

}

