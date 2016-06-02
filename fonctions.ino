
int percentageBatt (float tensionValue) {

  float fp_ = 0.;

  if (tensionValue > 3.694) {
    fp_ = 536.34*tensionValue*tensionValue*tensionValue-6723.8*tensionValue*tensionValue;
    fp_ += 28186*tensionValue-39407;
  } else if (tensionValue > 2.) {
    fp_ = 1e-12*pow(tensionValue, 22.315);
  } else {
    fp_ = 0;
  }

  return (int)fp_;
}


uint32_t myFreeRam(){ // for Teensy 3.0
    uint32_t stackTop;
    uint32_t heapTop;

    // current position of the stack.
    stackTop = (uint32_t) &stackTop;

    // current position of heap.
    void* hTop = malloc(1);
    heapTop = (uint32_t) hTop;
    free(hTop);

    // The difference is the free, available ram.
    return stackTop - heapTop;
}

int isGpsDataValid(float lat, float lon, unsigned long age) {
  int res = 1;

  if (lat == TinyGPS::GPS_INVALID_F_ANGLE || lon == TinyGPS::GPS_INVALID_F_ANGLE) {
    res = 0;
  }
  else if (age == TinyGPS::GPS_INVALID_AGE) {
    res = 0;
  }

  return res;
}


float get_sec_jour()
{
  static unsigned long time;
  static float res = 0;
  static unsigned long sec_, csec_;
  static unsigned long hour_, min_;

  gps.get_datetime(NULL, &time, NULL);

  hour_ = time / 1000000;
  min_ = (time / 10000) % 100;
  sec_ = (time / 100) % 100;
  csec_ = time % 100;
  res = 3600. * hour_ + 60. * min_ + sec_ + (float)csec_ / 100.;

  return res;
}

void resetdelay_() {
  start = millis();
}



int parseSegmentName(const char *name, float *lat_, float *lon_) {

  if (!name) {
    return 1; 
  }
  
  if (strstr(name, "$"))
      return 1;
  
  calculePos (name, lat_, lon_);

  return 0;
}

// desalloue automatiquement les segments lointains
// alloue automatiquement les segments proches

float watchdog(Segment *mon_seg, float lat1, float long1) {

  static float tmp_dist = 0.;
  static float tmp_lat = 0.;
  static float tmp_lon = 0.;
  float ret_val = 5000;
  
  //Serial.println(F("Debut watchdog "));
  //Serial.flush();

  // le segment est rempli
  if (mon_seg->isValid() && mon_seg->getStatus() == SEG_OFF) {

    if (mon_seg->longueur() >= 1) {
      // on teste l'eloignement
      Point pp = *mon_seg->getFirstPoint();
      
      tmp_dist = distance_between(lat1, long1, pp._lat, pp._lon);
      ret_val = tmp_dist;
      
      if (tmp_dist > DIST_ALLOC) {
        // on desalloue
        Serial.print(F("Le watchdog desalloue dist = "));
        Serial.print((int)tmp_dist);
        Serial.print(F("  -> "));
        Serial.println(mon_seg->getName());
        mon_seg->desallouerPoints();
        mon_seg->setStatus(SEG_OFF);
      }
    }
    else {

      if (parseSegmentName(mon_seg->getName(), &tmp_lat, &tmp_lon) == 1) {
        Serial.println(F("Echec parsing du nom"));
        loggerMsg("Echec parsing du nom");
        loggerMsg(mon_seg->getName());
        return ret_val;
      }

      // on etudie si on doit charger
      tmp_dist = distance_between(lat1, long1, tmp_lat, tmp_lon);
      ret_val = tmp_dist;

      if (tmp_dist < DIST_ALLOC) {

        if (mon_seg->longueur() > 0) {
          loggerMsg("Sauvetage de points");
          Serial.println(F("Sauvetage de points"));
          mon_seg->desallouerPoints();
        }

        Serial.println(F("Le watchdog charge le fichier"));
        chargerCRS(mon_seg);

      }
    }

  } else if (mon_seg->longueur() > 0 && mon_seg->isValid()) {

    Point tmp_pt(lat1, long1, 0., 0.);

    tmp_dist = mon_seg->dist(&tmp_pt);
    ret_val = tmp_dist;
    
    if (tmp_dist > MARGE_DESACT * DIST_ALLOC) {
      // on desalloue
      Serial.print(F("Le watchdog desalloue2 dist = "));
      Serial.print((int)tmp_dist);
      Serial.print(F("  -> "));
      Serial.println(mon_seg->getName());
      loggerMsg("Desallocation non nominale !");
      loggerMsg(mon_seg->getName());
      loggerMsg(String(tmp_dist, 1).c_str());
      mon_seg->desallouerPoints();
      mon_seg->setStatus(SEG_OFF);
    }
  }

  return ret_val;
}



