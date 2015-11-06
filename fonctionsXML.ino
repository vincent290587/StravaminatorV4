
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
        Serial.println(F(": probleme de nom de fichier: "));
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
    
    Serial.print(F("chargerCRS: "));Serial.println(mon_segment->getName());
    Serial.print(F("Nb points: "));Serial.println(mon_segment->longueur());
    
    Serial.flush();
    if (!file.open(mon_segment->getName(), O_READ)) {
      // echec d'ouverture
      Serial.print(F("cFichier introuvable:"));
      Serial.flush();
      Serial.println(mon_segment->getName());
      return 1;
    }
    
    
    while (file.fgets(chaine, TAILLE_LIGNE, NULL)) {

      // on se met au bon endroit
      if (strstr(chaine, "<Trackpoint>")) {
        // on est pret a charger le point
        if (!chargerPoint(chaine, mon_segment, &time_start))
          res++;
      }

    } // fin du fichier
    file.close();
    Serial.println(F("Chargement effectue !"));
    Serial.print(F("Nb points: "));Serial.println(mon_segment->longueur());
  } else {
    Serial.println(F("Segment vide"));
  }

  return res;
}

// charge un unique point dans le fichier deja ouvert
int chargerPoint(char *buffer, Segment *mon_segment, float *time_start) {

  static int isError = 0;
  static int year_, month_, day_, hour_, min_, sec_;
  static float sec_jour = 0.;
  static float lon, lat, alt, rtime;
  char *ptr_buffer = NULL;
  static char sbuff[TAILLE_LIGNE];

  lat = 0; lon = 0; alt = 0;
  isError = 0;
  sec_jour = 0.;
  
  if (!buffer || !mon_segment || !time_start) return 1;
  
  while (file.fgets(buffer, TAILLE_LIGNE, NULL)) {
    
    // on se met au bon endroit
    ptr_buffer = strstr(buffer, "<");
    
    if (strstr(buffer, "</Trackpoint>")) {
      // on a fini de lire le point
      //Serial.println(F("Fin lecture point CRS"));
      break;
    }
    else if (strstr(buffer, "Time")) {
      year_ = 0;
      month_ = 0;
      day_ = 0;
      hour_ = 0;
      min_ = 0;
      sec_ = 0;
      isError += sscanf(ptr_buffer, "<Time>%d-%d-%dT%d:%d:%dZ</Time>\n", &year_, &month_, &day_, &hour_, &min_, &sec_) != 6;
      sec_jour = hour_ * 3600 + min_ * 60 + sec_;
    } else if (strstr(buffer, "Chrono")) {
      isError += sscanf(ptr_buffer, "<Chrono>%s</Chrono>\n", sbuff) != 1;
      if (!isError) sec_jour = atof(sbuff);
    } 
    else if (strstr(buffer, "LatitudeDegrees")) {
      isError += sscanf(ptr_buffer, "<LatitudeDegrees>%s</LatitudeDegrees>\n", sbuff) != 1;
      if (!isError) lat = atof(sbuff);
    } 
    else if (strstr(buffer, "LongitudeDegrees")) {
      isError += sscanf(ptr_buffer, "<LongitudeDegrees>%s</LongitudeDegrees>\n", sbuff) != 1;
      if (!isError) lon = atof(sbuff);
    } 
    else if (strstr(buffer, "AltitudeMeters")) {
      isError += sscanf(ptr_buffer, "<AltitudeMeters>%s</AltitudeMeters>\n", sbuff) != 1;
      if (!isError) alt = atof(sbuff);
    }

    Serial.flush();
  } // fin du fichier ou du point
  
  if (!isError && mon_segment && time_start) {
    //Serial.println(F("Lecture correcte"));
    // on a tout bien lu
    // on cree le point
    
    if (mon_segment->longueur() > 0) {
      rtime = sec_jour - *time_start;
      mon_segment->ajouterPointFin(lat, lon, alt, rtime);
    } 
    else {
      // on init la liste
      *time_start = sec_jour;
      mon_segment->ajouterPointFin(lat, lon, alt, 0.);
    }
    //Serial.print(F("Nb points: "));Serial.println(longueurSegment(mon_segment));
    if (mon_segment->longueur() == 0) Serial.println(F("Echec de l'ajout"));
    Serial.flush();
    return 0;

  }
  else {
    // echec
    Serial.println(F("Echec de lecture du point"));
    return isError;
  }
}

