
#ifndef _TLCD_
#define _TLCD_

#include <TinyGPS.h>
#include "TSharpMem.h"
#include "Segment.h"
#include "utils.h"

#define SPI_CLK   13
#define SPI_MOSI  11

#define NB_LIG    _nb_lignes_tot

#define CLR_INV WHITE, BLACK
#define CLR_NRM BLACK, WHITE

#define LCDWIDTH       SHARPMEM_LCDHEIGHT
#define LCDHEIGHT      SHARPMEM_LCDWIDTH

#define BLACK 0
#define WHITE 1

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
  void updatePos(float lat_, float lon_, float alt_);
  void updateAll(void);
  void afficheSegments(void);
  void afficheListePoints(uint8_t ligne, uint8_t ind_seg, uint8_t mode);
  void partner(float rtime, float curtime, uint8_t ind);
  void registerSegment(Segment *seg);
  uint8_t calculMode();

 private:
  void traceLignes(void);
  void traceLignes_NS(void);
  void traceLignes_1S(void);
  void traceLignes_2S(void);

 
  uint8_t _nb_lignes_tot;
  uint8_t _ss, _seg_act;
  uint8_t _mode, _mode_prec;
  float _lat, _lon, _alt;
  Segment *_l_seg[2];
};

#endif

