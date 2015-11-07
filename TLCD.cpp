
#include "TLCD.h"
#include "utils.h"
#include "Segment.h"

/* ************* */
/* CONSTRUCTORS  */
/* ************* */
TLCD::TLCD(uint8_t ss) : TSharpMem(SPI_CLK, SPI_MOSI, ss) {

  _seg_act = 0;
  _nb_lignes_tot = 7;
  _mode_prec = MODE_SD;
  _mode = MODE_CRS;

}

void TLCD::registerSegment(Segment *seg) {

  if (_seg_act < 2 && seg->getStatus() != SEG_OFF) {
    _l_seg[_seg_act] = seg;
    _seg_act += 1;
  }
}

void TLCD::updatePos(float lat_, float lon_, float alt_) {
  _lat = lat_;
  _lon = lon_;
  _alt = alt_;

}

void TLCD::cadranH(uint8_t p_lig, const char *champ, String  affi, const char *p_unite) {

  int decal = 0;
  int x = LCDWIDTH / 2 * 0.5;
  int y = LCDHEIGHT / NB_LIG * (p_lig - 1);

  setTextColor(CLR_NRM); // 'inverted' text
  setCursor(5, y + 5);
  setTextSize(1);

  if (champ) print(champ);

  if (affi.length() < 4) {
    decal = (4 - affi.length()) * 20;
  }
  setCursor(x + 20 + decal, y + 20);
  setTextSize(3);
  print(affi);

  setTextSize(1);
  x = LCDWIDTH / 2;
  setCursor(x + 105, y + 5);// y + 42

  if (p_unite) print(p_unite);
}


void TLCD::cadran(uint8_t p_lig, uint8_t p_col, const char *champ, String  affi, const char *p_unite) {

  int decal = 0;
  int x = LCDWIDTH / 2 * (p_col - 1);
  int y = LCDHEIGHT / NB_LIG * (p_lig - 1);

  setTextColor(CLR_NRM); // 'inverted' text
  setCursor(x + 5, y + 5);
  setTextSize(1);

  if (champ) print(champ);

  if (affi.length() < 6) {
    decal = (4 - affi.length()) * 14;
  } else if (affi.length() > 5) {
    affi = "---";
  }
  setCursor(x + 25 + decal, y + 23);
  setTextSize(3);
  print(affi);

  setTextSize(1);
  setCursor(x + 95, y+5);// y + 42

  if (p_unite) print(p_unite);
}

void TLCD::traceLignes(void) {

  switch (_seg_act) {
    case 0:
      traceLignes_NS();
      break;
    case 1:
      traceLignes_1S();
      break;
    case 2:
      traceLignes_2S();
      break;
  }

}

void TLCD::traceLignes_1S(void) {

  uint8_t ind1;

  drawFastVLine(LCDWIDTH / 2, 0, LCDHEIGHT / NB_LIG * 4, BLACK);

  for (ind1 = 0; ind1 < NB_LIG - 1; ind1++) {
    if (ind1 != NB_LIG - 3)
      drawFastHLine(0, LCDHEIGHT / NB_LIG * (ind1 + 1), LCDWIDTH, BLACK);
  }
}

void TLCD::traceLignes_2S(void) {

  uint8_t ind1;

  drawFastVLine(LCDWIDTH / 2, 0, LCDHEIGHT / NB_LIG * 2, BLACK);

  for (ind1 = 0; ind1 < NB_LIG - 1; ind1++) {
    if (ind1 != NB_LIG - 3 && ind1 != NB_LIG - 6)
      drawFastHLine(0, LCDHEIGHT / NB_LIG * (ind1 + 1), LCDWIDTH, BLACK);
  }
}

void TLCD::traceLignes_NS(void) {

  uint8_t ind1;

  drawFastVLine(LCDWIDTH / 2, 0, LCDHEIGHT * 4 / _nb_lignes_tot, BLACK);
  drawFastVLine(LCDWIDTH / 2, LCDHEIGHT * 5 / _nb_lignes_tot, LCDHEIGHT, BLACK);

  for (ind1 = 0; ind1 < NB_LIG - 1; ind1++)
    drawFastHLine(0, LCDHEIGHT / NB_LIG * (ind1 + 1), LCDWIDTH, BLACK);

}

