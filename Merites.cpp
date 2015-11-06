/*
 *
 File:   Merites.cpp
 * Author: vincent
 *
 * Created on September 11, 2014, 11:01 AM
 */

#include "Merites.h"


Merite::Merite() {
  distance = 0.;
  climb = 0.;
  last_stored_ele = 0.;
  last_stored_lat = -100.;
  last_stored_lon = -200.;
  pr_won = 0;
  puissance = 0.;
}

// GETTERS

float Merite::getClimb() {
  return climb;
}

float Merite::getDistance() {
  return distance;
}

float Merite::getPower() {
  return puissance;
}

int Merite::getPR() {
  return pr_won;
}


// SETTERS

// maj des segments gagnes
void Merite::ajoutPR() {
  pr_won+=1;
}

// Mise a jour avec la pos GPS courante
int Merite::majMerite(float lat, float lon, float ele) {

  static float majDist = 0;
  static int res;

  res = 0;

  if (last_stored_lat < -90.) {
    // initialisation
    last_stored_lat = lat;
    last_stored_lon = lon;
    last_stored_ele = ele;
    distance = 0.;
    climb = 0.;
  }

  majDist = distance_between(lat, lon, last_stored_lat, last_stored_lon);

  // mise a jour de la distance parcourue
  // tous les 10 metres
  if (majDist > 10.) {
    distance += majDist;
    last_stored_lat = lat;
    last_stored_lon = lon;
    res = 1;
  }

  // hysteresis
  if (res == 1) {
    if (ele > last_stored_ele + 1.) {
      // mise a jour de la montee totale
      climb += ele - last_stored_ele;
      last_stored_ele = ele;
    } 
    else if (ele + 1. < last_stored_ele) {
      // on descend, donc on garde la derniere alti
      // la plus basse
      last_stored_ele = ele;
    }
  }

  return res;
}


void Merite::majPower(ListePoints *mes_points, float speed_) {

  static float fHp = 0.;
  static float MASSE = 80.;
  static float fSpeed = -1.;

  if (mes_points)
    if (mes_points->getElevTot() < -2.) {

      // STEP 1 : on filtre altitude et vitesse
      //  =>l'elevation de la liste est inversee, le temps aussi
      fHp = 0.7 * fHp + 0.3 * mes_points->getElevTot() / mes_points->getTempsTot();

      if (fSpeed < -0.5) {
        // on init
        fSpeed = speed_ / 3.6;
      } 
      else {
        fSpeed = 0.7 * fSpeed +  0.3 * speed_ / 3.6;
      }

      // STEP 2 : Calcul
      //    W = 0,24 Ã— v^3 + 0,137 Ã— m Ã— v + 9,81 Ã— m Ã— dh/dt
      puissance = 9.81 * MASSE * fHp; // grav
      puissance += 0.137 * MASSE * fSpeed; // sol + meca
      puissance += 0.24 * fSpeed * fSpeed * fSpeed; // air

      if (fSpeed < 1.5 || fSpeed > 40. || puissance < 20. || fHp < -0.05)
        puissance = -100.;
    }

  return;
}






