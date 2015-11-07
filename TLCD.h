
#ifndef _TLCD_
#define _TLCD_

#include <TinyGPS.h>
#include "TSharpMem.h"
#include "Segment.h"
#include "utils.h"
#include "WProgram.h"

#define SPI_CLK   13
#define SPI_MOSI  11

#define NB_LIG    _nb_lignes_tot

#define CLR_INV WHITE, BLACK
#define CLR_NRM BLACK, WHITE

#define LCDWIDTH       SHARPMEM_LCDHEIGHT
#define LCDHEIGHT      SHARPMEM_LCDWIDTH

#define BLACK 0
#define WHITE 1


typedef struct SAttitude { // definition d'un segment
  float lat;
  float lon;
  float alt;
  float speed;
  float cad_speed;
  float gpsalt;
  float climb;
  float dist;
  uint16_t next;
  int nbpts;
  float secj;
  uint8_t bpm;
  uint8_t cad_rpm;
  uint16_t pwr;
  uint8_t nbact;
  uint8_t nbpr;
  uint8_t nbkom;
  float temp;
  float pressu;
  float vbatt;
  uint8_t pbatt;
  float rrint;
  uint8_t ancs_type;
  String ancs_msg;
} SAttitude;

enum MODE_TLCD {
  MODE_SD,
  MODE_GPS,
  MODE_CRS,
  MODE_HRM,
  MODE_HT,
  MODE_MENU
};

class Segment;

class TLCD : public TSharpMem {
  public:
    TLCD(uint8_t ss);
    void cadran(uint8_t p_lig, uint8_t p_col, const char *champ, String  affi, const char *p_unite);
    void cadranH(uint8_t p_lig, const char *champ, String  affi, const char *p_unite);
    void updatePos(float lat_, float lon_, float alt_);
    void updateAll(SAttitude *att);
    void updateScreen(void);
    void afficheSegments(void);
    void afficheListePoints(uint8_t ligne, uint8_t ind_seg, uint8_t mode);
    void partner(float rtime, float curtime, uint8_t ind);
    void registerSegment(Segment *seg);
    uint8_t calculMode();
    void notifyANCS() { _ancs_mode = 5; return;}
    void decrANCS() { if (_ancs_mode > 0)_ancs_mode-=1; return;}
    void affiANCS();

  private:
    void traceLignes(void);
    void traceLignes_NS(void);
    void traceLignes_1S(void);
    void traceLignes_2S(void);

    SAttitude att;
    uint8_t _nb_lignes_tot;
    uint8_t _ss, _seg_act;
    uint8_t _mode, _mode_prec;
    uint8_t _ancs_mode;
    float _lat, _lon, _alt;
    Segment *_l_seg[2];
};

#endif