void TLCD::updateAll(SAttitude *att_) {
  updatePos(att_->lat, att_->lon, att_->alt);
  memcpy(&att, att_, sizeof(SAttitude));
}

void TLCD::updateScreen(void) {

  clearBuffer();

  switch (_mode) {
    case MODE_SD:
      break;
    case MODE_GPS:
      break;
    case MODE_CRS:
      afficheSegments();
      break;
    case MODE_HRM:
      break;
    case MODE_HT:
      break;
    case MODE_MENU:
      break;
  }

  affiANCS();
  refresh();
  _seg_act = 0;
}

void TLCD::afficheSegments(void) {
  float vmoy = 0.;
  uint8_t hrs=0, mns=0, scs=0;
  String mins="00";

  if (att.nbpts-MIN_POINTS > 0) {
    vmoy = att.dist / (att.nbpts-MIN_POINTS) * 3.6;
    hrs = (float)att.nbpts / 3600.;
    mns = att.nbpts % 3600;
    mns = mns / 60;
    scs = att.nbpts % 60;
    if (mns < 10) mins = "0";
    else mins = "";
    mins.append(String(mns));
  }

  if (_seg_act == 0) {

    _nb_lignes_tot = 7;
    // ligne colonne
    cadran(1, 1, "Dist", String(att.dist / 1000., 1), "km");
    cadran(1, 2, "Pwr", String("425"), "W");
    cadran(2, 1, "Speed", String(att.speed, 1), "km/h");
    cadran(2, 2, "Climb", String(att.climb, 0), "m");
    cadran(3, 1, "CAD", String(att.cad_rpm), "rpm");
    cadran(3, 2, "HRM", String(att.bpm), "bpm");
    cadran(4, 1, "PR", String(att.nbpr), 0);
    cadran(4, 2, "KOM", String(att.nbkom), 0);
    cadranH(5, "Next seg", String(att.next), "m");
    cadran(6, 1, "Vmoy", String(att.cad_speed, 2), "km/h");
    cadran(6, 2, "Dur", String(hrs) + ":" + mins, 0);
    cadran(7, 1, "Batt", String(att.pbatt), "%");

    hrs = att.secj / 3600.;
    mns = att.secj  - hrs * 3600;
    mns = mns / 60;
    if (mns < 10) mins = "0";
    else mins = "";
    mins += mns;
    
    cadran(7, 2, "Time", String(hrs) + ":" + mins, 0);

    traceLignes();
  } else if (_seg_act == 1) {

    _nb_lignes_tot = 7;
    cadran(1, 1, "Dist", String(att.dist / 1000., 1), "km");
    cadran(1, 2, "Pwr", String(att.pwr), "W");
    cadran(2, 1, "Speed", String(att.speed, 1), "km/h");
    cadran(2, 2, "Climb", String(att.climb, 0), "m");
    cadran(3, 1, "CAD", String(att.cad_rpm), "rpm");
    cadran(3, 2, "HRM", String(att.bpm), "bpm");
    cadran(4, 1, "PR", String(att.nbpr), 0);
    cadran(4, 2, "KOM", String(att.nbkom), 0);
    
    traceLignes();

    partner(_l_seg[0]->getAvance(), 55., NB_LIG);
    afficheListePoints(NB_LIG - 2, 0, 0);

  } else if (_seg_act == 2) {

    _nb_lignes_tot = 8;
    // ligne colonne
    cadran(1, 1, "Speed", String(att.speed, 1), "km/h");
    cadran(1, 2, "Pwr", String(att.pwr), "W");
    cadran(2, 1, "CAD", String(att.cad_rpm), "rpm");
    cadran(2, 2, "HRM", String(att.bpm), "bpm");

    
    partner(_l_seg[0]->getAvance(), 55., NB_LIG - 3);
    afficheListePoints(NB_LIG - 5, 0, 0);

    partner(_l_seg[1]->getAvance(), 55., NB_LIG);
    afficheListePoints(NB_LIG - 2, 1, 0);
    traceLignes();

  }
}


