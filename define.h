
// any pins can be used
#define sd_sharp 15
#define speakerPin 18
// MOLETTE
#define buttonDown    22
#define buttonUp      14
#define buttonPress   16

#define TAILLE_LIGNE 250
#define TAILLE_MAX  50

#define __DEBUG__

uint8_t led = 13;
uint8_t sd_cs = 20;// PTC1

static float sea_level_pressure = SENSORS_PRESSURE_SEALEVELHPA;
//static float temperature = 0;
//static float pressure = SENSORS_PRESSURE_SEALEVELHPA;
static float altOffset;
static unsigned long age;

//static float tensionValue = 4.15;  // variable to store the value coming from the 

static unsigned long start;
static unsigned long time_c = 0;

volatile uint8_t new_gps_data = 0;
volatile uint8_t new_hrm_data = 0;
volatile uint8_t new_cad_data = 0;
volatile uint8_t new_ancs_data = 0;

// set up variables using the SD utility library functions:
SdFat sd;
SdFile file;
SdFile gpx;


SAttitude att;


ListeSegments mes_segments;

ListePoints mes_points;


