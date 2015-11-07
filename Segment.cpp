
#include <math.h>
#include "Segment.h"

Segment::Segment(void) {

}

Segment::Segment(const char *nom_seg) {
  if (nom_seg)
        _nomFichier = nom_seg;
}

ListeSegments::ListeSegments(void) {

}

Point *Segment::getFirstPoint() {
  return _lpts.getFirstPoint();
}

void Segment::setSegmentName(const char *name_) {

    if (name_)
        _nomFichier = name_;

    return;
}

void Segment::emptyName() {
  _nomFichier = "";
}

float Segment::getAvance() {
  return _monAvance;
}

const char* Segment::getName() {
  return _nomFichier.c_str();
}

int Segment::isValid() {
  if (_nomFichier.length() != 0) {
    return 1;
  } else {
    return 0;
  }
  
}

void Segment::desallouerPoints() {

    _lpts.vider();

    return;
}


void Segment::ajouterPointFin(float lat, float lon, float alt, float msec) {

    _lpts.ajouteFin(lat, lon, alt, msec);

    return;
}

void Segment::ajouterPointDebutIso(float lat, float lon, float alt, float msec) {

    _lpts.ajouteDebut(lat, lon, alt, msec);
    _lpts.supprLast();

}



void Segment::toString() {

    //printf("Segment:\nName: %s\nSize: %d\n", _nomFichier.c_str(), _lpts.longueur());
    _lpts.toString();

}

Vecteur Segment::posAuSegment(Point point) {
    return _lpts.posRelative(point);
}

int Segment::testActivation(ListePoints *liste) {

    float distP1P2, distP1, distP2, p_scal, distQuad;
    Point P1, P2, PPc, PPp;
    Vecteur PC, PS;

    if (_lpts.longueur() <= 3 || liste->longueur() <= 2) {
        return 0;
    }

    // position courante
    Point *test = liste->getFirstPoint();
    PPc = test;
    PPp = liste->getPointAt(1);

    P1 = _lpts.getFirstPoint();
    // premier point
    P2 = _lpts.getPointAt(1);

    distP1 = P1.dist(&PPc);
    distP2 = P2.dist(&PPc);
    distP1P2 = P1.dist(&P2);

    distQuad = P1.dist(&PPc);

    if (distQuad > DIST_ACT) return 0;

    PC = Vecteur(PPp, PPc);
    PS = Vecteur(P1, P2);

    p_scal = PC._x * PS._x + PC._y * PS._y;

    if (sqrt(PC._x * PC._x + PC._y * PC._y) * sqrt(PS._x * PS._x + PS._y * PS._y) > 0.001) {
        p_scal /= sqrt(PC._x * PC._x + PC._y * PC._y);
        p_scal /= sqrt(PS._x * PS._x + PS._y * PS._y);
    } else {
        p_scal = -10.;
    }

    if (distP2 * distP2 < distP1 * distP1 + distP1P2 * distP1P2 && p_scal > 0.5) {
        return 1;
        //Serial.println(F("Test positif"));
    } else {
        return 0;
        //Serial.println(F("Segment toujours inactif"));
    }

}

int Segment::testDesactivation(ListePoints *liste) {

    float distP1P2, distP1, distP2;
    Point *P1, *P2, *PPc;

    if (_lpts.longueur() <= 3 || liste->longueur() <= 2) {
        return 0;
    }

    // position courante
    PPc = liste->getFirstPoint();

    P1 = _lpts.getPointAt(-2);
    distP1 = P1->dist(PPc);
    // dernier point
    P2 = _lpts.getPointAt(-1);

    
    distP2 = P2->dist(PPc);
    distP1P2 = P1->dist(P2);

    //Serial.print("distP1  "); Serial.println(distP1);
    //Serial.print("distP2  "); Serial.println(distP2);

    if (distP1 * distP1 > distP2 * distP2 + distP1P2 * distP1P2) {
        return 1;
    }
    else {
        return 0;
        //Serial.println(F("Test negatif"));
    }

}


void Segment::majPerformance(ListePoints *mes_points) {

    int activable, desactivable;
    Point pc;
    Vecteur vect;
    Vecteur delta;

    if (mes_points->longueur() < 2) {
        return;
    }
    
    pc = *mes_points->getFirstPoint();
    
    if (!pc.isValid()) {
        return;
    }

    activable = testActivation(mes_points);
    
    delta = _lpts.deltaListe();

    if (_actif == SEG_OFF) {
        if (activable > 0) {
            // activation
            pc = getFirstPoint();
            vect = mes_points->posRelative(pc);

            _monStart = vect._t;
            _monCur = vect._t;

            _monElev0 = vect._z;
            _monPElev = 0.;
            _elevTot = delta._z;
            _monAvance = 0.;
            _actif = SEG_START;

            //            Serial.print(F("Segment active : "));
            //            Serial.println(mes_segments[i].nomFichier);
        }
    } else if (_actif > SEG_OFF) {
        // deja actif
        if (_actif == SEG_START) {
          _actif = SEG_ON;
        }

        desactivable = testDesactivation(mes_points);

        if (desactivable == 0) {
            // on met a jour la perfo
            
            vect = posAuSegment(pc);

            if (vect._y < MARGE_ACT * DIST_ACT) {

                _monCur = vect._t;
                _monAvance = _monStart + vect._t - pc._rtime;

                _monElev0 = _lpts.getFirstPoint()->_alt;

                if (_elevTot > 5.) {
                    _monPElev = vect._z;
                    _monPElev -= pc._alt;
                    _monPElev /= _elevTot;
                }
                
            } else {

                //                Serial.print(F("Desactivation de "));
                //                Serial.println(mes_segments[i].nomFichier);
                _actif = SEG_FIN;
            }

        } else {
            // on doit desactiver

            Point lp = *_lpts.getLastPoint();
            
            if (!lp.isValid()) {
                desallouerPoints();
                _actif = SEG_OFF;
                return;
            }

            // position relative du dernier point segment / mes points
            vect = mes_points->posRelative(lp);

            _monCur = vect._t;
            _monAvance = _monStart - vect._t + delta._t;
            _actif = SEG_FIN;

            Serial.print("Avance: ");
            Serial.println(_monAvance);
        }

    } else if (_actif < SEG_OFF) {
      _actif += 1;
    }



}


int Segment::nomCorrect() {

    int i;

    if (_nomFichier.length() == 0)
        return 0;

    if (_nomFichier.endsWith(".CRS")) {
        return 1;
    }

    if (!strstr(_nomFichier.c_str(), "#") || !strstr(_nomFichier.c_str(), ".")) {
        return 0;
    }

    if (strlen(_nomFichier.c_str()) != 12) {
        return 0;
    }

    for (i = 0; i < 12; i++) {

        if (i == 5) {
            if (_nomFichier.c_str()[i] != '#')
                return 0;
        } else if (i == 8) {
            if (_nomFichier.c_str()[i] != '.')
                return 0;
        } else {
            if (_nomFichier.c_str()[i] < '0' || _nomFichier.c_str()[i] > 'Z' || (_nomFichier.c_str()[i] > '9' && _nomFichier.c_str()[i] < 'A')) {
                return 0;
            }
        }
    }

    return 1;
}

