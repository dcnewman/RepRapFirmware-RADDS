#ifndef PINS_DUET_H__
#define PINS_DUET_H__

#ifdef DECLARE_PIN_VARIABLES

// Default board type
#define DEFAULT_BOARD_TYPE BoardType::RADDS_15
#define ELECTRONICS "RADDS (+ Extension)"

#define SUPPORT_INKJET		0					// set nonzero to support inkjet control
#define SUPPORT_ROLAND		0					// set nonzero to support Roland mill

// The physical capabilities of the machine

// The number of drives in the machine, including X, Y, and Z
// plus extruder drives
const size_t DRIVES = 8;

// Initialization macro used in statements needing to initialize
// values in arrays of size DRIVES.  E.g.,
//
// max_fee_rates[DRIVES] = {DRIVES_(1, 1, 1, 1, 1, 1, 1, 1, 1)}
#define DRIVES_(a,b,c,d,e,f,g,h,i) { a,b,c,d,e,f,g,h }

// The number of movement axes in the machine, usually just X, Y and Z.
// <= DRIVE
const size_t AXES = 3;

// The number of heaters in the machine
// 0 is the heated bed even if there isn't one.
const int8_t HEATERS = 4;

// Initialization macro used in statements needing to initialize
// values in arrays of size HEATERS.  E.g.,
//
// defaultPidKis[HEATERS] = {HEATERS_(5.0, 0.1, 0.1, 0.1, 0.1, 0.1)};
#define HEATERS_(a,b,c,d,e,f,g) { a,b,c,d }

// The numbers of entries in each array must correspond with the
// values of DRIVES, AXES, or HEATERS
//
// Set values to -1 to flag unavailability.

const size_t NUM_SERIAL_CHANNELS = 2;

// Endstops
// E Stops not currently used
// Note: RepRapFirmware only as a single endstop per axis
//       gcode defines if it is a max ("high end") or min ("low end")
//       endstop.  gcode also sets if it is active HIGH or LOW
//
// 28 = RADDS X min
// 30 = RADDS Y min
// 32 = RADDS Z min
// 39 = RADDS PWM3
//
// This leaves 34, 36, and 38 as spare pins (X, Y, Z max)

const int8_t END_STOP_PINS[DRIVES] = { 28, 30, 32, 39, -1, -1, -1, -1 };

// DRIVES
//                                    X   Y   Z  E1  E2  E3  E4  E5
const Pin ENABLE_PINS[DRIVES] =    { 26, 22, 15, 62, 65, 49, 37, 31 };
const Pin STEP_PINS[DRIVES] =      { 24, 17,  2, 61, 64, 51, 35, 29 };
const Pin DIRECTION_PINS[DRIVES] = { 23, 16,  3, 60, 63, 53, 33, 27 };

// What each axis needs to make it go forwards - defaults
const bool DIRECTIONS[DRIVES] = {
   BACKWARDS, FORWARDS, FORWARDS, FORWARDS,
   FORWARDS,  FORWARDS, FORWARDS, FORWARDS };

// What to send to enable...
const bool ENABLE_DRIVE = false;

// ...and disable a drive
const bool DISABLE_DRIVE = !ENABLE_DRIVE;

// Logic level to assert to enable a drive
const bool ENABLE_VALUES[DRIVES] = {
		        ENABLE_DRIVE, ENABLE_DRIVE, ENABLE_DRIVE, ENABLE_DRIVE,
				ENABLE_DRIVE, ENABLE_DRIVE, ENABLE_DRIVE, ENABLE_DRIVE };

// HEATERS - The bed is assumed to be the at index 0

// Analogue pin numbers
const Pin TEMP_SENSE_PINS[HEATERS] = { 4, 0, 1, 2 };

// 0 for inverted heater (e.g. Duet v0.6)
// 1 for not (e.g. Duet v0.4; RADDS)
const bool HEAT_ON = true;

//pin D38 is PWM capable but not an Arduino PWM pin
const Pin HEAT_ON_PINS[HEATERS] = { 7, 13, 12, 11 }; // bed, h0, h1, h2

// Digital pin number that controls the ATX power on/off
const Pin ATX_POWER_PIN = 40;

// Z Probe pin
// Must be an ADC capable pin.  Can be any of the ARM's A/D capable
// pins even a non-Arduino pin.
const Pin Z_PROBE_PIN = 5;  // RADDS "ADC" pin

// Digital pin number to turn the IR LED on (high) or off (low)
// channel 1: Z_PROBE_MOD_PIN07
// channel 0: Z_PROBE_MOD_PIN
// Make channel 0 == channel 1 for RADDS.  Difference in channels
// is an artifact of difference in different versions of Duet electronics
//
// D34 -- unused X-max on RADDS
const Pin Z_PROBE_MOD_PIN = 34;
const Pin Z_PROBE_MOD_PIN07 = 34;

// Use a PWM capable pin
// Firmware uses SamNonDue so feel free to use a non-Arduino pin
const size_t NUM_FANS = 2;
const Pin COOLING_FAN_PINS[NUM_FANS] = { 9, 8 }; // Fan 0, Fan 1

