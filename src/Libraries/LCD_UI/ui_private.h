/*
    This file is part of Repetier-Firmware.

    Repetier-Firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Repetier-Firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Repetier-Firmware.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef _ui_private_h
#define _ui_private_h

#define	READ(pin)														\
	PIO_Get(g_APinDescription[pin].pPort, PIO_INPUT,					\
			g_APinDescription[pin].ulPin)

#define	WRITE(pin, v)													\
	do {																\
		if (v) {														\
			g_APinDescription[pin].pPort->PIO_SODR = g_APinDescription[pin].ulPin; \
		}																\
		else {															\
			g_APinDescription[pin].pPort->PIO_CODR = g_APinDescription[pin].ulPin; \
		}																\
	} while(0)

#define SPEED_MIN_MILLIS 300
#define SPEED_MAX_MILLIS  50

#define PULLUP(IO,v)									\
	{ pinMode(IO, (v != LOW) ? INPUT_PULLUP : INPUT); }

#define	SET_INPUT(pin)													\
	pmc_enable_periph_clk(g_APinDescription[pin].ulPeripheralId);		\
    PIO_Configure(g_APinDescription[pin].pPort, PIO_INPUT,				\
				  g_APinDescription[pin].ulPin, 0) 

#define	SET_OUTPUT(pin)													\
	PIO_Configure(g_APinDescription[pin].pPort, PIO_OUTPUT_1,			\
    g_APinDescription[pin].ulPin, g_APinDescription[pin].ulPinConfiguration)

#define TOGGLE(pin) WRITE(pin, !(READ(pin)))

#define MIN(x,y) ((x) <= (y) ? (x) : (y))
#define MAX(x,y) ((x) >= (y) ? (x) : (y))

// ----------------------------------------------------------------------------
//                          Action codes
// 1-999     : Autorepeat
// 1000-1999 : Execute
// 2000-2999 : Write code
// 4000-4999 : Show menu
// Add UI_ACTION_TOPMENU to show a menu as top menu
// ----------------------------------------------------------------------------

#define UI_ACTION_TOPMENU 8192
#define UI_ACTION_DUMMY  10000

#define UI_ACTION_NEXT     1
#define UI_ACTION_PREVIOUS 2

#define UI_ACTION_X_UP                 100
#define UI_ACTION_X_DOWN               101
#define UI_ACTION_Y_UP                 102
#define UI_ACTION_Y_DOWN               103
#define UI_ACTION_Z_UP                 104
#define UI_ACTION_Z_DOWN               105
#define UI_ACTION_EXTRUDER_UP          106
#define UI_ACTION_EXTRUDER_DOWN        107
#define UI_ACTION_EXTRUDER_TEMP_UP     108
#define UI_ACTION_EXTRUDER_TEMP_DOWN   109
#define UI_ACTION_HEATED_BED_UP        110
#define UI_ACTION_HEATED_BED_DOWN      111
#define UI_ACTION_FAN_UP               112
#define UI_ACTION_FAN_DOWN             113

#define UI_ACTION_BACK                  1000
#define UI_ACTION_OK                    1001
#define UI_ACTION_MENU_UP               1002
#define UI_ACTION_TOP_MENU              1003
#define UI_ACTION_EMERGENCY_STOP        1004
#define UI_ACTION_XPOSITION             1005
#define UI_ACTION_YPOSITION             1006
#define UI_ACTION_ZPOSITION             1007
#define UI_ACTION_EPOSITION             1008
#define UI_ACTION_INSTANT_DV_X          1009
#define UI_ACTION_INSTANT_DV_Y          1010
#define UI_ACTION_INSTANT_DV_Z          1011
#define UI_ACTION_SD_DELETE             1012
#define UI_ACTION_SD_PRINT              1013
#define UI_ACTION_SD_PAUSE              1014
#define UI_ACTION_SD_CONTINUE           1015
#define UI_ACTION_HOME_ALL              1021
#define UI_ACTION_HOME_X                1022
#define UI_ACTION_HOME_Y                1023
#define UI_ACTION_HOME_Z                1024
#define UI_ACTION_SELECT_EXTRUDER1      1025
#define UI_ACTION_PID_MIN               1026
#define UI_ACTION_PREHEAT               1027
#define UI_ACTION_ACCEL_X               1032
#define UI_ACTION_ACCEL_Y               1033
#define UI_ACTION_ACCEL_Z               1034
#define UI_ACTION_ACCEL_E               1035
#define UI_ACTION_BAUDRATE              1040
#define UI_ACTION_MAX_FEEDRATE_X        1044
#define UI_ACTION_MAX_FEEDRATE_Y        1045
#define UI_ACTION_MAX_FEEDRATE_Z        1046
#define UI_ACTION_STEPS_X               1047
#define UI_ACTION_STEPS_Y               1048
#define UI_ACTION_STEPS_Z               1049
#define UI_ACTION_FAN_OFF               1050
#define UI_ACTION_FAN_25                1051
#define UI_ACTION_FAN_50                1052
#define UI_ACTION_FAN_75                1053
#define UI_ACTION_FAN_FULL              1054
#define UI_ACTION_FEEDRATE_MULTIPLY     1055
#define UI_ACTION_PID_PGAIN             1058
#define UI_ACTION_PID_IGAIN             1059
#define UI_ACTION_PID_DGAIN             1060
#define UI_ACTION_X_OFFSET              1063
#define UI_ACTION_Y_OFFSET              1064
#define UI_ACTION_STEPS_E               1065
#define UI_ACTION_MAX_FEEDRATE_E        1067
#define UI_ACTION_PID_MAX               1071
#define UI_ACTION_ADVANCE_K             1072
#define UI_ACTION_SET_ORIGIN            1073
#define UI_ACTION_POWER                 1078
#define UI_ACTION_COOLDOWN              1080
#define UI_ACTION_HEATED_BED_OFF        1081
#define UI_ACTION_EXTRUDER0_OFF         1082
#define UI_ACTION_EXTRUDER1_OFF         1083
#define UI_ACTION_HEATED_BED_TEMP       1084
#define UI_ACTION_EXTRUDER0_TEMP        1085
#define UI_ACTION_EXTRUDER1_TEMP        1086
#define UI_ACTION_DISABLE_STEPPER       1090
#define UI_ACTION_RESET_EXTRUDER        1091
#define UI_ACTION_EXTRUDER_RELATIVE     1092
#define UI_ACTION_SELECT_EXTRUDER0      1093
#define UI_ACTION_FLOWRATE_MULTIPLY     1096
#define UI_ACTION_KILL                  1097
#define UI_ACTION_RESET                 1098
#define UI_ACTION_PAUSE                 1099
#define UI_ACTION_EXTRUDER2_OFF         1102
#define UI_ACTION_EXTRUDER2_TEMP        1103
#define UI_ACTION_SELECT_EXTRUDER2      1104
#define UI_ACTION_FANSPEED              1106
#define UI_ACTION_SD_STOP               1108

#define UI_ACTION_MENU_XPOS             4000
#define UI_ACTION_MENU_YPOS             4001
#define UI_ACTION_MENU_ZPOS             4002
#define UI_ACTION_MENU_XPOSFAST         4003
#define UI_ACTION_MENU_YPOSFAST         4004
#define UI_ACTION_MENU_ZPOSFAST         4005
#define UI_ACTION_MENU_SDCARD           4006
#define UI_ACTION_MENU_QUICKSETTINGS    4007
#define UI_ACTION_MENU_EXTRUDER         4008
#define UI_ACTION_MENU_POSITIONS        4009

// Load basic language definition to make sure all values are defined
#include "ui_lang.h"

typedef struct {
  const char *text; // Menu text
  uint8_t menuType; // 0 = Info, 1 = Headline, 2 = submenu ref, 3 = direct action command, 4 = modify action command
  unsigned int action; // must be int so it gets 32 bit on arm!
  uint8_t filter; // allows dynamic menu filtering based on Printer::menuMode bits set.
  uint8_t nofilter; // Hide if one of these bits are set
  bool showEntry() const;
} const UIMenuEntry;

typedef struct {
	// 0 = info page
	// 1 = file selector
	// 2 = submenu
	// 3 = modififaction menu
	unsigned char menuType;
	int           id;          // Type of modification
	int           numEntries;
	const UIMenuEntry * const * entries;
} const UIMenu;

#if UI_ENCODER_SPEED == 0

// Full speed
static const int8_t encoder_table[16] = {
	0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0};

#elif UI_ENCODER_SPEED == 1

// Half speed
static const int8_t encoder_table[16] = {
	0, 0, -1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, -1, 0, 0};

#else

// Quarter speed
static const int8_t encoder_table[16] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0};

#endif

// Key codes
#define UI_KEYS_INIT_CLICKENCODER_LOW(pinA, pinB)			\
	SET_INPUT(pinA);										\
	SET_INPUT(pinB);										\
	PULLUP(pinA, HIGH);										\
	PULLUP(pinB, HIGH)

#define UI_KEYS_INIT_BUTTON_LOW(pin) \
	SET_INPUT(pin);					 \
	PULLUP(pin, HIGH)

#define UI_KEYS_INIT_CLICKENCODER_HIGH(pinA, pinB)	\
	SET_INPUT(pinA);								\
	SET_INPUT(pinB);								\
	PULLUP(pinA, LOW);								\
	PULLUP(pinB, LOW)

#define UI_KEYS_INIT_BUTTON_HIGH(pin)			\
	SET_INPUT(pin);								\
	PULLUP(pin,LOW)

#define UI_KEYS_CLICKENCODER_LOW(pinA, pinB)							\
	encoderLast = (encoderLast << 2) & 0x0F;					\
	if (!READ(pinA)) encoderLast |= 2;								\
	if (!READ(pinB)) encoderLast |= 1;								\
	encoderPos += encoder_table[encoderLast]

#define UI_KEYS_CLICKENCODER_LOW_REV(pinA, pinB)						\
	encoderLast = (encoderLast << 2) & 0x0F;					\
	if (!READ(pinA)) encoderLast |= 2;								\
	if (!READ(pinB)) encoderLast |= 1;								\
	encoderPos -= encoder_table[encoderLast]

#define UI_KEYS_BUTTON_LOW(pin, action_)		\
	if ((READ(pin)) == 0) action = action_

#define UI_KEYS_CLICKENCODER_HIGH(pinA, pinB)							\
	encoderLast = (encoderLast << 2) & 0x0F;					\
	if (READ(pinA)) encoderLast |= 2;								\
	if (READ(pinB)) encoderLast |= 1;								\
	encoderPos += encoder_table[encoderLast]

#define UI_KEYS_CLICKENCODER_HIGH_REV(pinA, pinB)						\
	encoderLast = (encoderLast << 2) & 0x0F;					\
	if (READ(pinA)) encoderLast |= 2;								\
	if (READ(pinB)) encoderLast |= 1;								\
	encoderPos -= encoder_table[encoderLast]

#define UI_KEYS_BUTTON_HIGH(pin, action_)		\
	if ((READ(pin)) != 0) action=action_

#define UI_KEYS_INIT_MATRIX(r1, r2, r3, r4, c1, c2, c3, c4)	\
	if ((c1) >= 0) {										\
		SET_INPUT(c1);										\
		WRITE(c1, HIGH);									\
	}														\
	if ((c2) >= 0) {										\
		SET_INPUT(c2);										\
		WRITE(c2, HIGH);									\
	}														\
	if ((c3) >= 0) {										\
		SET_INPUT(c3);										\
		WRITE(c3,HIGH);										\
	}														\
    if ((c4) >= 0) {										\
		SET_INPUT(c4);										\
		WRITE(c4, HIGH);									\
	}														\
	if ((r1) >= 0) SET_OUTPUT(r1);							\
	if ((r2) >= 0) SET_OUTPUT(r2);							\
	if ((r3) >= 0)SET_OUTPUT(r3);							\
	if ((r4) >= 0) SET_OUTPUT(r4);							\
    if ((r1) >= 0) WRITE(r1, LOW);							\
	if ((r2) >= 0) WRITE(r2, LOW);							\
	if ((r3) >= 0) WRITE(r3, LOW);							\
	if ((r4) >= 0) WRITE(r4, LOW)

#define UI_KEYS_MATRIX(r1, r2, r3, r4, c1, c2, c3, c4)					\
	{																	\
		uint8_t r = (((c1) >= 0) ? READ(c1) : 0) &&						\
			(((c2) >= 0) ? READ(c2) : 0) &&								\
			(((c3) >= 0) ? READ(c3) : 0) &&								\
			(((c4) >= 0) ? READ(c4) : 0);								\
		if (!r) {														\
			r = 255;											    	\
			if ((r2) >= 0) WRITE(r2, HIGH);								\
			if ((r3) >= 0) WRITE(r3, HIGH);								\
			if ((r4) >= 0) WRITE(r4, HIGH);								\
			if ((r1) >= 0) {											\
				asm volatile ("nop\nnop\nnop\nnop\nnop");				\
				if (!(((c1) >= 0 ? READ(c1) : 1) &&						\
					  ((c2) >= 0 ? READ(c2) : 1) &&						\
					  ((c3) >= 0 ? READ(c3) : 1) &&						\
					  ((c4) >= 0 ? READ(c4) : 1))) r = 0;				\
				else WRITE(r1, HIGH);									\
			}															\
			if ((r == 255) && ((r2) >= 0)) {							\
				WRITE(r2, LOW);											\
				asm volatile ("nop\nnop\nnop\nnop\nnop");				\
				if (!(((c1) >= 0 ? READ(c1) : 1) &&						\
					  ((c2) >= 0 ? READ(c2) : 1) &&						\
					  ((c3) >= 0 ? READ(c3) : 1) &&						\
					  ((c4) >= 0 ? READ(c4) : 1))) r = 4;				\
				else WRITE(r2, HIGH);									\
			}															\
			if ((r == 255) && ((r3) >= 0)) {								\
				WRITE(r3, LOW);											\
				asm volatile ("nop\nnop\nnop\nnop\nnop");				\
				if (!(((c1) >= 0 ? READ(c1) : 0) &&						\
					  ((c2) >= 0 ? READ(c2) : 1) &&						\
					  ((c3) >= 0 ? READ(c3) : 1) &&						\
					  ((c4) >= 0 ? READ(c4) : 1))) r = 8;				\
				else WRITE(r3, HIGH);									\
			}															\
			if ((r == 255) && ((r4) >= 0)) {							\
				WRITE(r4, LOW);											\
				asm volatile ("nop\nnop\nnop\nnop\nnop");				\
				if (!(((c1) >= 0 ? READ(c1) : 1) &&						\
					  ((c2) >= 0 ? READ(c2) : 1) &&						\
					  ((c3) >= 0 ? READ(c3) : 1) &&						\
					  ((c4) >= 0 ? READ(c4) : 1))) r = 12;				\
				else WRITE(r4, HIGH);									\
			}															\
			if (((c2) >= 0) && !(READ(c2))) r += 1;						\
			else if (((c3) >= 0) && !(READ(c3))) r += 2;				\
			else if (((c4) >= 0) && !(READ(c4))) r += 3;				\
			if (r < 16) action = matrixActions[r];						\
		}																\
		if ((r1) >= 0) WRITE(r1, LOW);									\
		if ((r2) >= 0) WRITE(r2, LOW);									\
		if ((r3) >= 0) WRITE(r3, LOW);									\
		if ((r4) >= 0) WRITE(r4, LOW);									\
	}

// I2C keymask tests
#define UI_KEYS_I2C_CLICKENCODER_LOW(pinA, pinB)		\
	encoderLast = (encoderLast << 2) & 0x0F;	\
	if (!(keymask & (pinA))) encoderLast |= 2;		\
	if (!(keymask & (pinB))) encoderLast |= 1;		\
	encoderPos += encoder_table[encoderLast]

#define UI_KEYS_I2C_CLICKENCODER_LOW_REV(pinA, pinB)	\
	encoderLast = (encoderLast << 2) & 0x0F;	\
	if (!(keymask & (pinA))) encoderLast |= 2;		\
	if (!(keymask & (pinB))) encoderLast |= 1;		\
	encoderPos -= encoder_table[encoderLast]

#define UI_KEYS_I2C_BUTTON_LOW(pin, action_)		\
	if ((keymask & (pin)) == 0) action = action_

#define UI_KEYS_I2C_CLICKENCODER_HIGH(pinA, pinB)\
	encoderLast = (encoderLast << 2) & 0x0F;	\
	if (keymask & (pinA)) encoderLast |= 2;			\
	if (keymask & (pinB)) encoderLast |= 1;			\
	encoderPos += encoder_table[encoderLast]

#define UI_KEYS_I2C_CLICKENCODER_HIGH_REV(pinA, pinB)	\
	encoderLast = (encoderLast << 2) & 0x0F;	\
	if (keymask & (pinA)) encoderLast |= 2;			\
	if (keymask & (pinB)) encoderLast |= 1;			\
	encoderPos -= encoder_table[encoderLast]

#define UI_KEYS_I2C_BUTTON_HIGH(pin, action_) \
	if (((pin) & keymask) != 0) action = action_

#define UI_STRING(name, text) const char name[] = text

#define UI_PAGE6(name, row1, row2, row3, row4, row5, row6)			\
	UI_STRING(name ## _1txt, row1);									\
	UI_STRING(name ## _2txt, row2);									\
	UI_STRING(name ## _3txt, row3);									\
	UI_STRING(name ## _4txt, row4);									\
	UI_STRING(name ## _5txt, row5);									\
	UI_STRING(name ## _6txt, row6);									\
	UIMenuEntry name ## _1 = {name ## _1txt, 0, 0, 0, 0};			\
	UIMenuEntry name ## _2 = {name ## _2txt, 0, 0, 0, 0};			\
	UIMenuEntry name ## _3 = {name ## _3txt, 0, 0, 0, 0};			\
	UIMenuEntry name ## _4 = {name ## _4txt, 0, 0, 0, 0};			\
	UIMenuEntry name ## _5 = {name ## _5txt, 0, 0, 0, 0};			\
	UIMenuEntry name ## _6 = {name ## _6txt, 0, 0, 0, 0};			\
	const UIMenuEntry * const name ## _entries [] = {&name ## _1,	\
													&name ## _2,	\
													&name ## _3,	\
													&name ## _4,	\
													&name ## _5,	\
													&name ## _6};	\
	const UIMenu name = {0, 0, 6, name ## _entries}

#define UI_PAGE4(name, row1, row2, row3, row4)							\
	UI_STRING(name ## _1txt, row1);										\
	UI_STRING(name ## _2txt, row2);										\
	UI_STRING(name ## _3txt, row3);										\
	UI_STRING(name ## _4txt, row4);										\
	UIMenuEntry name ## _1 = {name ## _1txt, 0, 0, 0, 0};				\
	UIMenuEntry name ## _2 = {name ## _2txt, 0, 0, 0, 0};				\
	UIMenuEntry name ## _3 = {name ## _3txt, 0, 0, 0, 0};				\
	UIMenuEntry name ## _4 = {name ## _4txt, 0, 0, 0, 0};				\
	const UIMenuEntry * const name ## _entries [] = {&name ## _1, &name ## _2, &name ## _3, &name ## _4}; \
	const UIMenu name = {0, 0, 4, name ## _entries}

#define UI_PAGE2(name, row1, row2)										\
	UI_STRING(name ## _1txt, row1);										\
	UI_STRING(name ## _2txt, row2);										\
	UIMenuEntry name ## _1 = {name ## _1txt, 0, 0, 0, 0};				\
	UIMenuEntry name ## _2 = {name ## _2txt, 0, 0, 0, 0};				\
	const UIMenuEntry * const name ## _entries[] = {&name ## _1, &name ## _2}; \
	const UIMenu name = {0, 0, 2, name ## _entries}

#define UI_MENU_ACTION4C(name, action, rows) UI_MENU_ACTION4(name, action, rows)

#define UI_MENU_ACTION2C(name, action, rows) UI_MENU_ACTION2(name, action, rows)

#define UI_MENU_ACTION4(name, action, row1, row2, row3, row4)			\
	UI_STRING(name ## _1txt, row1);										\
	UI_STRING(name ## _2txt, row2);										\
	UI_STRING(name ## _3txt, row3);										\
	UI_STRING(name ## _4txt, row4);										\
	UIMenuEntry name ## _1 = {name ## _1txt, 0, 0, 0, 0};				\
	UIMenuEntry name ## _2 = {name ## _2txt, 0, 0, 0, 0};				\
	UIMenuEntry name ## _3 = {name ## _3txt, 0, 0, 0, 0};				\
	UIMenuEntry name ## _4 = {name ## _4txt, 0, 0, 0, 0};				\
	const UIMenuEntry * const name ## _entries[] = {&name ## _1, &name ## _2, &name ## _3, &name ## _4}; \
	const UIMenu name = {3, action, 4, name ## _entries}

#define UI_MENU_ACTION2(name, action, row1, row2)						\
	UI_STRING(name ## _1txt, row1);										\
	UI_STRING(name ## _2txt, row2);										\
	UIMenuEntry name ## _1 = {name ## _1txt, 0, 0, 0, 0};				\
	UIMenuEntry name ## _2 = {name ## _2txt, 0, 0, 0, 0};				\
	const UIMenuEntry * const name ## _entries[] = {&name ## _1, &name ## _2}; \
	const UIMenu name = {3, action, 2, name ## _entries}

#define UI_MENU_HEADLINE(name, text)				\
	UI_STRING(name ## _txt, text);					\
	UIMenuEntry name = {name ## _txt, 1, 0, 0, 0}

#define UI_MENU_CHANGEACTION(name, row, action)			\
	UI_STRING(name ## _txt, row);						\
	UIMenuEntry name = {name ## _txt, 4, action, 0,0 }

#define UI_MENU_ACTIONCOMMAND(name, row, action)		\
	UI_STRING(name ## _txt, row);						\
	UIMenuEntry name = {name ## _txt, 3, action, 0, 0}

#define UI_MENU_ACTIONSELECTOR(name, row, entries)						\
	UI_STRING(name ## _txt, row);										\
	UIMenuEntry name = {name ## _txt, 2, (unsigned int)&entries, 0, 0}

#define UI_MENU_SUBMENU(name, row, entries)								\
	UI_STRING(name ## _txt, row);										\
	UIMenuEntry name = {name ## _txt, 2, (unsigned int)&entries, 0, 0}

#define UI_MENU_CHANGEACTION_FILTER(name, row, action, filter, nofilter) \
	UI_STRING(name ## _txt, row);										\
	UIMenuEntry name = {name ## _txt, 4, action, filter, nofilter}

#define UI_MENU_ACTIONCOMMAND_FILTER(name, row, action, filter, nofilter) \
	UI_STRING(name ## _txt, row);										\
	UIMenuEntry name = {name ## _txt, 3, action, filter, nofilter}

#define UI_MENU_ACTIONSELECTOR_FILTER(name, row, entries, filter, nofilter)	\
	UI_STRING(name ## _txt, row);										\
	UIMenuEntry name = {name ## _txt, 2, (unsigned int)&entries, filter, nofilter}

#define UI_MENU_SUBMENU_FILTER(name, row, entries, filter, nofilter)	\
	UI_STRING(name ## _txt, row);										\
	UIMenuEntry name = {name ## _txt, 2, (unsigned int)&entries, filter, nofilter}

#define UI_MENU(name, items, itemsCnt)						\
	const UIMenuEntry * const name ## _entries[] = items;	\
	const UIMenu name = {2, 0, itemsCnt, name ## _entries}

#define UI_MENU_FILESELECT(name, items, itemsCnt)			\
	const UIMenuEntry * const name ## _entries[] = items;	\
	const UIMenu name = {1, 0, itemsCnt, name ## _entries}

#if FEATURE_CONTROLLER == 2 || FEATURE_CONTROLLER == 10 || FEATURE_CONTROLLER == 11 // reprapdiscount smartcontroller has a sd card buildin

#ifdef SDCARDDETECT
#undef SDCARDDETECT
#endif
#define SDCARDDETECT 49

#ifdef SDCARDDETECTINVERTED
#undef SDCARDDETECTINVERTED
#endif
#define SDCARDDETECTINVERTED false

#ifdef SDSUPPORT
#undef SDSUPPORT
#endif
#define SDSUPPORT true

#endif

extern UIDisplay uid;

#if FEATURE_CONTROLLER == 1
#include "ui_fc1.h"
#endif

#if FEATURE_CONTROLLER == 0 // No controller at all

#define UI_HAS_KEYS 0
#define UI_DISPLAY_TYPE 0

void UIDisplay::initKeys() {}
void UIDisplay::checkKeys(int &action) {}
void UIDisplay::checkSlowEncoder() {}
void UIDisplay::checkSlowKeys(int &action) {}

#endif // FEATURE_CONTROLLER == 0

#if FEATURE_CONTROLLER == 3 // Adafruit RGB controller

#define UI_HAS_KEYS 1
#define UI_HAS_BACK_KEY 1
#define UI_DISPLAY_TYPE 3
#define UI_DISPLAY_CHARSET 1
#define UI_COLS 16
#define UI_ROWS 2
#define UI_DISPLAY_I2C_CHIPTYPE 1
#define UI_DISPLAY_I2C_ADDRESS 0x40
#define UI_DISPLAY_I2C_OUTPUT_PINS 65504
#define UI_DISPLAY_I2C_OUTPUT_START_MASK 0
#define UI_DISPLAY_I2C_PULLUP 31
#define UI_I2C_CLOCKSPEED 400000L
#define UI_DISPLAY_RS_PIN _BV(15)
#define UI_DISPLAY_RW_PIN _BV(14)
#define UI_DISPLAY_ENABLE_PIN _BV(13)
#define UI_DISPLAY_D0_PIN _BV(12)
#define UI_DISPLAY_D1_PIN _BV(11)
#define UI_DISPLAY_D2_PIN _BV(10)
#define UI_DISPLAY_D3_PIN _BV(9)
#define UI_DISPLAY_D4_PIN _BV(12)
#define UI_DISPLAY_D5_PIN _BV(11)
#define UI_DISPLAY_D6_PIN _BV(10)
#define UI_DISPLAY_D7_PIN _BV(9)
#define UI_INVERT_MENU_DIRECTION true
#define UI_HAS_I2C_KEYS
#define UI_HAS_I2C_ENCODER 0
#define UI_I2C_KEY_ADDRESS 0x40

void UIDisplay::initKeys() {}
void UIDisplay::checkKeys(int &action) {}

void UIDisplay::checkSlowEncoder()
{
	i2cStartWait(UI_DISPLAY_I2C_ADDRESS + I2C_WRITE);
	i2cWrite(0x12); // GIOA
	i2cStop();
	i2cStartWait(UI_DISPLAY_I2C_ADDRESS + I2C_READ);
	unsigned int keymask = i2cReadAck();
	keymask = keymask + (i2cReadNak() << 8);
	i2cStop();
}

void UIDisplay::checkSlowKeys(int &action)
{
	i2cStartWait(UI_DISPLAY_I2C_ADDRESS + I2C_WRITE);
	i2cWrite(0x12); // GPIOA
	i2cStop();
	i2cStartWait(UI_DISPLAY_I2C_ADDRESS + I2C_READ);
	unsigned int keymask = i2cReadAck();
	keymask = keymask + (i2cReadNak() << 8);
	i2cStop();
	UI_KEYS_I2C_BUTTON_LOW(4, UI_ACTION_PREVIOUS); // Up button
	UI_KEYS_I2C_BUTTON_LOW(8, UI_ACTION_NEXT); // down button
	UI_KEYS_I2C_BUTTON_LOW(16, UI_ACTION_BACK); // left button
	UI_KEYS_I2C_BUTTON_LOW(2, UI_ACTION_OK); // right button
	UI_KEYS_I2C_BUTTON_LOW(1, UI_ACTION_MENU_QUICKSETTINGS);  //Select button
}

#endif // FEATURE_CONTROLLER == 3, Adafruit RGB controller

#if FEATURE_CONTROLLER == 4 // Foltyn 3D Master

#define UI_HAS_KEYS 1
#define UI_HAS_BACK_KEY 1
#define UI_DISPLAY_TYPE 1
#define UI_DISPLAY_CHARSET 2
#define UI_COLS 20
#define UI_ROWS 4
#define UI_DISPLAY_RS_PIN		63		// PINK.1, 88, D_RS
#define UI_DISPLAY_RW_PIN		-1
#define UI_DISPLAY_ENABLE_PIN	        65		// PINK.3, 86, D_E
#define UI_DISPLAY_D0_PIN		59		// PINF.5, 92, D_D4
#define UI_DISPLAY_D1_PIN		64		// PINK.2, 87, D_D5
#define UI_DISPLAY_D2_PIN		44		// PINL.5, 40, D_D6
#define UI_DISPLAY_D3_PIN		66		// PINK.4, 85, D_D7
#define UI_DISPLAY_D4_PIN		59		// PINF.5, 92, D_D4
#define UI_DISPLAY_D5_PIN		64		// PINK.2, 87, D_D5
#define UI_DISPLAY_D6_PIN		44		// PINL.5, 40, D_D6
#define UI_DISPLAY_D7_PIN		66		// PINK.4, 85, D_D7
#define UI_DELAYPERCHAR		   320
#define UI_INVERT_MENU_DIRECTION false

void UIDisplay::initKeys()
{
	UI_KEYS_INIT_BUTTON_LOW(4); // push button, connects gnd to pin
	UI_KEYS_INIT_BUTTON_LOW(5);
	UI_KEYS_INIT_BUTTON_LOW(6);
	UI_KEYS_INIT_BUTTON_LOW(11);
	UI_KEYS_INIT_BUTTON_LOW(42);
}

void UIDisplay::checkKeys(int &action)
{
	UI_KEYS_BUTTON_LOW(4, UI_ACTION_OK); // push button, connects gnd to pin
	UI_KEYS_BUTTON_LOW(5, UI_ACTION_NEXT); // push button, connects gnd to pin
	UI_KEYS_BUTTON_LOW(6, UI_ACTION_PREVIOUS); // push button, connects gnd to pin
	UI_KEYS_BUTTON_LOW(11, UI_ACTION_BACK); // push button, connects gnd to pin
	UI_KEYS_BUTTON_LOW(42, UI_ACTION_SD_PRINT ); // push button, connects gnd to pin
}

void UIDisplay::checkSlowEncoder() {}
void UIDisplay::checkSlowKeys(int &action) {}

#endif // FEATURE_CONTROLLER == 4, Foltyn 3D Master


#if FEATURE_CONTROLLER == 5 // Viki Lcd

// You need to change these 3 button according to the positions
// where you put them into your board!
#define UI_ENCODER_A      7 // pins the click encoder are connected to
#define UI_ENCODER_B      22
#define UI_RESET_PIN      32 // single button for reset
#define SDCARDDETECT      49 // Set to -1 if you have not connected that pin
#define SDSS              53 // Chip select pin

#define SDSUPPORT true
#define SDCARDDETECTINVERTED false

#define UI_HAS_KEYS 1
#define UI_HAS_BACK_KEY 1
#define UI_DISPLAY_TYPE 3
#define UI_DISPLAY_CHARSET 1
#define UI_COLS 20
#define UI_ROWS 4
#define UI_DISPLAY_I2C_CHIPTYPE 1
#define UI_DISPLAY_I2C_ADDRESS 0x40
#define UI_DISPLAY_I2C_OUTPUT_PINS 0xFFE0
#define UI_DISPLAY_I2C_OUTPUT_START_MASK 0x01C0 // bits that are high always, for now the 3 viki leds
#define UI_DISPLAY_I2C_PULLUP 0x001F
#define UI_I2C_CLOCKSPEED 100000L // Note with very long cables make this much smaller, for 2ft cables I found 80000 worked ok

#define UI_DISPLAY_RS_PIN _BV(15)
#define UI_DISPLAY_RW_PIN _BV(14)
#define UI_DISPLAY_ENABLE_PIN _BV(13)
#define UI_DISPLAY_D0_PIN _BV(12)
#define UI_DISPLAY_D1_PIN _BV(11)
#define UI_DISPLAY_D2_PIN _BV(10)
#define UI_DISPLAY_D3_PIN _BV(9)
#define UI_DISPLAY_D4_PIN _BV(12)
#define UI_DISPLAY_D5_PIN _BV(11)
#define UI_DISPLAY_D6_PIN _BV(10)
#define UI_DISPLAY_D7_PIN _BV(9)

#if true || !defined(BEEPER_PIN) || BEEPER_PIN < 0

#define BEEPER_PIN        _BV(5)
#define BEEPER_TYPE       2
#define BEEPER_ADDRESS    UI_DISPLAY_I2C_ADDRESS // I2C address of the chip with the beeper pin

#endif // true

#define UI_I2C_HEATBED_LED    _BV(8)
#define UI_I2C_HOTEND_LED     _BV(7)
#define UI_I2C_FAN_LED        _BV(6)

#define UI_INVERT_MENU_DIRECTION false
#define UI_HAS_I2C_KEYS
#define UI_HAS_I2C_ENCODER 0
#define UI_I2C_KEY_ADDRESS 0x40

void UIDisplay::initKeys()
{
	UI_KEYS_INIT_CLICKENCODER_LOW(UI_ENCODER_A, UI_ENCODER_B); // click encoder on real pins. Phase is connected with gnd for signals.
	UI_KEYS_INIT_BUTTON_LOW(UI_RESET_PIN); // Kill pin
}

void UIDisplay::checkKeys(int &action)
{
	UI_KEYS_CLICKENCODER_LOW_REV(UI_ENCODER_A, UI_ENCODER_B); // click encoder on real pins
	UI_KEYS_BUTTON_LOW(UI_RESET_PIN, UI_ACTION_RESET);
}

void UIDisplay::checkSlowEncoder() { }// not used in Viki

void UIDisplay::checkSlowKeys(int &action)
{
	i2cStartWait(UI_DISPLAY_I2C_ADDRESS + I2C_WRITE);
	i2cWrite(0x12); // GPIOA
	i2cStop();
	i2cStartWait(UI_DISPLAY_I2C_ADDRESS + I2C_READ);
	unsigned int keymask = i2cReadAck();
	keymask = keymask + (i2cReadNak() << 8);
	i2cStop();
	UI_KEYS_I2C_BUTTON_LOW(4, UI_ACTION_MENU_SDCARD);        // Up button
	UI_KEYS_I2C_BUTTON_LOW(8, UI_ACTION_MENU_QUICKSETTINGS); // down button
	UI_KEYS_I2C_BUTTON_LOW(16, UI_ACTION_BACK);              // left button
	UI_KEYS_I2C_BUTTON_LOW(2, UI_ACTION_MENU_POSITIONS);     // right button
	UI_KEYS_I2C_BUTTON_LOW(1, UI_ACTION_OK);                 //Select button
}

#endif // FEATURE_CONTROLLER == 5, Viki Lcd

#if FEATURE_CONTROLLER == 7 // RADDS

void UIDisplay::initKeys()
{
	// click encoder on pins 47 and 45. Phase is connected with gnd for signals
	UI_KEYS_INIT_CLICKENCODER_LOW(UI_ENCODER_A, UI_ENCODER_B);

	// push button, connects gnd to pin
	UI_KEYS_INIT_BUTTON_LOW(UI_ENCODER_CLICK);
	UI_KEYS_INIT_BUTTON_LOW(UI_BUTTON_BACK);
}

void UIDisplay::checkKeys(int &action)
{
	// click encoder on pins 47 and 45
	// Phase is connected with gnd for signals
	UI_KEYS_CLICKENCODER_LOW_REV(UI_ENCODER_A, UI_ENCODER_B);

	// Push button, connects gnd to pin
	UI_KEYS_BUTTON_LOW(UI_ENCODER_CLICK, UI_ACTION_OK);
	UI_KEYS_BUTTON_LOW(UI_BUTTON_BACK, UI_ACTION_BACK);
}

void UIDisplay::checkSlowEncoder() {}
void UIDisplay::checkSlowKeys(int &action) {}

#endif // FEATURE_CONTROLLER == 7, RADDS pin assignment for displays

#if FEATURE_CONTROLLER == 8 || FEATURE_CONTROLLER == 9 // PiBot Expansion Port

#define UI_HAS_KEYS 1
#define UI_HAS_BACK_KEY 1
#define UI_DISPLAY_TYPE 1
#define UI_DISPLAY_CHARSET 1
#define UI_DELAYPERCHAR 320
#define UI_INVERT_MENU_DIRECTION true
#define BEEPER_SHORT_SEQUENCE 6,2 // Needs longer beep sequence
#define BEEPER_LONG_SEQUENCE 24,8
#define BEEPER_TYPE 1
#define BEEPER_TYPE_INVERTING false

#if FEATURE_CONTROLLER == 9   // 16x02 Display

#define UI_COLS 16
#define UI_ROWS 2

#else  ////20x04 Display

#define UI_COLS 20
#define UI_ROWS 4

#endif // FEATURE_CONTROLLER == 9

#ifdef PiBot_V_1_4

#define BEEPER_PIN             31
#define UI_DISPLAY_RS_PIN      45
#define UI_DISPLAY_RW_PIN      -1
#define UI_DISPLAY_ENABLE_PIN  44
#define UI_DISPLAY_D0_PIN      43
#define UI_DISPLAY_D1_PIN      42
#define UI_DISPLAY_D2_PIN      19
#define UI_DISPLAY_D3_PIN      18
#define UI_DISPLAY_D4_PIN      43
#define UI_DISPLAY_D5_PIN      42
#define UI_DISPLAY_D6_PIN      19
#define UI_DISPLAY_D7_PIN      18
#define UI_ENCODER_A           61
#define UI_ENCODER_B           62
#define UI_ENCODER_CLICK       63
#define UI_RESET_PIN           28
#define UI_DELAYPERCHAR 320
#define UI_BUTTON_OK       49
#define UI_BUTTON_NEXT     48
#define UI_BUTTON_PREVIOUS 47
#define UI_BUTTON_BACK     46
#define UI_BUTTON_SD_PRINT 29

#else // !PiBot_V_1_4

#define BEEPER_PIN             37
#define UI_DISPLAY_RS_PIN      16
#define UI_DISPLAY_RW_PIN      -1
#define UI_DISPLAY_ENABLE_PIN  17
#define UI_DISPLAY_D0_PIN      23
#define UI_DISPLAY_D1_PIN      25
#define UI_DISPLAY_D2_PIN      27
#define UI_DISPLAY_D3_PIN      29
#define UI_DISPLAY_D4_PIN      23
#define UI_DISPLAY_D5_PIN      25
#define UI_DISPLAY_D6_PIN      27
#define UI_DISPLAY_D7_PIN      29
#define UI_ENCODER_A           33
#define UI_ENCODER_B           31
#define UI_ENCODER_CLICK       35
#define UI_RESET_PIN           41
#define UI_DELAYPERCHAR 320
#define UI_BUTTON_OK       4
#define UI_BUTTON_NEXT     6
#define UI_BUTTON_PREVIOUS 5
#define UI_BUTTON_BACK     11
#define UI_BUTTON_SD_PRINT 42

#endif // PiBot_V_1_4

void UIDisplay::initKeys()
{
	UI_KEYS_INIT_BUTTON_LOW(UI_BUTTON_OK); // push button, connects gnd to pin
	UI_KEYS_INIT_BUTTON_LOW(UI_BUTTON_NEXT);
	UI_KEYS_INIT_BUTTON_LOW(UI_BUTTON_PREVIOUS);
	UI_KEYS_INIT_BUTTON_LOW(UI_BUTTON_BACK);
	UI_KEYS_INIT_BUTTON_LOW(UI_BUTTON_SD_PRINT);
}

void UIDisplay::checkKeys(int &action)
{
	UI_KEYS_BUTTON_LOW(UI_BUTTON_OK, UI_ACTION_OK); // push button, connects gnd to pin
	UI_KEYS_BUTTON_LOW(UI_BUTTON_NEXT, UI_ACTION_NEXT); // push button, connects gnd to pin
	UI_KEYS_BUTTON_LOW(UI_BUTTON_PREVIOUS, UI_ACTION_PREVIOUS); // push button, connects gnd to pin
	UI_KEYS_BUTTON_LOW(UI_BUTTON_BACK, UI_ACTION_BACK); // push button, connects gnd to pin
	UI_KEYS_BUTTON_LOW(UI_BUTTON_SD_PRINT, UI_ACTION_SD_PRINT ); // push button, connects gnd to pin
}

void UIDisplay::checkSlowEncoder() {}
void UIDisplay::checkSlowKeys(int &action) {}

#endif // FEATURE_CONTROLLER == 8 || FEATURE_CONTROLLER == 9, PiBot Expansion Port

#if FEATURE_CONTROLLER == 12 // FELIXPrinters Controller

#define UI_HAS_KEYS 1
#define UI_HAS_BACK_KEY 0
#define UI_DISPLAY_TYPE 1
#define UI_DISPLAY_CHARSET 1
#define UI_COLS 20
#define UI_ROWS 4
#define UI_ENCODER_SPEED 2
#define BEEPER_TYPE 0
#define BEEPER_PIN             -1
#define UI_DISPLAY_RS_PIN      16
#define UI_DISPLAY_RW_PIN      -1
#define UI_DISPLAY_ENABLE_PIN  17
#define UI_DISPLAY_D0_PIN      -1
#define UI_DISPLAY_D1_PIN      -1
#define UI_DISPLAY_D2_PIN      -1
#define UI_DISPLAY_D3_PIN      -1
#define UI_DISPLAY_D4_PIN      23
#define UI_DISPLAY_D5_PIN      25
#define UI_DISPLAY_D6_PIN      27
#define UI_DISPLAY_D7_PIN      29
#define UI_ENCODER_A           35
#define UI_ENCODER_B           37
#define UI_ENCODER_CLICK       31
#define UI_DELAYPERCHAR 320
#define UI_INVERT_MENU_DIRECTION false

void UIDisplay::initKeys()
{
	UI_KEYS_INIT_CLICKENCODER_LOW(UI_ENCODER_A, UI_ENCODER_B); // click encoder on pins 47 and 45. Phase is connected with gnd for signals.
	UI_KEYS_INIT_BUTTON_LOW(UI_ENCODER_CLICK); // push button, connects gnd to pin
}

void UIDisplay::checkKeys(int &action)
{
	UI_KEYS_CLICKENCODER_LOW_REV(UI_ENCODER_A, UI_ENCODER_B); // click encoder on pins 47 and 45. Phase is connected with gnd for signals.
	UI_KEYS_BUTTON_LOW(UI_ENCODER_CLICK, UI_ACTION_OK); // push button, connects gnd to pin
}

void UIDisplay::checkSlowEncoder() {}
void UIDisplay::checkSlowKeys(int &action) {}

#endif // FEATURE_CONTROLLER == 12, FELIXPrinters Controller

#if FEATURE_CONTROLLER == 13 // SeeMeCNC LCD + Rambo

#define UI_HAS_KEYS 1
#define UI_HAS_BACK_KEY 0
#define UI_DISPLAY_TYPE 1
#define UI_DISPLAY_CHARSET 1
#define UI_COLS 20
#define UI_ROWS 4
#define BEEPER_TYPE 1
#define BEEPER_PIN             79
#define UI_DISPLAY_RS_PIN      70
#define UI_DISPLAY_RW_PIN      -1
#define UI_DISPLAY_ENABLE_PIN  71
#define UI_DISPLAY_D0_PIN      -1
#define UI_DISPLAY_D1_PIN      -1
#define UI_DISPLAY_D2_PIN      -1
#define UI_DISPLAY_D3_PIN      -1
#define UI_DISPLAY_D4_PIN      72
#define UI_DISPLAY_D5_PIN      73
#define UI_DISPLAY_D6_PIN      74
#define UI_DISPLAY_D7_PIN      75
#define UI_ENCODER_A           76
#define UI_ENCODER_B           77
#define UI_ENCODER_CLICK       78
#define UI_KILL_PIN            80
#define UI_DELAYPERCHAR 320
#define UI_INVERT_MENU_DIRECTION true

void UIDisplay::initKeys()
{
	UI_KEYS_INIT_CLICKENCODER_LOW(UI_ENCODER_A, UI_ENCODER_B);
	UI_KEYS_INIT_BUTTON_LOW(UI_ENCODER_CLICK);
	UI_KEYS_INIT_BUTTON_LOW(UI_KILL_PIN);
}

void UIDisplay::checkKeys(int &action)
{
	UI_KEYS_CLICKENCODER_LOW_REV(UI_ENCODER_A, UI_ENCODER_B);
	UI_KEYS_BUTTON_LOW(UI_ENCODER_CLICK, UI_ACTION_OK);
	UI_KEYS_BUTTON_LOW(UI_KILL_PIN, UI_ACTION_KILL);
}

void UIDisplay::checkSlowEncoder() {}
void UIDisplay::checkSlowKeys(int &action) {}

#endif // FEATURE_CONTROLLER == 13, SeeMeCNC LCD + Rambo

#if FEATURE_CONTROLLER == 14

#define SDSUPPORT true
#define SDCARDDETECT -1
#define UI_HAS_KEYS 1
#define UI_HAS_BACK_KEY 1
#define UI_DISPLAY_TYPE 3
#define UI_DISPLAY_CHARSET 1
#define UI_COLS 20
#define UI_ROWS 4
#define UI_DISPLAY_I2C_CHIPTYPE 1
#define UI_DISPLAY_I2C_ADDRESS 0x40
#define UI_DISPLAY_I2C_OUTPUT_PINS 65504
#define UI_DISPLAY_I2C_OUTPUT_START_MASK 0
#define UI_DISPLAY_I2C_PULLUP 31
#define UI_I2C_CLOCKSPEED 400000L
#define UI_DISPLAY_RS_PIN _BV(15)
#define UI_DISPLAY_RW_PIN _BV(14)
#define UI_DISPLAY_ENABLE_PIN _BV(13)
#define UI_DISPLAY_D0_PIN _BV(12)
#define UI_DISPLAY_D1_PIN _BV(11)
#define UI_DISPLAY_D2_PIN _BV(10)
#define UI_DISPLAY_D3_PIN _BV(9)
#define UI_DISPLAY_D4_PIN _BV(12)
#define UI_DISPLAY_D5_PIN _BV(11)
#define UI_DISPLAY_D6_PIN _BV(10)
#define UI_DISPLAY_D7_PIN _BV(9)
#define UI_INVERT_MENU_DIRECTION false
#define UI_HAS_I2C_KEYS
#define UI_HAS_I2C_ENCODER 0
#define UI_I2C_KEY_ADDRESS 0x40

void UIDisplay::initKeys() {}
void UIDisplay::checkKeys(int &action) {}

void UIDisplay::checkSlowEncoder()
{
	i2cStartWait(UI_DISPLAY_I2C_ADDRESS+I2C_WRITE);
	i2cWrite(0x12); // GIOA
	i2cStop();
	i2cStartWait(UI_DISPLAY_I2C_ADDRESS+I2C_READ);
	unsigned int keymask = i2cReadAck();
	keymask = keymask + (i2cReadNak()<<8);
	i2cStop();
}

void UIDisplay::checkSlowKeys(int &action)
{
	i2cStartWait(UI_DISPLAY_I2C_ADDRESS+I2C_WRITE);
	i2cWrite(0x12); // GPIOA
	i2cStop();
	i2cStartWait(UI_DISPLAY_I2C_ADDRESS+I2C_READ);
	unsigned int keymask = i2cReadAck();
	keymask = keymask + (i2cReadNak()<<8);
	i2cStop();
	UI_KEYS_I2C_BUTTON_LOW(_BV(4), UI_ACTION_OK); // push button, connects gnd to pin
	UI_KEYS_I2C_BUTTON_LOW(_BV(1), UI_ACTION_BACK); // push button, connects gnd to pin
	UI_KEYS_I2C_BUTTON_LOW(_BV(0), UI_ACTION_SD_PRINT); // push button, connects gnd to pin
	UI_KEYS_I2C_BUTTON_LOW(_BV(3), UI_ACTION_PREVIOUS); // Up button
	UI_KEYS_I2C_BUTTON_LOW(_BV(2), UI_ACTION_NEXT); // down button
}

#endif // FEATURE_CONTROLLER == 14

#if FEATURE_CONTROLLER == 15, Sanguinololu + panelolu2

#define UI_HAS_KEYS 1
#define UI_HAS_BACK_KEY 0
#define UI_DISPLAY_TYPE 3
#define UI_DISPLAY_CHARSET 2
#define UI_COLS 20
#define UI_ROWS 4
#define UI_INVERT_MENU_DIRECTION false

#define UI_DISPLAY_I2C_CHIPTYPE 1
#define UI_DISPLAY_I2C_ADDRESS 0x40
#define UI_DISPLAY_I2C_OUTPUT_PINS 65528
#define UI_DISPLAY_I2C_OUTPUT_START_MASK 0
#define UI_DISPLAY_I2C_PULLUP 23
#define UI_I2C_CLOCKSPEED 100000L
//#define UI_HAS_I2C_KEYS
//#define UI_HAS_I2C_ENCODER 0
//#define UI_I2C_KEY_ADDRESS UI_DISPLAY_I2C_ADDRESS
#define BEEPER_TYPE 2
#define BEEPER_TYPE_INVERTING true
#define BEEPER_ADDRESS UI_DISPLAY_I2C_ADDRESS
#define COMPILE_I2C_DRIVER

#define UI_DISPLAY_RS_PIN 		_BV(15)
#define UI_DISPLAY_RW_PIN 		_BV(14)
#define UI_DISPLAY_ENABLE_PIN 	_BV(13)
#define UI_DISPLAY_D0_PIN 		_BV(12)
#define UI_DISPLAY_D1_PIN 		_BV(11)
#define UI_DISPLAY_D2_PIN 		_BV(10)
#define UI_DISPLAY_D3_PIN 		_BV(9)
#define UI_DISPLAY_D4_PIN 		_BV(12)
#define UI_DISPLAY_D5_PIN 		_BV(11)
#define UI_DISPLAY_D6_PIN 		_BV(10)
#define UI_DISPLAY_D7_PIN 		_BV(9)
#define BEEPER_PIN _BV(5)
#define UI_I2C_HEATBED_LED    _BV(8)
#define UI_I2C_HOTEND_LED     _BV(7)
#define UI_I2C_FAN_LED        _BV(6)

void UIDisplay::initKeys()
{
	UI_KEYS_INIT_CLICKENCODER_LOW(10, 11); // click encoder on pins 47 and 45. Phase is connected with gnd for signals.
	UI_KEYS_INIT_BUTTON_LOW(30); // push button, connects gnd to pin
}

void UIDisplay::checkKeys(int &action)
{
	 UI_KEYS_CLICKENCODER_LOW_REV(10, 11); // click encoder on pins 47 and 45. Phase is connected with gnd for signals.
	 UI_KEYS_BUTTON_LOW(30,UI_ACTION_OK); // push button, connects gnd to pin
}

void UIDisplay::checkSlowEncoder() {}

void UIDisplay::checkSlowKeys(int &action) {}

#endif // FEATURE_CONTROLLER == 15, Sanguinololu + panelolu2

#if FEATURE_CONTROLLER > 0
#if UI_ROWS == 4
#if UI_COLS == 16

#define UI_LINE_OFFSETS {0, 0x40, 0x10, 0x50} // 4x16

#elif UI_COLS == 20

//#define UI_LINE_OFFSETS {0, 0x20, 0x40, 0x60} // 4x20 with KS0073
#define UI_LINE_OFFSETS {0, 0x40, 0x14, 0x54} // 4x20 with HD44780

#else // UI_COLS != 16 && UI_COLS != 20

#error Unknown combination off rows/columns - define UI_LINE_OFFSETS manually.

#endif // UI_COLS

#else // UI_ROWS != 4

#define UI_LINE_OFFSETS {0, 0x40, 0x10, 0x50} // 2x16, 2x20, 2x24

#endif // UI_ROWS

#include "ui_menu.h"

#endif // FEATURE_CONTROLLER > 0

#define UI_VERSION_STRING "Repetier " REPETIER_VERSION

#ifdef UI_HAS_I2C_KEYS
#define COMPILE_I2C_DRIVER
#endif

#if UI_DISPLAY_TYPE != 0


#if UI_DISPLAY_TYPE == 3
#define COMPILE_I2C_DRIVER
#endif

#ifndef UI_TEMP_PRECISION
#if UI_COLS > 16
#define UI_TEMP_PRECISION 1
#else
#define UI_TEMP_PRECISION 0
#endif
#endif

#define UI_INITIALIZE initialize();
#define UI_FAST if(pwm_count & 4) {fastAction();}
#define UI_MEDIUM mediumAction();
#define UI_SLOW slowAction();
#define UI_STATUS(status) setStatus(PSTR(status));
#define UI_STATUS_UPD(status) { setStatus(PSTR(status)); refreshPage() ;}
#define UI_STATUS_RAM(status) setStatus(status);
#define UI_STATUS_UPD_RAM(status) { setStatus(status); refreshPage(); }
#define UI_ERROR(status) setStatus(PSTR(status), true);
#define UI_ERROR_UPD(status) { setStatus(PSTR(status), true); refreshPage(); }
#define UI_ERROR_RAM(status) setStatus(status, true);
#define UI_ERROR_UPD_RAM(status) {setStatus(status, true); refreshPage(); }
//#define UI_ERROR(msg) {errorMsg = msg; pushMenu((void*)&ui_menu_error, true); }
#define UI_CLEAR_STATUS { statusMsg[0] = 0; }
#else
#define UI_INITIALIZE {}
#define UI_FAST {}
#define UI_MEDIUM {}
#define UI_SLOW {}
#define UI_STATUS(status) {}
#define UI_STATUS_RAM(status) {}
#define UI_STATUS_UPD(status) {}
#define UI_STATUS_UPD_RAM(status) {}
#define UI_CLEAR_STATUS {}
#define UI_ERROR(msg) {}
#define UI_ERROR_UPD(status) {}
#define UI_ERROR_RAM(status) {}
#define UI_ERROR_UPD_RAM(status) {}
#endif  // Display

#endif
