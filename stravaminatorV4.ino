#include <TinyGPS.h>
#include <Adafruit_GFX.h>
#include <SdFat.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <STC3100.h>
#include <list>
#include <avr/sleep.h>
#include <Nordic.h>
#include <IntelliScreen.h>
#include <SPIFFS25.h>
#include "TSharpMem.h"
#include "TLCD.h"
#include "Merites.h"
#include "ListePoints.h"
#include "Segment.h"
#include "Parcours.h"
#include "define.h"


TLCD display(sd_sharp);

TinyGPS gps;

Nordic nordic;

Merite cumuls;

SPIFFS25 sst;

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

STC3100 stc = STC3100(0x70);

void setup() {
  att.has_started = 0;
  // initialize the digital pin as an output.
  Serial.begin(115200);
  Serial1.begin(9600);
  Serial3.begin(115200);
  pinMode(led, OUTPUT);
  digitalWriteFast(led, HIGH);

  memset(&att, 0, sizeof(SAttitude));

  delay(50);
  display.begin();
  delay(5);
  display.resetBuffer();
  display.updateScreen();
  display.registerHisto(&mes_points);

#ifdef __DEBUG__
  uint8_t safe = 0;
  //digitalWriteFast(led, LOW);
  while (!Serial) {
    delay(1000);
    safe++;
    if (safe > 100) {
      Serial.end();
      break;
    }
  }
#endif

  Serial.println("Debut");

  /* Initialise the sensor */
  if (!bmp.begin())
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.println(F("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!"));
  }

  /* Initialise the sensor */
  if (!stc.begin(25))
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.println("Ooops, no STC detected ... Check your wiring or I2C ADDR!");
  }

  // on demarre la SPI flash
  sst.begin(memCs, memWp, memHold);

  sst.globalUnlock();
#ifdef __DEBUG__
  dumpLogGPS();
  sst.totalErase();
#endif

  if (!sd.begin(sd_cs, SPI_HALF_SPEED)) {
    Serial.println(F("Card initialization failed."));
    display.setSD(-1);
    display.updateScreen();
    tone(speakerPin, 6440, 2000);
    delay(1000);
    display.setModeCalcul(MODE_HRM);
    display.setModeAffi(MODE_HRM);
    display.updateScreen();
  }
  else {
    Serial.println(F("Card OK."));
    display.setSD(1);
    display.updateScreen();
    delay(150);
    initListeSegments();
    Serial.print("Nombre total de segments: ");
    Serial.println(mes_segments.size());
    display.setNbSeg(mes_segments.size());
    display.updateScreen();
    delay(1000);
    display.setModeCalcul(MODE_GPS);
    display.setModeAffi(MODE_GPS);
    display.updateScreen();
    delay(400);
    Serial.println("Fin setup");
  }
  att.has_started = 1;
}


void serialEvent() {
  char c;
  uint8_t format = 0;
  while (Serial.available()) {
    c = Serial.read();
    if (mode_simu == 0) {
      format = nordic.encode(c);
      if (format == _SENTENCE_PC) {
        if (nordic.getPC() == 0) {
          // demande de telechargement de l'historique
          download_request = 1;
        } else if (nordic.getPC() == 1) {
          // demande de mise a jour des segments
          //upload_request = 1;
        } else if (nordic.getPC() == 2) {
          // demande de mise a jour des segments
          mode_simu = 1;
          Serial.println("Mode simu activé");
        }
      }
    } else {
      if (c == '#') {
        Serial.println("Mode simu desactivé");
        mode_simu = 0;
      }
      if (gps.encode(c)) {
        new_gps_data = 1;
      }
    }
  }
}

void serialEvent1() {
  char c;
  while (Serial1.available() && att.has_started > 0) {
    c = Serial1.read();
    //Serial.write(c);
    if (mode_simu == 0) {
      if (gps.encode(c)) {
        new_gps_data = 1;
      }
    }
  }
}

void serialEvent3() {
  char c;
  uint8_t format = 0;
  while (Serial3.available() && att.has_started > 0) {
    c = Serial3.read();
    Serial.write(c);
    format = nordic.encode(c);
    switch (format) {
      case _SENTENCE_HRM:
        new_hrm_data = 1;
        break;
      case _SENTENCE_CAD:
        new_cad_data = 1;
        break;
      case _SENTENCE_ANCS:
        new_ancs_data = 1;
        break;
      case _SENTENCE_DBG:
        new_dbg_data = 1;
        break;
      case _SENTENCE_NONE:
        break;
      default:
        Serial.println(String("Phrase non reconnue: ") + format);
        break;
    }
  }
}