// Firmware will attach a FALLING interrupt to this pin
// see FanInterrupt() in Platform.cpp
//
// D25 -- Unused GPIO on AUX1
const Pin COOLING_FAN_RPM_PIN = 25;

// Number of MAX31855 chips to support
const size_t MAX31855_START_CHANNEL = 100;
const int8_t MAX31855_DEVICES = 2;

// Digital pins the 31855s have their select lines tied to
const Pin MAX31855_CS_PINS[MAX31855_DEVICES] = { 38, 36 };

// Default thermistor betas
const float BED_R25 = 10000.0;
const float BED_BETA = 4066.0;
const float EXT_R25 = 100000.0;
const float EXT_BETA = 4066.0;

// Thermistor series resistor value in Ohms
const float THERMISTOR_SERIES_RS = 4700.0;

// Definition of which pins we allow to be controlled using M42
//
// Spare pins on the Arduino Due are
//
//  D5 / TIOA6  / C.25
//  D6 / PWML7  / C.24
// ### Removed: now E0_AXIS endstop D39 / PWMH2  / C.7
// D58 / AD3    / A.6
// D59 / AD2    / A.4
// D66 / DAC0   / B.15
// D67 / DAC1   / B.16
// D68 / CANRX0 / A.1
// D69 / CANTX0 / A.0
// D70 / SDA1   / A.17
// D71 / SCL1   / A.18
// D72 / RX LED / C.30
// D73 / TX LED / A.21

const size_t NUM_PINS_ALLOWED = 80;
#define PINS_ALLOWED {					\
	/* pins 00-07 */   	0b01100000,		\
	/* pins 08-15 */   	0,				\
	/* pins 16-23 */	0,				\
	/* pins 24-31 */	0,				\
	/* pins 32-39 */	0b00000000,		\
	/* pins 40-47 */	0,				\
	/* pins 48-55 */	0,				\
	/* pins 56-63 */	0b00001100,		\
	/* pins 64-71 */	0b11111100,		\
	/* pins 72-79 */	0b00000011		\
}


// SAM3X Flash locations (may be expanded in the future)
const uint32_t IAP_FLASH_START = 0x000F0000;
const uint32_t IAP_FLASH_END = 0x000FFFFF;

#endif // DECLARE_PIN_VARIABLES

// Use TX0/RX0 for the auxiliary serial line
#define SERIAL_MAIN_DEVICE SerialUSB
#define SERIAL_AUX_DEVICE Serial1

// Hardware SPI support for SD cards

// SD select
#define SD_SS            4
#define SD_DETECT_PIN   14 // card detect switch; needs pullup asserted
#define SD_DETECT_VAL    0 // detect switch active low
#define SD_DETECT_PIO_ID ID_PIOD

#ifdef LCD_UI

// Hardware I2C support for LCD
#define TWI_ID           ID_TWI1

#define FEATURE_CONTROLLER              7
#define UI_PAGES_DURATION            4000
#define UI_ANIMATION                    0
#define UI_SPEEDDEPENDENT_POSITIONING   0
#define UI_DISABLE_AUTO_PAGESWITCH      1
#define UI_AUTORETURN_TO_MENU_AFTER 30000
#define UI_ENCODER_SPEED                1
#define UI_KEY_BOUNCETIME              10
#define UI_KEY_FIRST_REPEAT           500
#define UI_KEY_REDUCE_REPEAT           50
#define UI_KEY_MIN_REPEAT              50
#define FEATURE_BEEPER                  1
#define UI_START_SCREEN_DELAY        1000

#define CASE_LIGHTS_PIN                -1
#define SPI_PIN                        77
#define SPI_CHAN                        0
#define UI_HAS_KEYS                     1
#define UI_HAS_BACK_KEY                 1
#define UI_DISPLAY_TYPE                 1
#define UI_DISPLAY_CHARSET              1
#define BEEPER_TYPE                     1
#define UI_COLS                        20
#define UI_ROWS                         4
#define BEEPER_PIN                     41
#define UI_DISPLAY_RS_PIN              42
#define UI_DISPLAY_RW_PIN              -1
#define UI_DISPLAY_ENABLE_PIN          43
#define UI_DISPLAY_D0_PIN              44
#define UI_DISPLAY_D1_PIN              45
#define UI_DISPLAY_D2_PIN              46
#define UI_DISPLAY_D3_PIN              47
#define UI_DISPLAY_D4_PIN              44
#define UI_DISPLAY_D5_PIN              45
#define UI_DISPLAY_D6_PIN              46
#define UI_DISPLAY_D7_PIN              47
#define UI_ENCODER_A                   52
#define UI_ENCODER_B                   50
#define UI_ENCODER_CLICK               48
#define UI_RESET_PIN                   -1
#define UI_DELAYPERCHAR                40
#define UI_INVERT_MENU_DIRECTION        0
#define UI_BUTTON_BACK                 71

// Beeper sound definitions for short beeps during key actions and longer
// beeps for important actions.  Parameters are the delay in microseconds
// followed by the number of repetitions.  Values must be in range 1..255
#define BEEPER_SHORT_SEQUENCE         2,2
#define BEEPER_LONG_SEQUENCE          8,8

#endif // LCD_UI

#endif
