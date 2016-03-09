
uint32_t getLastAddress() {

  SWhiteBox whitebox;
  memset(&whitebox, 0, sizeof(SWhiteBox));

  sst.readArray(0, whitebox.tab, sizeof(SWhiteBox));

  if (whitebox.sinit.secret_code != SECRET_CODE) {
    Serial.println("Formattage SST26VF");
    sst.totalErase();
    memset(&whitebox, 0, sizeof(SWhiteBox));
    whitebox.sinit.secret_code = SECRET_CODE;
    whitebox.sinit.last_data = SST_RECORDING_START;
    sst.writeArray(0, whitebox.tab, sizeof(SWhiteBox));
    return SST_RECORDING_START;
  } else {
    return whitebox.sinit.last_data;
  }

}

void incrementeAddress () {

  SWhiteBox whitebox;
  memset(&whitebox, 0, sizeof(SWhiteBox));

  sst.readArray(0, whitebox.tab, sizeof(SWhiteBox));

  if (whitebox.sinit.secret_code != SECRET_CODE) {
    Serial.println("Erreur code SST26VF");
    return;
  } else {
    whitebox.sinit.secret_code = SECRET_CODE;
    whitebox.sinit.last_data += sizeof(SAttitude);
    whitebox.sinit.nb_data += 1;
    sst.writeArray(0, whitebox.tab, sizeof(SWhiteBox));
  }

}


void dumpLogGPS() {
  return;
  if (gpx.isOpen()) gpx.close();
  if (gpx.open("today.csv", O_READ)) {

    while (gpx.available()) {
      Serial.write(gpx.read());
    }
    Serial.flush();
    gpx.close();
  }
  return;
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
  return;
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
  return;
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

  uint32_t address_write = getLastAddress();
  SBlackBox bbox;

  memcpy(&bbox.satt, &att, sizeof(SAttitude));
  sst.writeArray(address_write, bbox.tab, sizeof(SAttitude));

  // update current address
  incrementeAddress();

  return;

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

  static char dataTW[20];

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