// the loop routine runs over and over again forever:
void loop() {
#ifdef __DEBUG__
  digitalWriteFast(led, LOW);
  Serial.println("LOOP");
#endif

  // vidage des buffer Serials
  yield();

  // maj nordic
  if (new_hrm_data) {
    att.bpm = nordic.getBPM();
    att.rrint = nordic.getRR();
  }
  if (new_cad_data && nordic.getRPM()) att.cad_rpm = nordic.getRPM();
  if (new_hrm_data && nordic.getSpeed() > 0.) att.cad_speed = nordic.getSpeed();
  if (new_ancs_data) {
    if (alertes_nb < 20) {
      alertes_nb += 1;
    }
    String tmp = String ("") + nordic.getANCS_title() + nordic.getANCS_msg();
    if (tmp.indexOf("Appel") > 0 || tmp.indexOf("essage") > 0) otherTone();
    display.notifyANCS(nordic.getANCS_type(), nordic.getANCS_title(), nordic.getANCS_msg());
  }
  if (new_dbg_data) {
    String tmp = String("Erreur nRF51: code=") + String(nordic.getDBG_code(), HEX) + " - line:" + String(nordic.getDBG_line());
    loggerMsg(tmp.c_str());
    if (nordic.getDBG_type() == 1) loggerMsg(nordic.getDBG_msg());
  }
  if (download_request) {
    Serial.println("Debut MaJ");
    if (PChisto()) {
      Serial.println("Erreur lors de la MaJ");
    }
  }

  /* Get a new measurement */
  stc.getBatteryData(&batt_data);
  att.cbatt = batt_data.Current;
  att.pbatt = percentageBatt(batt_data.Voltage);
  att.vbatt = batt_data.Voltage;


#ifdef __DEBUG_STC__
  Serial.print("Temperature STC :    ");
  Serial.print(batt_data.Temperature, 2);
  Serial.println(" degres C");
  Serial.print("Current STC     :    ");
  Serial.print(batt_data.Current, 2);
  Serial.println(" mA");
  Serial.print("Voltage STC     :    ");
  Serial.print(batt_data.Voltage, 2);
  Serial.println(" V");
  Serial.print("Charge STC      :    ");
  Serial.print(batt_data.Charge, 2);
  Serial.println(" mAh");
#endif

  new_gps_data = 0;
  new_hrm_data = 0;
  new_cad_data = 0;
  new_ancs_data = 0;
  new_dbg_data = 0;
  download_request = 0;
  upload_request = 0;

  switch (display.getModeCalcul()) {
    case MODE_GPS:
      display.setNbSat(gps.satellites());
      display.setHDOP(gps.hdop());
    case MODE_CRS:
      // recup infos gps
      gps.f_get_position(&att.lat, &att.lon, &age);
      if (!isGpsDataValid(att.lat, att.lon, age)) goto piege;
      att.nbpts++;

      att.gpsalt = gps.f_altitude();
      att.speed = gps.f_speed_kmph();
      att.secj = get_sec_jour();
      
      // maj BMP
      updateAltitude(&att.alt);
      
      mes_points.enregistrePos(att.lat, att.lon, att.alt, att.secj);

      // maj merites
      if (att.nbpts < MIN_POINTS) {
        // maj sharp
        display.updateAll(&att);
        if (att.nbpts < 4) {
          basicShort();
        }
        goto piege;
      } else if (att.nbpts == MIN_POINTS) {
        // maj BMP
        updateAltitudeOffset(&att.alt);
        // maj
        display.setModeCalcul(MODE_CRS);
        display.setModeAffi(MODE_CRS);
        att.secj_prec = att.secj;
        att.nbsec_act = 1;
      }

      att.dist = cumuls.getDistance();
      att.climb = cumuls.getClimb();

      boucle_outdoor();

      if (cumuls.majMerite(att.lat, att.lon, att.alt) == 1) {
        loggerData();
        att.nbsec_act += att.secj - att.secj_prec;
        att.secj_prec = att.secj;
      } else if (att.speed < 3.) {
        att.secj_prec = att.secj;
      }

      att.pwr = cumuls.getPower();
      att.climb = cumuls.getClimb();
      // maj sharp
      //display.updateAll(&att);
      break;
    case MODE_SIMU:
    case MODE_HRM:
    case MODE_HT:
      //display.updateAll(&att);
      break;

  }

  switch (display.getModeAffi()) {
    case MODE_GPS:
    case MODE_CRS:
    case MODE_PAR:
      display.updateAll(&att);
      break;
    case MODE_SIMU:
      boucle_simu();
    case MODE_HRM:
    case MODE_HT:
      display.updateAll(&att);
      break;

  }

piege:
  display.updateScreen();

  digitalWriteFast(led, HIGH);
  while (cond_wait() == 1) {
    idle();
  }

}

void idle() {
  set_sleep_mode(SLEEP_MODE_IDLE);
  noInterrupts();
  sleep_enable();
  interrupts();
  asm("wfi");
  sleep_disable();
  yield();
}

void usage_fault_isr(void) {
  digitalWrite(led, HIGH);
  basicLong();
  CPU_RESTART
}

void software_isr(void) {
  digitalWrite(led, HIGH);
  basicLong();
  CPU_RESTART
}

void hard_fault_isr(void) {
  digitalWrite(led, HIGH);
  basicLong();
  CPU_RESTART
}


void buttonDownEvent () {
  display.buttonDownEvent();
}

void buttonUpEvent () {
  display.buttonUpEvent();
}


void buttonPressEvent () {
  display.buttonPressEvent();
}

void activerNavigateur() {
  return;
}

void desactiverNavigateur() {
  return;
}


