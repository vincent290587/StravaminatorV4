


uint8_t cond_wait () {

  switch (display.calculMode()) {
    case MODE_GPS:
    case MODE_CRS:
      if (new_gps_data != 0) return 0;
      break;
    case MODE_HRM:
      if (new_hrm_data != 0) return 0;
      break;
    case MODE_HT:
      if (new_cad_data != 0) return 0;
      break;
  }

  return 1;
}


void boucle_outdoor () {

  float min_dist_seg = 5000;
  float tmp_dist;

  resetdelay_();

  std::list<Segment>::iterator _iter;
  Segment *seg;

  att.nbact = 0;
  for (_iter = mes_segments._segs.begin(); _iter != mes_segments._segs.end(); _iter++) {

    seg = _iter.operator->();

    if (seg->isValid()) {
      
      tmp_dist = watchdog (seg, att.lat, att.lon);
      if (tmp_dist < min_dist_seg && seg->getStatus() == SEG_OFF) min_dist_seg = tmp_dist;

      seg->majPerformance(&mes_points);

      if (seg->getStatus() != SEG_OFF) {
        att.nbact+=1;
        if (seg->getStatus() == SEG_START) {
          Serial.println("Segment commence");
          segStartTone ();
        } else if (seg->getStatus() == SEG_FIN) {
          Serial.println("Segment termine");
          if (seg->getAvance() > 0.) {
            att.nbpr++;
            att.nbkom++;
            segEndTone();
          } else {
            
          }
        }
        Serial.print("Register ");
        Serial.println(seg->getName());
        display.registerSegment(seg);
      }

    }

  // pour taches de fond
  delayMicroseconds(10);

  } // fin for

  att.next = min_dist_seg;

#ifdef __DEBUG__
  Serial.print("Next Seg: ");Serial.print(min_dist_seg);Serial.print("   FreeRam:  ");Serial.print(myFreeRam());Serial.print("   ");
  printTime();
  att.pbatt = 96;
  att.climb = 1256;
  att.pwr = 425;
  Serial2.println("$HRM,172,1082");
  Serial2.println("$CAD,79,32.6");
  if (att.nbpts % 70 == 0)
    Serial2.println("$ANCS,3,Bonjour mon vincounet");
#endif
}



