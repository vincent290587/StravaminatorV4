#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C
#define CPU_RESTART_VAL 0x5FA0004
#define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL);

// any pins can be used
#define sd_sharp 23
#define speakerPin 18
// MOLETTE
#define buttonDown    22
#define buttonUp      14
#define buttonPress   16

#define TAILLE_LIGNE 250
#define TAILLE_MAX  50


//#define __DEBUG__


uint8_t BL_level = 180;

uint8_t led = 17;
uint8_t sd_cs = 22;// PTC1


static float sea_level_pressure = SENSORS_PRESSURE_SEALEVELHPA;
//static float temperature = 0;
//static float pressure = SENSORS_PRESSURE_SEALEVELHPA;
static float altOffset;
static unsigned long age;

static unsigned long start;
static unsigned long time_c = 0;

uint8_t new_gps_data = 0;
uint8_t new_hrm_data = 0;
uint8_t new_cad_data = 0;
uint8_t new_ancs_data = 0;
uint8_t new_dbg_data = 0;
uint8_t download_request = 0;
uint8_t upload_request = 0;
uint8_t mode_simu = 0;
uint8_t alertes_nb = 0;

// set up variables using the SD utility library functions:
SdFat sd;
SdFile file;
SdFile gpx;


SAttitude att;


ListeSegments mes_segments;

ListePoints mes_points;

tBatteryData batt_data;



