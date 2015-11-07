/*
   File:   Nordic.h
   Author: vincent

   Created on November 5, 2015, 9:19 AM
*/

#ifndef NORDIC_H
#define	NORDIC_H

#include "WProgram.h"

#define MAX_SIZE 150

#define COMBINE(sentence_type, term_number) (((unsigned)(sentence_type) * 10) | term_number)

enum {
  _SENTENCE_NONE, _SENTENCE_OTHER, _SENTENCE_HRM, _SENTENCE_CAD, _SENTENCE_ANCS
};

class Nordic {
  public:

    Nordic();
    Nordic(const Nordic& orig);
    virtual ~Nordic();
    uint8_t encode(char c); // process one character received from GPS
    uint8_t encodeSentence(const char *sent_); // process one character received from GPS
    Nordic &operator<<(char c) {
      encode(c);
      return *this;
    }

    unsigned long getBPM() {
      return _bpm;
    }
    unsigned long getRR() {
      return _rr;
    }
    unsigned long getRPM() {
      return _rpm;
    }
    float getSpeed() {
      return _speed;
    }
    unsigned long getANCS_type() {
      return _ancs_type;
    }
    const char *getANCS_msg() {
      return _ancs_msg;
    }


  private:

    int nstrcmp(const char *str1, const char *str2);
    long natol(const char *str);
    bool isdigit(char c) {
      return c >= '0' && c <= '9';
    }
    unsigned long parse_decimal();
    uint8_t term_complete();

    unsigned long _bpm, _rr;
    unsigned long _rpm;
    unsigned long _ancs_type;
    float _speed;
    char _ancs_msg[MAX_SIZE];


    unsigned long _encoded_characters;

    // parsing state variables
    byte _parity;
    bool _is_checksum_term;
    char _term[MAX_SIZE];
    byte _sentence_type;
    byte _term_number;
    byte _term_offset;
    bool _gps_data_good;
};

#endif	/* NORDIC_H */

