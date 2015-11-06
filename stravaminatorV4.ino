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
#include "ListePoints.h"
#include "Segment.h"
#include "define.h"


//using namespace std;

TLCD display(sd_sharp);

TinyGPS gps;

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);





void setup() {
  // initialize the digital pin as an output.
  Serial.begin(115200);
  Serial1.begin(4800);
  Serial3.begin(115200);
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
  }

  
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

// the loop routine runs over and over again forever:
void loop() {

  digitalWrite(led, LOW);
  // emptySerials();
  new_gps_data = 0;
  switch (display.calculMode()) {
    case MODE_CRS:
      gps.f_get_position(&att.lat, &att.lon, &age);
      if (!isGpsDataValid(att.lat, att.lon, age)) return;
      att.secj = get_sec_jour();
      mes_points.enregistrePos(att.lat, att.lon, att.alt, att.secj);
      display.updatePos(att.lat, att.lon, att.alt);
      boucle_outdoor();
      break;
    case MODE_HRM:
    case MODE_HT:
      break;
  }

  display.updateAll();

  digitalWrite(led, HIGH);
  while(cond_wait()==1) {
    delay(50);
  }

}

