/*
   File:   Merites.hpp
   Author: vincent

   Created on September 11, 2014, 9:25 AM
*/

#ifndef MERITES_HPP
#define	MERITES_HPP

#include "ListePoints.h"

class Merite {

  public:
    Merite();
    int majMerite(float lat, float lon, float ele);
    float getDistance();
    float getClimb();
    float getPower();
    void majPower(ListePoints *mes_points, float speed_);

  private:
    float distance;
    float climb;
    float puissance;
    float last_stored_ele;
    float last_stored_lat;
    float last_stored_lon;
    int pr_won;

};



#endif	/* MERITES_HPP */