void TLCD::partner(float rtime, float curtime, uint8_t ligne) {

  int hl, ol, dixP;
  float indice = rtime / curtime;
  static int centre = LCDWIDTH / 2;

  if (curtime < 2.) {
    return;
  }

  hl = LCDHEIGHT / NB_LIG * (ligne - 1) + 25;
  ol = 30;

  //fillRect(centre - 8, hl - 8, 18, 18, WHITE);
  fillRect(ol, hl, LCDWIDTH - 2 * ol, 3, BLACK);

  // on fait apparaitre l'indice de performance en %
  // -> si -0.25 centre en ol
  // -> si  0.25 centre en 240-ol
  //centre = ol + (LCDWIDTH - ol - ol) * (0.25 + indice) / (0.5);
  centre = regFenLim(indice, -0.25, 0.25, ol, LCDWIDTH - ol);
  if (centre < ol) {
    centre = ol;
  }
  else if (centre > LCDWIDTH - ol) {
    centre = LCDWIDTH - ol;
  }

  dixP = (LCDWIDTH - 2.*ol) * 10. / 50.;
  drawFastVLine(LCDWIDTH / 2 - dixP, hl + 2, 7, BLACK);
  drawFastVLine(LCDWIDTH / 2 + dixP, hl + 2, 7, BLACK);

  fillTriangle(centre - 7, hl + 7, centre, hl - 7, centre + 7, hl + 7, BLACK);
  setCursor(centre - 13, hl + 15);
  setTextSize(NB_LIG > 7 ? 1:2);
  setTextColor(CLR_NRM); // 'inverted' text
  print(String((int)(indice * 100.)));
  

  // marques
  drawFastVLine(LCDWIDTH / 2, hl - 12, 12, BLACK);

}