void reparerNomFichier(Segment *mon_segment) {

  static float time_start = 0.;
  static char chaine[TAILLE_LIGNE];

  time_start = 0.;

  Serial.flush();
  if (mon_segment) {
    // ouverture fichier
    if (file.isOpen()) {
      file.close();
    }
    if (!file.open(mon_segment->getName(), O_READ | O_WRITE | O_CREAT)) {
      // echec d'ouverture
      loggerMsg("Fichier introuvable:");
      loggerMsg(mon_segment->getName());
      Serial.flush();
      return;
    }

    Serial.print("Reparation: ");
    Serial.println(mon_segment->getName());
    Serial.print("Taille: ");
    Serial.println(file.fileSize());
    Serial.flush();

    if (file.fileSize() < 5000) {
      loggerMsg("Taille trop faible: ");
      loggerMsg(mon_segment->getName());
      // TODO
      //mon_segment->nomFichier = NULL;
      Serial.flush();
    }
    
    // lecture des lignes du fichier
    while (file.fgets(chaine, TAILLE_LIGNE, NULL)) {
      
      // on se met au bon endroit
      if (strstr(chaine, "<Trackpoint>")) {
        // on est pret a charger le point
        Serial.print(F("Nb points: "));Serial.println(mon_segment->longueur());
        mon_segment->desallouerPoints();
        if (!chargerPoint(chaine, mon_segment, &time_start)) {
          // on vient de reussir a charger notre premier point
          //Serial.print(F("Nb points: "));Serial.println(mon_segment->longueur());
          if (mon_segment->longueur()==0) {
            loggerMsg("Liste vide !!!");
            break;
          }
          Serial.println("Construction nom: ");
          Serial.flush();
          if (repareNomSegment(mon_segment)) {
            loggerMsg("Echec creation nouveau nom");
            break;
          }
          Serial.print("Renommage: ");Serial.println(mon_segment->getName());
          
	        mon_segment->desallouerPoints();
          //Serial.println(F("On vire")); Serial.flush();
          //Serial.print(F("Nb points: "));Serial.println(longueurSegment(mon_segment));
          
          if (!file.rename(sd.vwd(), mon_segment->getName())) {

            Serial.print(F("Changement nom: "));
            Serial.println(mon_segment->getName());
            // TODO !!!!
            //if (mon_segment->getName().c_str()[4]!='Z') {
            //  mon_segment->nomFichier[4]+=1;
            //} else if (mon_segment->nomFichier[11]!='Z') {
            //  mon_segment->nomFichier[11]+=1;
            //}
            Serial.print(F("  -->  "));
            Serial.println(mon_segment->getName());
            if (!file.rename(sd.vwd(), mon_segment->getName())) {
              loggerMsg("Echec du renommage: liberation");
              // TODO !!!
              //mon_segment->nomFichier = NULL;
              Serial.flush();
              break;
            }
          }
          // fin renommage
          Serial.println("Renommage OK");
          break;
        }
        else {
          loggerMsg("Echec de chargement du point");
          Serial.flush();
          break;
        }
      }

    } // fin du fichier
    if (file.isOpen()) {
      file.close();
    }
    loggerMsg("Fin renommage");
  }

  return;
}


int repareNomSegment(Segment *mon_segment) {

  if (mon_segment->longueur()==0)
    return 1;

    Point pp = *mon_segment->getFirstPoint();

    if (!pp.isValid()) {
      return 1;
    }
  
    // on arrondit au plus proche a 5.e-6 degres
    mon_segment->setSegmentName(calculeNom (pp._lat, pp._lon));

    return 0;
}








