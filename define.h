#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C
#define CPU_RESTART_VAL 0x5FA0004
#define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL);

// any pins can be used
#define sd_sharp 23
#define speakerPin 18

#define TAILLE_LIGNE 250
#define TAILLE_MAX  50

#define TABLE                 0x00000000
#define SECRET_CODE           0xDEADBEEF
#define SST_RECORDING_START   SPI_FLASH_64K_SIZE
#define NB_SECTORS_REC        30

//#define __DEBUG__
//#define __DEBUG_STC__

static uint8_t BL_level = 180;

const uint8_t virtbtn0 = 2; // PTD0
const uint8_t virtbtn1 = 14; // PTD1
const uint8_t led = 17;
const uint8_t sd_cs = 22;// PTC1

const uint8_t memCs = 15; //chip select      PTC0
const uint8_t memWp = 41; //write protection PTB23
const uint8_t memHold = 40; //hold           PTB22

static float sea_level_pressure = SENSORS_PRESSURE_SEALEVELHPA;
static float altOffset;
static unsigned long age;

static unsigned long start;
static unsigned long time_c = 0;
static unsigned long lastFix = 0;

static uint8_t new_gps_data = 0;
static uint8_t new_hrm_data = 0;
static uint8_t new_cad_data = 0;
static uint8_t new_ancs_data = 0;
static uint8_t new_btn_data = 0;
static uint8_t new_dbg_data = 0;
static uint8_t download_request = 0;
static uint8_t upload_request = 0;
static uint8_t mode_simu = 0;
static uint8_t alertes_nb = 0;

// structure SST26VF
typedef struct SInitTable {
  uint32_t secret_code;
  uint32_t last_data;
  uint32_t nb_data;
} SInitTable;

typedef union SWhiteBox {
  SInitTable sinit;
  uint8_t tab[12];
} SWhiteBox;

// set up variables using the SD utility library functions:
SdFat sd;
SdFile file;
SdFile gpx;


SAttitude att;

ListeSegments mes_segments;

ListePoints   mes_points;

ListeParcours mes_parcours;

tBatteryData  batt_data;