void TLCD::afficheListePoints(uint8_t ligne, uint8_t ind_seg, uint8_t mode) {

  int posTrouve = 0;
  float minLat = 100.;
  float minLon = 400.;
  float minAlt = 10000.;
  float maxLat = -100.;
  float maxLon = -400.;
  float maxAlt = -100.;
  float curDist = 0.;
  float dDist = 0.;
  float maxDist = 0.;
  float maDist = 10000.;
  Point *pCourant, *pSuivant;
  Point *maPos;
  float maDpex = 0;
  float maDpey = 0;
  std::list<Point>::iterator _iter;
  ListePoints *liste;

  if (_l_seg[ind_seg]->longueur() < 5) return;


  uint16_t debut_cadran = LCDHEIGHT / NB_LIG * (ligne - 1);
  uint16_t fin_cadran   = LCDHEIGHT / NB_LIG * (ligne + 1);

  // on cherche la taille de fenetre
  liste = _l_seg[ind_seg]->getListePoints();
  maPos = liste->getFirstPoint();
  pCourant = maPos;
  for (_iter = liste->getLPTS()->begin(); _iter != liste->getLPTS()->end();) {

    pCourant = _iter.operator->();
    _iter++;
    if (_iter == liste->getLPTS()->end()) break;
    pSuivant = _iter.operator->();

    if (!pSuivant->isValid() || !pCourant->isValid()) break;

    if (pCourant->_lat < minLat) minLat = pCourant->_lat;
    if (pCourant->_lon < minLon) minLon = pCourant->_lon;
    if (pCourant->_alt < minAlt) minAlt = pCourant->_alt;

    if (pCourant->_lat > maxLat) maxLat = pCourant->_lat;
    if (pCourant->_lon > maxLon) maxLon = pCourant->_lon;
    if (pCourant->_alt > maxAlt) maxAlt = pCourant->_alt;

    if (maDist > distance_between(pCourant->_lat, pCourant->_lon, _lat, _lon) && _l_seg[ind_seg]->getStatus() > SEG_OFF) {
      maDist = distance_between(pCourant->_lat, pCourant->_lon, _lat, _lon) + 0.1;
      maPos = pCourant;
    }

    maxDist += distance_between(pCourant->_lat, pCourant->_lon, pSuivant->_lat, pSuivant->_lon);

  }

  if (_l_seg[ind_seg]->getStatus() < SEG_OFF) {
    maPos = pCourant;
  }

  while (maxLon - minLon < (maxLat - minLat)) {
    minLon -= 0.0008;
    maxLon += 0.0008;
  }

  while (maxLat - minLat < 0.5 * (maxLon - minLon)) {
    minLat -= 0.0014;
    maxLat += 0.0014;
  }

  // marge TODO
  minLat -= 0.0014;
  minLon -= 0.0008;
  minAlt -= 15.;
  maxLat += 0.0014;
  maxLon += 0.0008;
  maxAlt += 15.;
  maxDist += 50.;

  curDist = 0.;
  // on affiche
  for (_iter = liste->getLPTS()->begin(); _iter != liste->getLPTS()->end();) {

    pCourant = _iter.operator->();
    _iter++;
    if (_iter == liste->getLPTS()->end()) break;
    pSuivant = _iter.operator->();

    if (!pSuivant->isValid() || !pCourant->isValid()) break;

    dDist = distance_between(pCourant->_lat, pCourant->_lon, pSuivant->_lat, pSuivant->_lon);

    if (mode == 0) {
      // mode segment
      drawLine(regFenLim(pCourant->_lon, minLon, maxLon, 0, LCDWIDTH),
               regFenLim(pCourant->_lat, minLat, maxLat, fin_cadran, debut_cadran),
               regFenLim(pSuivant->_lon, minLon, maxLon, 0, LCDWIDTH),
               regFenLim(pSuivant->_lat, minLat, maxLat, fin_cadran, debut_cadran), BLACK);
    } else {
      // altitude
      drawLine(regFenLim(curDist, -50., maxDist, 0, LCDWIDTH),
               regFenLim(pCourant->_alt, minAlt, maxAlt, fin_cadran, debut_cadran),
               regFenLim(curDist + dDist, -50., maxDist, 0, LCDWIDTH),
               regFenLim(pSuivant->_alt, minAlt, maxAlt, fin_cadran, debut_cadran), BLACK);
    }

    curDist += dDist;
    if (pCourant == maPos) posTrouve = 1;
    if (posTrouve == 0) {
      maDist = curDist;
    }
  }

  drawCircle(regFenLim(pCourant->_lon, minLon, maxLon, 0, LCDWIDTH),
             regFenLim(pCourant->_lat, minLat, maxLat, fin_cadran, debut_cadran), 5, BLACK);

  if (_l_seg[ind_seg]->getStatus() < SEG_OFF) {
    _lon = pCourant->_lon;
    _lat = pCourant->_lat;
    _alt = pCourant->_alt;
  }

  // ma position
  if (mode == 0) {
    // mode segment
    maDpex = regFenLim(_lon, minLon, maxLon, 0, LCDWIDTH);
    maDpey = regFenLim(_lat, minLat, maxLat, fin_cadran, debut_cadran);
  } else {
    // altitude
    maDpex = regFenLim(maDist, -50., maxDist, 0, LCDWIDTH);
    maDpey = regFenLim(_alt, minAlt, maxAlt, fin_cadran, debut_cadran);
  }
  fillCircle(maDpex, maDpey, 4, BLACK);


  setTextColor(CLR_NRM); // 'inverted' text

  if (maDpey > fin_cadran - 30) {
    setCursor(maDpex > LCDWIDTH - 70 ? LCDWIDTH - 70 : maDpex, maDpey - 20);
  } else {
    setCursor(maDpex > LCDWIDTH - 70 ? LCDWIDTH - 70 : maDpex, maDpey + 15);
  }

  setTextSize(2);

  print(String(_l_seg[ind_seg]->getAvance(), 1));
  // completion
  setCursor(10, debut_cadran + 10);
  print((int)(maDist / curDist * 100.));
  print("%");
}

void TLCD::affiANCS() {
  if (_ancs_mode > 0) {
    fillRect(0, 0, LCDWIDTH, LCDHEIGHT / NB_LIG - 2, WHITE);
    setCursor(5, 5);
    setTextSize(2);
    setTextColor(CLR_NRM); // 'inverted' text
    print(att.ancs_msg);
  }
  decrANCS();
  return;
}

uint8_t TLCD::calculMode() {

  return MODE_CRS;
}


