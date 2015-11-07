#include <TinyGPS.h>
#include <Adafruit_GFX.h>
#include <SdFat.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <list>
#include "TSharpMem.h"
#include "TLCD.h"
#include "Nordic.h"
#include "Merites.h"
#include "ListePoints.h"
#include "Segment.h"
#include "define.h"


//using namespace std;

TLCD display(sd_sharp);

TinyGPS gps;

Nordic nordic;

Merite cumuls;

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);



void setup() {
  // initialize the digital pin as an output.
  Serial.begin(115200);
  Serial1.begin(4800);
  Serial2.begin(115200);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);

  display.begin();

  display.clearDisplay();

  while (!Serial) delay(500);

  Serial.println("Debut");

  if (!sd.begin(sd_cs, SPI_HALF_SPEED)) {
    Serial.println(F("Card initialization failed."));
    tone(speakerPin, 6440, 2000);
    delay(2000);
    noTone(speakerPin);
    while (1) delay(1000);
  }
  else {
    Serial.println(F("Card OK."));
    initListeSegments();
    Serial.print("Nombre total de segments: ");
    Serial.println(mes_segments.size());
  }

  memset(&att, 0, sizeof(SAttitude));
}

void serialEvent1() {
  char c;
  while (Serial1.available()) {
    c = Serial1.read();
    //Serial.write(c);
    if (gps.encode(c)) {
      new_gps_data = 1;
    }
  }
}

void serialEvent2() {
  char c;
  while (Serial2.available()) {
    c = Serial2.read();
    //Serial.write(c);
    switch (nordic.encode(c)) {
      case _SENTENCE_HRM:
        new_hrm_data = 1;
        break;
      case _SENTENCE_CAD:
        new_cad_data = 1;
        break;
      case _SENTENCE_ANCS:
        new_ancs_data = 1;
        break;
    }
  }
}

// the loop routine runs over and over again forever:
void loop() {

  digitalWrite(led, LOW);
  yield();

  // maj nordic
  if (new_hrm_data) att.bpm = nordic.getBPM();
  if (new_cad_data) att.cad_rpm = nordic.getRPM();
  if (new_hrm_data) att.cad_speed = nordic.getSpeed();
  if (new_ancs_data) {
    att.ancs_type = nordic.getANCS_type();
    att.ancs_msg = nordic.getANCS_msg();
    otherTone();
    Serial.println(att.ancs_msg);
    display.notifyANCS();
  }
  
  new_gps_data = 0;
  new_hrm_data = 0;
  new_cad_data = 0;
  new_ancs_data = 0;

  switch (display.calculMode()) {
    case MODE_CRS:
      // recup infos gps
      gps.f_get_position(&att.lat, &att.lon, &age);
      if (!isGpsDataValid(att.lat, att.lon, age)) goto piege;
      att.nbpts++;

      att.speed = gps.f_speed_kmph();
      att.secj = get_sec_jour();
      mes_points.enregistrePos(att.lat, att.lon, att.alt, att.secj);

      // maj merites
      if (att.nbpts < MIN_POINTS) {
        // maj sharp
        Serial.println("Merites = 0");
        display.updateAll(&att);
        if (att.nbpts < 4) {
          basicShort();
        }
        goto piege;
      }
      att.dist = cumuls.getDistance();
      att.climb = cumuls.getClimb();
      
      boucle_outdoor();

      if (cumuls.majMerite(att.lat, att.lon, att.alt) == 1) {
        loggerData();
      }

      // maj sharp
      display.updateAll(&att);
      break;
    case MODE_HRM:
    case MODE_HT:
      break;
  }

piege:
  display.updateScreen();

  digitalWrite(led, HIGH);
  while (cond_wait() == 1) {
    delay(50);
  }

}

