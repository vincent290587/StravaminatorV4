/* 
 * File:   utils.c
 * Author: vincent
 *
 * Created on October 27, 2015, 10:55 AM
 */


#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "utils.h"

#define FACTOR 100000.

float min(float val1, float val2) {
  if (val1 <= val2) return val1;
  else return val2;
}

float max(float val1, float val2) {
  if (val1 <= val2) return val2;
  else return val1;
}

float regFen(float val_, float b1_i, float b1_f, float b2_i, float b2_f) {
  
  float x, res;
  // calcul x
  x = (val_ - b1_i) / (b1_f - b1_i);
  
  // calcul valeur: x commun
  res = x * (b2_f - b2_i) + b2_i;
  return res;
}

float regFenLim(float val_, float b1_i, float b1_f, float b2_i, float b2_f) {
  
  float x, res;
  // calcul x
  x = (val_ - b1_i) / (b1_f - b1_i);
  
  // calcul valeur: x commun
  res = x * (b2_f - b2_i) + b2_i;
  if (res < min(b2_i,b2_f)) res = min(b2_i,b2_f);
  if (res > max(b2_i,b2_f)) res = max(b2_i,b2_f);
  return res;
}

float distance_between(float lat1, float long1, float lat2, float long2) {
  float delta = 3.141592 * (long1 - long2) / 180.;
  float sdlong = sin(delta);
  float cdlong = cos(delta);
  lat1 = 3.141592 * (lat1) / 180.;
  lat2 = 3.141592 * (lat2) / 180.;
  float slat1 = sin(lat1);
  float clat1 = cos(lat1);
  float slat2 = sin(lat2);
  float clat2 = cos(lat2);
  delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
  delta = delta*delta;
  delta += clat2 * sdlong * clat2 * sdlong;
  delta = sqrt(delta);
  float denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
  delta = atan2(delta, denom);
  return delta * 6369933;
}


char *calculeNom (float lat, float lon) {

    static char tableau[15];

    int iLat = (lat + 90.) * FACTOR + 0.5;
    int iLon = (lon + 180.) * FACTOR + 0.5;

    strncpy(tableau, toBase36(0, iLat, tableau), 5);

    tableau[5] = '#';

    strncpy(tableau + 6, toBase36(1, iLon, tableau), 6);

    tableau[12] = '\0';
    tableau[13] = '\0';

    return tableau;
}

void calculePos (const char *nom, float *lat, float *lon) {

    static char tab[15];
    int iLat;
    int iLon;
    
    if (!nom) {
      return;
    }

    strncpy(tab, nom, 5);
    tab[5] = '\0';
    iLat = toBase10(tab);
    if (lat) {
        *lat = (float) iLat / FACTOR - 90.;
    }

    strncpy(tab, nom + 6, 2);
    strncpy(tab + 2, nom + 9, 3);
    tab[5] = '\0';
    iLon = toBase10(tab);
    if (lon) {
        *lon = (float) iLon / FACTOR - 180.;
    }

    return;
}

char *toBase36 (int avecPoint, int aConv, char *sortie) {

    static char tab[15];

    if (!avecPoint) {
        strncpy(tab, base36enc(aConv), 5);
        tab[5] = '\0';
    } else {
        strncpy(tab, base36enc(aConv), 2);
        tab[2] = '.';
        strncpy(tab + 3, base36enc(aConv) + 2, 3);
        tab[6] = '\0';
    }

    return tab;
}

long unsigned int toBase10 (char *entree) {

    static char tab[15];

    if (!entree) {
        return 0;
    }
    
    if (!strstr(entree, ".")) {
        strncpy(tab, entree, 5);
        tab[5] = '\0';
    } else {
        strncpy(tab, entree, 2);
        strncpy(tab + 2, entree + 3, 3);
        tab[5] = '\0';
    }

    return strtoul(tab, NULL, 36);

}

char *base36enc (long unsigned int value) {

    static char base36[45] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ$$$$$$";
    /* log(2**64) / log(36) = 12.38 => max 13 char + '\0' */
    static char buffer[14];
    unsigned int offset = sizeof (buffer);

    memset(buffer, '0', 13 * sizeof (char));

    buffer[--offset] = '\0';
    do {
        buffer[--offset] = base36[value % 36];
    } while (value /= 36);

    offset = 8;
    return buffer + offset;

}
