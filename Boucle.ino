


uint8_t cond_wait () {

  switch (display.calculMode()) {
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

  //Serial.println("Boucle OUTDOOR");

  for (_iter = mes_segments._segs.begin(); _iter != mes_segments._segs.end(); _iter++) {

    seg = _iter.operator->();

    if (seg->isValid()) {
      
      tmp_dist = watchdog (seg, att.lat, att.lon);
      if (tmp_dist < min_dist_seg) min_dist_seg = tmp_dist;

      seg->majPerformance(&mes_points);

      if (seg->getStatus() != SEG_OFF) {
        if (seg->getStatus() == SEG_START) {
          Serial.println("Segment commence");
          segStartTone ();
        } else if (seg->getStatus() == SEG_FIN) {
          Serial.println("Segment termine");
          segEndTone();
        }
        Serial.print("Register ");
        Serial.println(seg->getName());
        display.registerSegment(seg);
      }

    }


  }

  Serial.print("Next Seg: ");Serial.print(min_dist_seg);Serial.print("   FreeRam:  ");Serial.print(myFreeRam());Serial.print("   ");

  printTime();
}



