


uint8_t cond_wait () {

  static long millis_ = millis();

  if (download_request || upload_request) {
    return 0;
  }

  if (display.getPendingAction() != NO_ACTION) {
    display.updateScreen();
  }

  if (millis() - millis_ > 1500) {
    millis_ = millis();
    return 0;
  }

  if (new_btn_data != 0) {
    millis_ = millis();
    return 0;
  }

  switch (display.getModeCalcul()) {
    case MODE_GPS:
      if (new_ancs_data != 0) {
        millis_ = millis();
        return 0;
      }
    case MODE_CRS:
    case MODE_PAR:
      if (new_gps_data != 0) {
        millis_ = millis();
        return 0;
      }
      break;
    case MODE_HRM:
      if (new_hrm_data != 0) {
        millis_ = millis();
        return 0;
      }
      break;
    case MODE_HT:
      if (new_cad_data != 0) {
        millis_ = millis();
        return 0;
      }
      break;
  }

  return 1;
}


void boucle_outdoor () {

  float min_dist_seg = 50000;
  float tmp_dist;
  uint8_t order_glasses;

  resetdelay_();

  cumuls.majPower(&mes_points, att.speed);

  std::list<Segment>::iterator _iter;
  Segment *seg;

  att.nbact = 0;
  order_glasses = 0;
  display.resetSegments();
  for (_iter = mes_segments._segs.begin(); _iter != mes_segments._segs.end(); _iter++) {

    seg = _iter.operator->();

    if (seg->isValid()) {

      tmp_dist = watchdog (seg, att.lat, att.lon);
      if (tmp_dist < min_dist_seg && seg->getStatus() == SEG_OFF) min_dist_seg = tmp_dist;

      seg->majPerformance(&mes_points);

      if (seg->getStatus() != SEG_OFF) {
        att.nbact += 1;
        if (seg->getStatus() == SEG_START) {
          Serial.println("Segment commence");
          segStartTone ();
        } else if (seg->getStatus() == SEG_FIN) {
          Serial.println("Segment termine");
          if (seg->getAvance() > 0.) {
            att.nbpr++;
            att.nbkom++;
            victoryTone ();
          }
        } else if (seg->getStatus() == SEG_ON && order_glasses == 0) {
          Serial3.println(Nordic::encodeOrder(seg->getAvance(), seg->getCur()));
          order_glasses = 1;
        }
        display.registerSegment(seg);
      }

    }

    // pour taches de fond
    delayMicroseconds(10);

  } // fin for

  if (order_glasses == 0) {
    Serial3.println(Nordic::encodeOrder(5., 5.));
    order_glasses = 1;
  }

  att.next = min_dist_seg;

  time_c = millis() - start;

#ifdef __DEBUG__
  Serial.print("Next Seg: "); Serial.print(min_dist_seg); Serial.print("  ");
  Serial.print("Nb seg actifs: "); Serial.print(att.nbact); Serial.print("  ");
  
  Serial.print(F("Temps de la boucle:  ")); Serial.print(time_c); Serial.println(F("ms"));
  //att.pwr = 425;
  //att.speed = 28.5;
#endif
}

void boucle_simu () {

    static float avance = -15.;

    Serial.println("Boucle simu");

    Serial3.println(Nordic::encodeOrder(avance, 100));

    delay(100);

    errorTone();

    avance += 1.;

    if (avance > 15.) {
      avance = -15.;
    }
  
}

