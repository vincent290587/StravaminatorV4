
// initialise la liste des segments
// en listant les fichiers sur la SD
void initListeSegments() {

  static char chaine[20];

  Serial.flush();
  Serial.println(F("Fichiers ajoutes:"));
  Serial.flush();

  if (file.isOpen())
    file.close();

  while (file.openNext(sd.vwd(), O_READ)) {

    if (file.isFile()) {
      // Indicate not a directory.
      file.getFilename(chaine);
      Serial.print(chaine);
      if (Segment::nomCorrect(chaine)) {
        Serial.println(F("  ajoute"));
        mes_segments.push_back(Segment(chaine));

      } else {
        Serial.println(F(": probleme de nom de fichier"));
      }
    }


    if (file.isOpen())
      file.close();
  }

  return;
}


// charge physique le .crs depuis la SD
int chargerCRS(Segment *mon_segment) {

  int res = 0;
  static float time_start = 0.;
  static char chaine[TAILLE_LIGNE];

  res = 0;
  time_start = 0.;

  if (file.isOpen()) {
    file.close();
  }
  if (mon_segment) {

    Serial.print(F("chargerCRS: ")); Serial.println(mon_segment->getName());
    Serial.print(F("Nb points: ")); Serial.println(mon_segment->longueur());

    Serial.flush();
    if (!file.open(mon_segment->getName(), O_READ)) {
      // echec d'ouverture
      Serial.print(F("cFichier introuvable:"));
      Serial.flush();
      Serial.println(mon_segment->getName());
      return 1;
    }


    while (file.fgets(chaine, TAILLE_LIGNE - 1, NULL)) {

      // on se met au bon endroit
      if (strstr(chaine, "<")) {
        // meta data
      } else if (strstr(chaine, ";")) {
        // on est pret a charger le point
        if (!chargerPoint(chaine, mon_segment, &time_start))
          res++;
      }

    } // fin du fichier
    file.close();
    Serial.println(F("Chargement effectue !"));
    Serial.print(F("Nb points: ")); Serial.println(mon_segment->longueur());
  } else {
    Serial.println(F("Segment vide"));
  }

  return res;
}

// charge un unique point dans le fichier deja ouvert
int chargerPoint(char *buffer, Segment *mon_segment, float *time_start) {

  static int isError = 0;
  static float lon, lat, alt, rtime;
  static float data[4];
  uint8_t pos = 0;
  char *pch;

  lat = 0; lon = 0; alt = 0;
  isError = 0;

  if (!buffer || !mon_segment || !time_start) return 1;

  // on se met au bon endroit
  pch = strtok (buffer, " ; ");
  while (pch != NULL && pos < 4)
  {
    data[pos] = strtof(pch, 0);
    pch = strtok (NULL, " ; ");
    pos++;
  }
  isError = pos != 4;

  if (!isError && mon_segment && time_start) {

    lat = data[0];
    lon = data[1];
    rtime = data[2];
    alt = data[3];

    if (mon_segment->longueur() > 0) {
      if (time_start) rtime -= *time_start;
      mon_segment->ajouterPointFin(lat, lon, alt, rtime);
    }
    else {
      // on init la liste
      if (time_start) *time_start = rtime;
      mon_segment->ajouterPointFin(lat, lon, alt, 0.);
    }

    return 0;

  }
  else {
    // echec
    Serial.println(F("Echec de lecture du point"));
    Serial.println(buffer);
    return isError;
  }
}


