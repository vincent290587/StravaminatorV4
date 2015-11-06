/* 
 * File:   utils.h
 * Author: vincent
 *
 * Created on October 27, 2015, 10:55 AM
 */

#ifndef UTILS_H
#define	UTILS_H

#ifdef	__cplusplus
extern "C" {
#endif

float regFen(float val_, float b1_i, float b1_f, float b2_i, float b2_f);

float regFenLim(float val_, float b1_i, float b1_f, float b2_i, float b2_f);

float distance_between(float lat1, float long1, float lat2, float long2);

char *calculeNom (float lat, float lon);

void calculePos (const char *nom, float *lat, float *lon);

char *toBase36 (int avecPoint, int aConv, char *sortie);

long unsigned int toBase10 (char *entree);

char *base36enc (long unsigned int value);

#ifdef	__cplusplus
}
#endif

#endif	/* UTILS_H */

