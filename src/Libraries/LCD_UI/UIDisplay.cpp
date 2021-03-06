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

#include <math.h>
#include <stdlib.h>
#include <inttypes.h>
#include <ctype.h>

//#include "Pins.h"
#include "RepRapFirmware.h"
#include "I2C.h"
#include "ui_private.h"

static uint16_t menuMode = 0;

void UIDisplay::setMode(uint16_t flags, bool set)
{
	if (set)
	{
		menuMode |= flags;
	}
	else
	{
		menuMode &= ~flags;
	}
}

#define INCREMENT_MIN_MAX(a, steps, _min, _max)							\
	if ((increment < 0) &&												\
		((_min) >= 0) &&												\
		((a) < ((_min) - (increment * (steps))))) {						\
		(a) = (_min);													\
	}																	\
	else {																\
		(a) += increment * (steps);										\
		if ((a) < (_min)) (a) = (_min);									\
		else if ((a) > (_max)) (a)= (_max);								\
	}

static int whichHeater(int tool, int def);
static size_t currentHeater(size_t def);
static bool whichDriveReally(size_t *drive);
static bool sdPresent(void);
static void allowInterrupts(void);
static void forbidInterrupts(void);

static bool ui_has_error_message = false;

#define IS_UI_ERROR_MESSAGE() ui_has_error_message
#define SET_UI_ERROR_MESSAGE(x) ui_has_error_message = x

#if UI_ANIMATION

static bool animation = false;
#define toggleAnimation() animation != animation;
#define isAnimation() (animation)
#define setAnimation(x) animation = (x)

#else

#define toggleAnimation()
#define isAnimation() (false)
#define setAnimation(x)

#endif

#include "ui_beeper.h"

bool UIMenuEntry::showEntry() const
{
    bool ret = true;
    uint8_t f, f2;
    f = filter;
    if (f != 0)
        ret = (f & menuMode) != 0;
    f2 = nofilter;
    if (ret && (f2 != 0))
    {
        ret = (f2 & menuMode) == 0;
    }
    return ret;
}

static const uint32_t baudrates[] = {
	  9600,   14400,   19200,  28800,  38400,  56000,  57600,  76800,
	111112,  115200,  128000, 230400, 250000, 256000, 460800, 500000,
    921600, 1000000, 1500000,      0 };

#include "lcd_primitives.h"

#if UI_DISPLAY_TYPE == 1 || UI_DISPLAY_TYPE == 2 || UI_DISPLAY_TYPE == 3

void UIDisplay::createChar(uint8_t location, const uint8_t charmap[])
{
    location &= 0x7; // we only have 8 locations, 0 - 7
    lcdCommand(LCD_SETCGRAMADDR | (location << 3));
    for (int i = 0; i < 8; i++)
    {
        lcdPutChar(charmap[i]);
    }
}

#endif // UI_DISPLAY_TYPE == 1 || UI_DISPLAY_TYPE == 2 || UI_DISPLAY_TYPE == 3

#if UI_DISPLAY_TYPE == 1 || UI_DISPLAY_TYPE == 2

void UIDisplay::initializeLCD()
{
#include "ui_symbols.h"
    // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
    // according to datasheet, we need at least 40ms after power rises above 2.7V
    // before sending commands. Arduino can turn on way before 4.5V.
    // is this delay long enough for all cases??
	// ???? necessary
    delay(235);
    SET_OUTPUT(UI_DISPLAY_D4_PIN);
    SET_OUTPUT(UI_DISPLAY_D5_PIN);
    SET_OUTPUT(UI_DISPLAY_D6_PIN);
    SET_OUTPUT(UI_DISPLAY_D7_PIN);
    SET_OUTPUT(UI_DISPLAY_RS_PIN);
#if UI_DISPLAY_RW_PIN > -1
    SET_OUTPUT(UI_DISPLAY_RW_PIN);
#endif
    SET_OUTPUT(UI_DISPLAY_ENABLE_PIN);

    // Now we pull both RS and R/W low to begin commands
    WRITE(UI_DISPLAY_RS_PIN, LOW);
    WRITE(UI_DISPLAY_ENABLE_PIN, LOW);

    //put the LCD into 4 bit mode
    // this is according to the hitachi HD44780 datasheet
    // figure 24, pg 46

    // we start in 8bit mode, try to set 4 bit mode
    // at this point we are in 8 bit mode but of course in this
    // interface 4 pins are dangling unconnected and the values
    // on them don't matter for these instructions.
    WRITE(UI_DISPLAY_RS_PIN, LOW);
    delayMicroseconds(10);
    lcdWriteNibble(0x03);
    delayMicroseconds(5000); // I have one LCD for which 4500 here was not long enough.
    // second try
    lcdWriteNibble(0x03);
    delayMicroseconds(150); // wait
    // third go!
    lcdWriteNibble(0x03);
    delayMicroseconds(150);
    // finally, set to 4-bit interface
    lcdWriteNibble(0x02);
    delayMicroseconds(150);
    // finally, set # lines, font size, etc.
    lcdCommand(LCD_4BIT | LCD_2LINE | LCD_5X7);

    lcdCommand(LCD_CLEAR);					//-	Clear Screen
    delay(5); // clear is slow operation
    lcdCommand(LCD_INCREASE | LCD_DISPLAYSHIFTOFF);	//-	Entrymode (Display Shift: off, Increment Address Counter)
    lcdCommand(LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKINGOFF);	//-	Display on
	resetTimes();
    createChar(1, character_back);
    createChar(2, character_degree);
    createChar(3, character_selected);
    createChar(4, character_unselected);
    createChar(5, character_temperature);
    createChar(6, character_folder);
    createChar(7, character_ready);
}

#endif // UI_DISPLAY_TYPE == 1 || UI_DISPLAY_TYPE == 2

#if UI_DISPLAY_TYPE == 3

void UIDisplay::initializeLCD()
{
#include "ui_symbols.h"

	// ???? Necessary?
	// delay(235);
    lcdStartWrite();
    i2cWrite(outputMask & 255);
#if UI_DISPLAY_I2C_CHIPTYPE == 1
    i2cWrite(outputMask >> 8);
#endif
    delayMicroseconds(10);
    lcdWriteNibble(0x03);
    delayMicroseconds(5000); // I have one LCD for which 4500 here was not long enough.
    // second try
    lcdWriteNibble(0x03);
    delayMicroseconds(150); // wait
    // third go!
    lcdWriteNibble(0x03);
    delayMicroseconds(150);
    // finally, set to 4-bit interface
    lcdWriteNibble(0x02);
    delayMicroseconds(150);
    // finally, set # lines, font size, etc.
    lcdCommand(LCD_4BIT | LCD_2LINE | LCD_5X7);
    lcdCommand(LCD_CLEAR);					//-	Clear Screen
    delay(2); // clear is slow operation
    lcdCommand(LCD_INCREASE | LCD_DISPLAYSHIFTOFF);	//-	Entrymode (Display Shift: off, Increment Address Counter)
    lcdCommand(LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKINGOFF);	//-	Display on
	resetTimes();
	createChar(1, character_back);
    createChar(2, character_degree);
    createChar(3, character_selected);
    createChar(4, character_unselected);
    createChar(5, character_temperature);
    createChar(6, character_folder);
    createChar(7, character_ready);
    lcdStopWrite();
}

#endif // UI_DISPLAY_TYPE == 3

#if UI_DISPLAY_TYPE < 4

void UIDisplay::printRow(uint8_t r, const char *txt, const char *txt2,
						 uint8_t changeAtCol)
{
    changeAtCol = MIN(UI_COLS, changeAtCol);
    uint8_t col = 0;

    // Set row
    if (r >= UI_ROWS) return;

#if UI_DISPLAY_TYPE == 3
    lcdStartWrite();
#endif

	// Position cursor
    lcdWriteByte(128 + LCDLineOffsets[r], 0);

    char c;
    while (((c = *txt) != 0x00) && (col < changeAtCol))
    {
        txt++;
        lcdPutChar(c);
        col++;
    }
    while (col < changeAtCol)
    {
        lcdPutChar(' ');
        col++;
    }
    if (txt2 != NULL)
    {
        while (((c = *txt2) != 0x00) && (col < UI_COLS))
        {
            txt2++;
            lcdPutChar(c);
            col++;
        }
        while (col < UI_COLS)
        {
            lcdPutChar(' ');
            col++;
        }
    }

#if UI_DISPLAY_TYPE == 3
    lcdStopWrite();
#endif

#if UI_HAS_KEYS == 1 && UI_HAS_I2C_ENCODER > 0
    checkSlowEncoder();
#endif
}

#endif // UI_DISPLAY_TYPE < 4

#if UI_DISPLAY_TYPE == 4

void UIDisplay::createChar(uint8_t location, const uint8_t charmap[])
{
    location &= 0x7; // we only have 8 locations 0-7
    lcd.createChar(location, charmap);
}

void UIDisplay::printRow(uint8_t r, const char *txt, const char *txt2,
						 uint8_t changeAtCol)
{
    changeAtCol = MIN(UI_COLS, changeAtCol);
    uint8_t col = 0;
// Set row
    if (r >= UI_ROWS) return;
    lcd.setCursor(0, r);
    char c;
    while (((c = *txt) != 0x00) && (col < changeAtCol))
    {
        txt++;
        lcd.write(c);
        col++;
    }
    while (col < changeAtCol)
    {
        lcd.write(' ');
        col++;
    }
    if (txt2 != NULL)
    {
        while (((c = *txt2) != 0x00) && (col < UI_COLS))
        {
            txt2++;
            lcd.write(c);
            col++;
        }
        while (col < UI_COLS)
        {
            lcd.write(' ');
            col++;
        }
    }
#if UI_HAS_KEYS == 1 && UI_HAS_I2C_ENCODER > 0
    checkSlowEncoder();
#endif
}

void UIDisplay::initializeLCD()
{
#define UI_SYMBOLS_PARTIAL
#include "ui_symbols.h"
#undef UI_SYMBOLS_PARTIAL
 
	lcd.begin(UI_COLS, UI_ROWS);
    resetTimes();
    createChar(1, character_back);
    createChar(2, character_degree);
    createChar(3, character_selected);
    createChar(4, character_unselected);
}

#endif // UI_DISPLAY_TYPE == 4

#if UI_DISPLAY_TYPE == 5

void UIDisplay::printRow(uint8_t r, const char *txt, const char *txt2,
						 uint8_t changeAtCol)
{
    changeAtCol = MIN(UI_COLS, changeAtCol);
    uint8_t col = 0;
// Set row
    if (r >= UI_ROWS) return;
    int y = r*UI_FONT_HEIGHT;
    if (!u8g_IsBBXIntersection(&u8g, 0, y, UI_LCD_WIDTH, UI_FONT_HEIGHT + 2))
		return; // row not visible
    u8_tx = 0;
    u8_ty = y + UI_FONT_HEIGHT; //set position
    bool highlight = ((uint8_t)(*txt) == CHAR_SELECTOR) ||
		((uint8_t)(*txt) == CHAR_SELECTED);
    if (highlight)
    {
        u8g_SetColorIndex(&u8g, 1);
        u8g_draw_box(&u8g, 0, y+1, u8g_GetWidth(&u8g), UI_FONT_HEIGHT+1);
        u8g_SetColorIndex(&u8g, 0);
    }
    char c;
    while (((c = *txt++) != 0) && (col < changeAtCol))
    {
        u8PrintChar(c);
        col++;
    }

    if (txt2 != NULL)
    {
        col = changeAtCol;
        u8_tx = col * UI_FONT_WIDTH; //set position
        while (((c=*(txt2++)) != 0) && (col < UI_COLS))
        {
            u8PrintChar(c);
            col++;
        }
    }

    if (highlight)
    {
        u8g_SetColorIndex(&u8g, 1);
    }

#if UI_HAS_KEYS == 1 && UI_HAS_I2C_ENCODER > 0
    checkSlowEncoder();
#endif
}

void UIDisplay::initializeLCD()
{
#ifdef U8GLIB_ST7920
//U8GLIB_ST7920_128X64_1X u8g(UI_DISPLAY_D4_PIN, UI_DISPLAY_ENABLE_PIN, UI_DISPLAY_RS_PIN);
    u8g_InitSPI(&u8g, &u8g_dev_st7920_128x64_sw_spi,
				UI_DISPLAY_D4_PIN, UI_DISPLAY_ENABLE_PIN, UI_DISPLAY_RS_PIN,
				U8G_PIN_NONE, U8G_PIN_NONE);
#endif
    u8g_Begin(&u8g);
    //u8g.firstPage();
    u8g_FirstPage(&u8g);
    do
    {
        u8g_SetColorIndex(&u8g, 0);
    }
    while(u8g_NextPage(&u8g));

    u8g_SetFont(&u8g, UI_FONT_DEFAULT);
    u8g_SetColorIndex(&u8g, 1);
	resetTimes();
}

#endif // UI_DISPLAY_TYPE == 5

// ???? not member of class
static char printCols[MAX_COLS+1];

UIDisplay::UIDisplay(Platform* p, Heat* h, GCodes *g) : active(false), platform(p), heat(h)
{
	buf.Init();
	if (g) g->lcdUiBuffer(&buf);
}

#if UI_ANIMATION

static void slideIn(uint8_t row, const char *text)
{
    char *empty = "";
    int8_t i = 0;

    col = 0;
    addString(text);
    printCols[col] = 0;
    for (i = UI_COLS-1; i >= 0; i--)
    {
        printRow(row, empty, printCols, i);
        HAL::pingWatchdog();
        delay(10);
    }
}

#endif // UI_ANIMATION

void UIDisplay::Init()
{
	// Re-init
	buf.Init();

	menuMode = 0;
#if UI_ANIMATION
    oldMenuLevel = -2;
#endif

#if UI_AUTORETURN_TO_MENU_AFTER != 0
	ui_autoreturn_time = 0;
#endif

#ifdef COMPILE_I2C_DRIVER
    outputMask = UI_DISPLAY_I2C_OUTPUT_START_MASK;
#if UI_DISPLAY_I2C_CHIPTYPE == 0 && BEEPER_TYPE == 2 && BEEPER_PIN >= 0
#if BEEPER_ADDRESS == UI_DISPLAY_I2C_ADDRESS
    outputMask |= BEEPER_PIN;
#endif // BEEPER_ADDRESS == UI_DISPLAY_I2C_ADDRESS
#endif // UI_DISPLAY_I2C_CHIPTYPE == 0 && BEEPER_TYPE == 2 && BEEPER_PIN >= 0
    i2cInit(UI_I2C_CLOCKSPEED);
#if UI_DISPLAY_I2C_CHIPTYPE == 1
    // set direction of pins
    i2cStart(UI_DISPLAY_I2C_ADDRESS + I2C_WRITE);
    i2cWrite(0); // IODIRA
    i2cWrite(~(UI_DISPLAY_I2C_OUTPUT_PINS & 255));
    i2cWrite(~(UI_DISPLAY_I2C_OUTPUT_PINS >> 8));
    i2cStop();
    // Set pullups according to  UI_DISPLAY_I2C_PULLUP
    i2cStart(UI_DISPLAY_I2C_ADDRESS + I2C_WRITE);
    i2cWrite(0x0C); // GPPUA
    i2cWrite(UI_DISPLAY_I2C_PULLUP & 255);
    i2cWrite(UI_DISPLAY_I2C_PULLUP >> 8);
    i2cStop();
#endif // UI_DISPLAY_I2C_CHIPTYPE == 1
#endif // COMPILE_I2C_DRIVER
    flags = 0;
    menuLevel = 0;
    shift = -2;
    menuPos[0] = 0;
    lastAction = 0;
    lastButtonAction = 0;
    activeAction = 0;
    statusMsg[0] = 0;
    initKeys();
    cwd[0] = 0;
	cwd_skip = 0;
    folderLevel = 0;
    UI_STATUS(UI_TEXT_PRINTER_READY);
#if UI_DISPLAY_TYPE > 0
    initializeLCD();
#if UI_DISPLAY_TYPE == 3
    // I don't know why but after power up the lcd does not come up
    // but if I reinitialize i2c and the lcd again here it works.
    delay(10);
    i2cInit(UI_I2C_CLOCKSPEED);
	// set direction of pins
    i2cStart(UI_DISPLAY_I2C_ADDRESS + I2C_WRITE);
    i2cWrite(0); // IODIRA
    i2cWrite(~(UI_DISPLAY_I2C_OUTPUT_PINS & 255));
    i2cWrite(~(UI_DISPLAY_I2C_OUTPUT_PINS >> 8));
    i2cStop();
    // Set pullups according to  UI_DISPLAY_I2C_PULLUP
    i2cStart(UI_DISPLAY_I2C_ADDRESS + I2C_WRITE);
    i2cWrite(0x0C); // GPPUA
    i2cWrite(UI_DISPLAY_I2C_PULLUP & 255);
    i2cWrite(UI_DISPLAY_I2C_PULLUP >> 8);
    i2cStop();
    initializeLCD();
#endif // UI_DISPLAY_TYPE == 3
#if UI_ANIMATION == 0 || UI_DISPLAY_TYPE == 5
#if UI_DISPLAY_TYPE == 5
    //u8g picture loop
    u8g_FirstPage(&u8g);
    do
    {
#endif // UI_DISPLAY_TYPE == 5
        for (uint8_t y = 0; y < UI_ROWS; y++) displayCache[y][0] = 0;
        printRow(0, NAME " " VERSION);
        printRow(1, platform->GetElectronicsString());
#if UI_ROWS > 2
        printRow(UI_ROWS - 1, DATE);
#endif // UI_ROWS > 2
#if UI_DISPLAY_TYPE == 5
    }
    while (u8g_NextPage(&u8g));  //end picture loop
#endif // UI_DISPLAY_TYPE == 5
#else // !(UI_ANIMATION == 0 || UI_DISPLAY_TYPE == 5)
    slideIn(0, NAME " " VERSION);
    strcpy(displayCache[0], printCols);
    slideIn(1, platform->GetElectronicsString());
    strcpy(displayCache[1], printCols);
#if UI_ROWS > 2
    slideIn(UI_ROWS-1, DATE);
    strcpy(displayCache[UI_ROWS-1], printCols);
#endif // UI_ROWS > 2
#endif // UI_ANIMATION == false || UI_DISPLAY_TYPE == 5
    delay(UI_START_SCREEN_DELAY);
#endif // UI_DISPLAY_TYPE > 0
#if UI_DISPLAY_I2C_CHIPTYPE == 0 && (BEEPER_TYPE == 2 || defined(UI_HAS_I2C_KEYS))
    // Make sure the beeper is off
    i2cStartWait(UI_I2C_KEY_ADDRESS + I2C_WRITE);
    i2cWrite(255); // Disable beeper, enable read for other pins.
    i2cStop();
#endif // UI_DISPLAY_I2C_CHIP == 0 && (BEEPER_TYPE == 2 || defined(UI_HAS_I2C_KEYS))

	active = true;
}

void  UIDisplay::waitForKey()
{
    int nextAction = 0;

    lastButtonAction = 0;
    while(lastButtonAction == nextAction)
    {
        checkSlowKeys(nextAction);
    }
}

void UIDisplay::printRow(uint8_t r, const char *txt)
{
    if (r >= UI_ROWS) return;
    col = 0;
    addString(txt);
    printCols[col] = 0;
    printRow(r, printCols, NULL, UI_COLS);
}

void UIDisplay::addInt(int value, uint8_t digits, char fillChar)
{
    uint8_t dig = 0, neg = 0;

    if (value < 0)
    {
        value = -value;
        neg=1;
        dig++;
    }
    char buf[7]; // Assumes 8-bit chars plus zero byte.
    char *str = &buf[6];
    buf[6] = 0;
    do
    {
        unsigned int m = value;
        value /= 10;
        char c = m - 10 * value;
        *--str = c + '0';
        dig++;
    }
    while (value);
    if (neg)
        printCols[col++]='-';
    if (digits < 6)
	{
        while (dig < digits)
        {
            *--str = fillChar; //' ';
            dig++;
        }
	}
    while (*str && (col < MAX_COLS))
    {
        printCols[col++] = *str;
        str++;
    }
}

void UIDisplay::addLong(long value, char digits)
{
    uint8_t dig = 0, neg = 0;

    if (value < 0)
    {
        neg = 1;
        value = -value;
        dig++;
    }

    char buf[13]; // Assumes 8-bit chars plus zero byte.
    char *str = &buf[12];
    buf[12] = 0;
    do
    {
        unsigned long m = value;
        value /= 10;
        char c = m - 10 * value;
        *--str = c + '0';
        dig++;
    }
    while (value);

    if (neg)
        printCols[col++]='-';
    if (digits <= 11)
	{
        while (dig < digits)
        {
            *--str = ' ';
            dig++;
        }
	}

    while(*str && (col < MAX_COLS))
    {
        printCols[col++] = *str;
        str++;
    }
}

static const float roundingTable[] = {0.5, 0.05, 0.005, 0.0005};

void UIDisplay::addFloat(float number, char fixdigits, uint8_t digits)
{
    // Handle negative numbers
    if (number < 0.0)
    {
        printCols[col++] = '-';
        if (col >= MAX_COLS) return;
        number = -number;
        fixdigits--;
    }
    number += roundingTable[digits]; // for correct rounding

    // Extract the integer part of the number and print it
    unsigned long int_part = (unsigned long)number;
    float remainder = number - (float)int_part;
    addLong(int_part, fixdigits);

    if (col >= UI_COLS) return;

    // Print the decimal point, but only if there are digits beyond
    if (digits > 0)
    {
        printCols[col++]='.';
    }

    // Extract digits from the remainder one at a time
    while ((col < MAX_COLS) && (digits-- > 0))
    {
        remainder *= 10.0;
        uint8_t toPrint = uint8_t(remainder);
        printCols[col++] = '0'+toPrint;
        remainder -= toPrint;
    }
}

void UIDisplay::addString(const char *text)
{
	uint8_t c;

    while ((col < MAX_COLS) && (c = *text++))
    {
        printCols[col++] = c;
    }
}

static UI_STRING(ui_text_on, UI_TEXT_ON);
static UI_STRING(ui_text_off, UI_TEXT_OFF);
static UI_STRING(ui_yes, UI_TEXT_YES);
static UI_STRING(ui_no, UI_TEXT_NO);
static UI_STRING(ui_selected, UI_TEXT_SEL);
static UI_STRING(ui_unselected, UI_TEXT_NOSEL);
static UI_STRING(ui_action, UI_TEXT_STRING_ACTION);

void UIDisplay::parse(const char *txt)
{
    int ivalue = 0;
    float fvalue = 0;

    while (col < MAX_COLS)
    {
        char c = *txt++;
        if (c == 0) break; // finished
        if (c != '%')
        {
            printCols[col++] = c;
            continue;
        }
        // dynamic parameter, parse meaning and replace
        char c1 = *txt++;
        char c2 = *txt++;
        switch(c1)
        {
        case '%':
            if((c2 == '%') && (col < MAX_COLS))
                printCols[col++]='%';
            break;

		// ???
		// Change for ALL drives?
		// Add deltaV tag?
        case 'a': // Acceleration settings
			if (c2 == 'c')
			{
				// ???? #####
			}
			else
			{
				ivalue = c2 - '0';
			}
			if (0 <= ivalue && ivalue <= (int)(DRIVES - 1))
				addFloat(platform->Acceleration((size_t)(0x7f & ivalue)), 5, 0);
            break;

		case 'j': // Speed changes; dv's
			ivalue = c2 - '0';
			if (0 <= ivalue && ivalue <= (int)(DRIVES - 1))
			{
				addFloat(platform->ConfiguredInstantDv((size_t)(0x7f & ivalue)), 5, 0);
			}
            break;

        case 'e': // Extruder temperature
		{
			int ipre = UI_TEMP_PRECISION;
			ivalue = (int)(0x7f & currentHeater(1));
			if (c2 == 'r')   // Extruder relative mode
            {
                addString(reprap.GetGCodes()->DrivesRelative() ? ui_yes : ui_no);
                break;
            }
            if (heat->GetStatus((int8_t)ivalue) == Heat::HS_fault)
            {
                addString(" def ");
                break;
            }
			if (c2 == 'b')
			{
				// Bed temperature
				ivalue = 0;
				ipre = 0;
			}
            else if (c2 != 'c')
			{
				ivalue = whichHeater(c2 - '0', 1);
			}
            addFloat(heat->GetTemperature((int8_t)ivalue), 3, ipre);
            break;
		}

        case 'E': // Target extruder temperature
			ivalue = (int)(0x7f & currentHeater(1));
			if (c2 == 'b')
			{
				ivalue = 0;
			}
            else if (c2 != 'c')
			{
				ivalue = whichHeater(c2 - '0', 1);
			}
            addFloat(heat->GetActiveTemperature((int8_t)ivalue), 3, 0);
            break;

#if COOLING_FAN0_PIN > -1
        case 'F': // FAN speed
			if (c2 == 's')
			{
				float fv = platform->GetFanValue(0);
				if (c2 == 's') addInt((int)(fv * 100.0), 3);
			}
			break;
#endif

        case 'f':
		{
			size_t drive;
			if (c2 == 'e')
			{
				drive = AXES;
				whichDriveReally(&drive);
			}
			else
			{
				drive = c2 - '0';
			}
			if ((0 <= drive) && (drive <= (DRIVES - 1)))
			{
				addFloat(platform->MaxFeedrate(drive), 5, 0);
			}
			break;
		}

        case 'o':
            if (c2 == 's')
            {
				float fraction = reprap.GetPrintMonitor()->IsPrinting() ?
					reprap.GetGCodes()->FractionOfFilePrinted() : -1.0;
                if (fraction >= 0.0)
                {
                    addString(UI_TEXT_PRINT_POS);
					int percent = (int)(fraction * 100.0);
                    addInt(percent, 3);
                    if (col < MAX_COLS)
                        printCols[col++]='%';
                }
                else
                    parse(statusMsg);
                break;
            }

            if (c2 == 'c')
            {
                addLong(platform->GetBaudRate(0), 6);
                break;
            }

			// ?????
            if (c2 == 'e')
            {
                if (errorMsg != 0) addString((char *)errorMsg);
                break;
            }

            if (c2 == 'f')
            {
				if (reprap.GetCurrentTool() != NULL)
				{
					int extruder = reprap.GetCurrentTool()->Number();
					if ((extruder < 0) || (extruder >= (int)(DRIVES - AXES))) extruder = 0;
					addInt((int)(100.0 * reprap.GetGCodes()->GetExtrusionFactor((size_t)extruder)), 3);
				}
                break;
            }

            if (c2 == 'm')
            {
                addInt((int)(reprap.GetGCodes()->GetSpeedFactor() * 100.0), 3);
                break;
            }

            // Extruder output level
			{
				// Heater PWM %
				int heater = 1;
				if (c2 == 'c')
				{
					// Active tool temp.  Just report the first heater assigned to this tool
					heater = currentHeater(heater);
				}
				else
				{
					// ????
					// All possible heaters or just those in use???
					int iv = c2 - '0';
					if ((0 <= iv) && (iv <= (reprap.GetHeatersInUse() - 1))) heater = iv;
				}
				ivalue = (int)(100.0 * heat->GetAveragePWM((int8_t)(0x7F & heater)));
				addInt(ivalue, 3);
				if (col < MAX_COLS)
					printCols[col++]='%';
				break;
			}

        case 'x':
	    case 'y':
			ivalue = c2 - '0';
			if (0 <= ivalue && ivalue <= (int)(DRIVES - 1))
			{
				float liveCoordinates[DRIVES];
				reprap.GetMove()->LiveCoordinates(liveCoordinates);
				if ((ivalue < (int)AXES) && (reprap.GetCurrentTool() != NULL))
				{
					const float *offset = reprap.GetCurrentTool()->GetOffset();
					liveCoordinates[ivalue] += offset[ivalue];
				}
				fvalue = liveCoordinates[ivalue];
			}
            addFloat(fvalue, 4, 2);
            break;

        case 'X': // Extruder related
		{
            if ((c2 >= '0') && (c2 <= '9'))
            {
				addString(((reprap.GetCurrentTool() != NULL) &&
						   (reprap.GetCurrentTool()->Number() == (int)(c2 - '0'))) ? ui_selected : ui_unselected);
				break;
            }
			ivalue = ((reprap.GetCurrentTool() != NULL) && (reprap.GetCurrentTool()->HeaterCount() > 0)) ? 
				reprap.GetCurrentTool()->Heater(0) : 1;
			PidParameters pp = platform->GetPidParameters(ivalue);
			switch (c2)
			{
			case 'i' : addFloat(pp.kI, 4, 2); break;
			case 'p' : addFloat(pp.kP, 4, 2); break;
			case 'd' : addFloat(pp.kD, 4, 2); break;
			case 'm' : addFloat(pp.pidMin, 4, 2); break;
			case 'M' : addFloat(pp.pidMax, 4, 2); break;
			case 'e' :
            {
				ivalue = ((reprap.GetCurrentTool() != NULL) && (reprap.GetCurrentTool()->DriveCount() > 0)) ?
					reprap.GetCurrentTool()->Drive(0) : 0;
                addFloat(platform->GetElasticComp(ivalue + AXES), 2, 3);
				break;
            }
			case 'x' :
			case 'y' :
            {
				if (reprap.GetCurrentTool() != NULL)
				{
					const float *offset = reprap.GetCurrentTool()->GetOffset();
					fvalue = (c2 == 'x') ? offset[0] : offset[1];
				}
                addFloat(fvalue, 4, 2);
				break;
            }
			case 'F' :
            {
				if ((reprap.GetCurrentTool() != NULL) && (reprap.GetCurrentTool()->DriveCount() > 0))
				{
					addFloat(reprap.GetCurrentTool()->MaxFeedrate(), 5, 0);
				}
				break;
            }
			case 'A' :
            {
				if ((reprap.GetCurrentTool() != NULL) && (reprap.GetCurrentTool()->DriveCount() > 0))
				{
					addFloat(reprap.GetCurrentTool()->MaxAcceleration(), 5, 0);
				}
				break;
            }
			default : break;
			}
            break;
		}

        case 's': // Endstop positions
		{
			bool check, min;
			size_t drive;
			EndStopType etype;

			switch(c2)
			{
			default  : // fallthrough
			case 'x' : drive = 0; min = true;  break;
			case 'X' : drive = 0; min = false; break;
			case 'y' : drive = 1; min = true;  break;
			case 'Y' : drive = 1; min = false; break;
			case 'z' : drive = 2; min = true;  break;
			case 'Z' : drive = 2; min = false; break;
			}

			platform->GetEndStopConfiguration(drive, etype, check);
			check = false;
			switch(etype)
			{
			case EndStopType::highEndStop : check = !min; break;
			case EndStopType::lowEndStop  : check = min;  break;
			default : break;
			}

			if (!check)
			{
				// No endstop or endstop is the wrong type -- don't bother checking
				addString(ui_no);
				break;
			}

			switch (platform->Stopped(drive))
			{
			default :
			case EndStopHit::noStop  : addString(ui_text_off);                    break;
			case EndStopHit::lowHit  : addString(min ? ui_text_on : ui_text_off); break;
			case EndStopHit::highHit : addString(min ? ui_text_off : ui_text_on); break;
			}
            break;
		}

        case 'S':
		{
			size_t drive = 0;
			
			if (c2 == 'e' || c2 == 'c')
			{
				drive = AXES;
				whichDriveReally(&drive);
			}
			else
				drive = c2 - '0';
			fvalue = platform->DriveStepsPerUnit(drive);
			addFloat(fvalue, 3, 1);
            break;
		}

        case 'P':
            if (c2 == 'N') addString(platform->GetElectronicsString());
            break;

        case 'U':
            if (c2 == 't')   // Printing time
            {
				// ????
				// Repetier checked to see if all heaters were off and if so, displayed 0
				fvalue = reprap.GetPrintMonitor()->GetPrintDuration();
                long tmp = (long)(fvalue / 86400.0);
                fvalue -= (float)tmp * 86400.0;
                addInt(tmp, 5);
                addString(UI_TEXT_PRINTTIME_DAYS);
                tmp = (long)(fvalue / 3600.0);
                addInt(tmp, 2);
                addString(UI_TEXT_PRINTTIME_HOURS);
                fvalue -= (float)tmp * 3600.0;
                tmp = (long)(fvalue / 60.0);
                addInt(tmp, 2, '0');
                addString(UI_TEXT_PRINTTIME_MINUTES);
            }
            else if (c2 == 'f')     // Filament usage
            {
#if 0
				// ????
				// Not tracked by RepRapFirmware
                float dist = Printer::filamentPrinted*0.001 + HAL::eprGetFloat(EPR_PRINTING_DISTANCE);
                addFloat(dist, 6, 1);
#endif
            }
        }
    }
    printCols[col] = 0;
}

void UIDisplay::setStatus(const char *txt, bool error)
{
    if (!error && IS_UI_ERROR_MESSAGE()) return;
    uint8_t i = 0;
    while ((*txt != 0) && (i < 16))
        statusMsg[i++] = *txt++;
    statusMsg[i] = '\0';
    if (error)
        SET_UI_ERROR_MESSAGE(true);
}

static const UIMenu *const ui_pages[UI_NUM_PAGES] = UI_PAGES;
static uint8_t nFilesOnCard;

const char *UIDisplay::my_cwd(void)
{
	if (cwd[0] == 0)
	{
		strncpy(cwd, platform->GetGCodeDir(), sizeof(cwd));

		// We use cwd_skip when we need to ignore the "0:gcodes/" prefix
		cwd_skip = strlen(cwd);
	}
	return cwd;
}

void UIDisplay::sdUpdateFileCount()
{
	FileInfo file_info;
	MassStorage *ms = platform->GetMassStorage();

	nFilesOnCard = 0;
	if (ms->FindFirst(my_cwd(), file_info))
	{
		do
		{
			if ((folderLevel >= SD_MAX_FOLDER_DEPTH) && file_info.isDirectory)
				continue;
			++nFilesOnCard;
		} while (ms->FindNext(file_info));
	}
}

void UIDisplay::sdGetFilenameAt(uint8_t filePos, char *filename)
{
	FileInfo file_info;
	MassStorage *ms = platform->GetMassStorage();

	if (ms->FindFirst(my_cwd(), file_info))
    {
		do
		{
			if (file_info.isDirectory &&
				(folderLevel >= SD_MAX_FOLDER_DEPTH) &&
				!(StringEquals(file_info.fileName, "..")))
				continue;
			if (filePos--)
				continue;
			strcpy(filename, file_info.fileName);
			if (file_info.isDirectory)
				strcat(filename, "/");
			break;
		} while (ms->FindNext(file_info));
	}
}

bool UIDisplay::sdIsDirname(const char *name)
{
	if (!name || !name[0]) return false;
    while (*name) name++;
    return name[-1] == '/';
}

void UIDisplay::sdGoDir(const char *name)
{
    if (StringEquals(name, ".."))
    {
		if (cwd[0] == 0)
			folderLevel = 0;
#if 0
		// should never happen
		if (StringEquals(cwd, "/"))
		{
			folderLevel = 0;
			cwd[0] = 0;
		}
#endif
        if (folderLevel == 0)
			return;

		// Move to the last character in cwd[]
		// We've already ensured that strlen(cwd) != 0
		char *p = cwd + strlen(cwd) - 1;

		// Last character should be a / but just in case
		if (*p == '/') --p;

		// Find the start of the current subdirectory name
        while (*p != '/') p--;

		// Now NUL terminate just past the /
        p[1] = '\0';

		// And note that we've moved up the directory tree
        folderLevel--;
    }
    else
    {
        if (folderLevel >= SD_MAX_FOLDER_DEPTH) return;
		strlcat(cwd, name, sizeof(cwd));
        folderLevel++;
    }
    sdUpdateFileCount();
}

void UIDisplay::sdPrintFile(const char *fname)
{
	if (!fname || !fname[0]) return;

	// We'll generate the command
    //
	//     M32 <relative-file-path>
	//
	// to print this SD file.  Note that our file path "cwd"
	// used with the MassStorage routines includes the "0:gcode/"
	// prefix.  We want to omit that and so we need to ensure
	// that "cwd_skip" is set up.  It should be since we have
	// a filename from the SD card, but just to be sure we
	// call my_cwd() here.
	my_cwd();

	buf.Put("M32 ", 4);
	buf.Put(cwd + cwd_skip);
	buf.Put(fname);

	// Once the LF goes in, we have a complete gcode command which
	// GCodes::Spin() can now pick up and run with

	buf.Put("\n", 1);
}

bool UIDisplay::sdDeleteFile(const char *fname)
{
	return platform->GetMassStorage()->Delete(cwd, fname);
}

void UIDisplay::sdRefresh(uint8_t &r, char cache[UI_ROWS][MAX_COLS+1])
{
	byte length, offset, skip;
	FileInfo file_info;
	MassStorage *ms = platform->GetMassStorage();

	if (!ms->FindFirst(my_cwd(), file_info))
		return;

	offset = menuTop[menuLevel];
	skip = (offset > 0) ? offset - 1 : 0;

    while ((r + offset) < (nFilesOnCard + 1) && (r < UI_ROWS))
    {
        // done if past last used entry
        // skip deleted entry and entries for . and  ..
        // only list subdirectories and files

		// Do not list folders if we are at maximum folder depth
		if ((folderLevel >= SD_MAX_FOLDER_DEPTH) &&
			file_info.isDirectory &&
			!(StringEquals(file_info.fileName, "..")))
			goto next;

		// Skip this file or folder if we are not interested in it
		if (skip > 0)
		{
			skip--;
			goto next;
		}

		col = 0;
		if ((r + offset) == menuPos[menuLevel])
			printCols[col++] = CHAR_SELECTOR;
		else
			printCols[col++] = ' ';

		// print file name with possible blank fill
		if (file_info.isDirectory)
			printCols[col++] = 6; // Prepend folder symbol
	    length = MIN((int)strlen(file_info.fileName), MAX_COLS - col);
		memcpy(printCols + col, file_info.fileName, length);
		col += length;
		printCols[col] = 0;
		strcpy(cache[r++], printCols);

	next:
		if (!(ms->FindNext(file_info)))
			break;
    }
}

// Refresh current menu page
void UIDisplay::refreshPage()
{
    uint8_t r;
    uint8_t mtype = 0;
    char cache[UI_ROWS][MAX_COLS+1];

    adjustMenuPos();

#if UI_AUTORETURN_TO_MENU_AFTER != 0
    // Reset timeout on menu back when user active on menu
    if (encoderLast != encoderStartScreen)
        ui_autoreturn_time = millis() + UI_AUTORETURN_TO_MENU_AFTER;
#endif

    encoderStartScreen = encoderLast;

    // Copy result into cache
    if (menuLevel == 0)
    {
        const UIMenu *men = ui_pages[menuPos[0]];
        uint8_t nr = men->numEntries;
        const UIMenuEntry * const *entries = men->entries;
        for (r = 0; (r < nr) && (r < UI_ROWS); r++)
        {
            UIMenuEntry *ent = entries[r];
            col = 0;
            parse(ent->text);
            strcpy(cache[r], printCols);
        }
    }
    else
    {
		UIMenu *men = (UIMenu *)menu[menuLevel];
        uint8_t nr = men->numEntries;
        mtype = men->menuType;
        uint8_t offset = menuTop[menuLevel];
        const UIMenuEntry * const *entries = men->entries;
        for (r = 0; (r + offset < nr) && (r < UI_ROWS); )
        {
            UIMenuEntry *ent = entries[r+offset];
            if (!ent->showEntry())
            {
                offset++;
                continue;
            }
            unsigned char entType = ent->menuType;
            unsigned int entAction = ent->action;
            col = 0;
            if ((entType >= 2) && (entType <= 4))
            {
                if (((r + offset) == menuPos[menuLevel]) && (activeAction != entAction))
                    printCols[col++] = CHAR_SELECTOR;
                else if(activeAction == entAction)
                    printCols[col++] = CHAR_SELECTED;
                else
                    printCols[col++] = ' ';
            }
            parse(ent->text);
            if (entType == 2)   // Draw submenu marker at the right side
            {
                while (col < (UI_COLS-1)) printCols[col++]=' ';
                if (col > UI_COLS)
                {
                    printCols[MIN(UI_COLS - 1, col)] = CHAR_RIGHT;
                }
                else
                    printCols[col] = CHAR_RIGHT; // Arrow right
                printCols[++col] = 0;
            }
            strcpy(cache[r], printCols);
            r++;
        }
    }

    if (mtype == 1)
    {
        sdRefresh(r, cache);
    }

    printCols[0] = 0;
    while (r < UI_ROWS)
        strcpy(cache[r++], printCols);

    // Compute transition
    uint8_t transition = 0; // 0 = display, 1 = up, 2 = down, 3 = left, 4 = right
#if UI_ANIMATION
    if ((menuLevel != oldMenuLevel) && reprap.GetMove()->AllMovesAreFinished())
    {
        if ((oldMenuLevel == 0) || (oldMenuLevel == -2))
            transition = 1;
        else if (menuLevel == 0)
            transition = 2;
        else if (menuLevel > oldMenuLevel)
            transition = 3;
        else
            transition = 4;
    }
#endif
    uint8_t loops = 1;
    uint8_t dt = 1, y;
    if ((transition == 1) || (transition == 2))
		loops = UI_ROWS;
    else if (transition > 2)
    {
        dt = (UI_COLS + UI_COLS - 1) / 16;
        loops = UI_COLS + 1 / dt;
    }

    uint8_t off0 = (shift <= 0) ? 0 : shift;
    uint8_t scroll = dt;
    uint8_t off[UI_ROWS];
    if (transition == 0)
    {
        for (y = 0; y < UI_ROWS; y++)
            strcpy(displayCache[y], cache[y]);
    }

    for (y = 0; y < UI_ROWS; y++)
    {
        uint8_t len = strlen(displayCache[y]);
        off[y] = (len > UI_COLS) ? MIN(len - UI_COLS, off0) : 0;
#if UI_ANIMATION
        if (transition == 3)
        {
            for (r = len; r < MAX_COLS; r++)
            {
                displayCache[y][r] = 32;
            }
            displayCache[y][MAX_COLS] = 0;
        }
        else if (transition == 4)
        {
            for (r = strlen(cache[y]); r < MAX_COLS; r++)
            {
                cache[y][r] = 32;
            }
            cache[y][MAX_COLS] = 0;
        }
#endif
    }
    for (uint8_t l = 0; l < loops; l++)
    {
        if (encoderLast != encoderStartScreen)
        {
            scroll = 200;
        }
        scroll += dt;
#if UI_DISPLAY_TYPE == 5
#define drawHProgressBar(x, y, width, height, progress)			\
		{ u8g_DrawFrame(&u8g, x, y, width, height);				\
		  int p = ceil(((width) - 2) * (progress)  / 100);	    \
		  u8g_DrawBox(&u8g, (x)+1, (y)+1, p, (height) - 2); }


#define drawVProgressBar(x, y, width, height, progress)					\
		{ u8g_DrawFrame(&u8g, x, y, width, height);						\
			int p = (height) - 1 - ceil(((height) - 2) * (progress) / 100); \
			u8g_DrawBox(&u8g, (x) + 1, (y) + p, (width) - 2, ((height) - p)); }

        //fan
        int fanPercent, sdPercent;
        char fanString[2];
        if ((menuLevel == 0) && (menuPos[0] == 0))
        {
            //ext1 and ext2 animation symbols
			// ????
			int heater = whichHeater(0, HEATERS);
			if ((heater != HEATERS) && (Heat::HS_active == heat->GetStatus(heater)))
                cache[0][0] = isAnimation() ? '\x08' : '\x09';
            else
                cache[0][0] = '\x0a'; //off
			// ????
			// Use the actual heater for the second extruder
            if ((reprap.GetExtrudersInUse() > 1) && (HEATERS != (heater = whichHeater(1, HEATERS))) &&
				(Heat::HS_active == heat->GetStatus(heater)))
                cache[1][0] = isAnimation() ? '\x08' : '\x09';
            else
                cache[1][0] = '\x0a'; //off

            // heat bed animated icons
			if (Heat::HS_active == heat->GetStatus(0))
                cache[2][0] = isAnimation() ? '\x0c' : '\x0d';
            else
                cache[2][0] = '\x0b';

            //fan
            fanPercent = (int)(100.0 * platform->GetFanValue(0));
            fanString[1] = 0;
            if (fanPercent > 0)  //fan running animation
            {
                fanString[0] = isAnimation() ? '\x0e' : '\x0f';
            }
            else
            {
                fanString[0] = '\x0e';
            }

            //SD Card
            if (reprap.GetPrintMonitor()->IsPrinting())
            {
				sdPercent = (int)(100.0 * reprap.GetGCodes()->FractionOfFilePrinted());
			}
			else
			{
				sdPercent = -1;
            }
        }

        //u8g picture loop
        u8g_FirstPage(&u8g);
        do
        {
#endif
            if (transition == 0)
            {
#if UI_DISPLAY_TYPE == 5
                if ((menuLevel == 0) && (menuPos[0] == 0))
                {
                    u8g_SetFont(&u8g, UI_FONT_SMALL);
                    uint8_t py = 8;
                    for (uint8_t r = 0; r < 3; r++)
                    {
                        if(u8g_IsBBXIntersection(&u8g, 0, py - UI_FONT_SMALL_HEIGHT, 1, UI_FONT_SMALL_HEIGHT))
                            printU8GRow(0, py, cache[r]);
                        py += 10;
                    }
                    //fan
                    if (u8g_IsBBXIntersection(&u8g, 0, 30 - UI_FONT_SMALL_HEIGHT, 1, UI_FONT_SMALL_HEIGHT))
                        printU8GRow(117, 30, fanString);
                    drawVProgressBar(116, 0, 9, 20, fanPercent);
                    if (u8g_IsBBXIntersection(&u8g, 0, 43 - UI_FONT_SMALL_HEIGHT, 1, UI_FONT_SMALL_HEIGHT))
                        printU8GRow(0, 43, cache[3]); //mul
                    if(u8g_IsBBXIntersection(&u8g, 0, 52 - UI_FONT_SMALL_HEIGHT, 1, UI_FONT_SMALL_HEIGHT))
                        printU8GRow(0, 52, cache[4]); //buf

                    //SD Card
                    if ((sdPercent >= 0) && u8g_IsBBXIntersection(&u8g, 70, 48 - UI_FONT_SMALL_HEIGHT, 1,
																  UI_FONT_SMALL_HEIGHT))
                    {
                        printU8GRow(70, 48, "SD");
                        drawHProgressBar(83, 42, 40, 5, sdPercent);
                    }

                    //Status
                    py = u8g_GetHeight(&u8g) - 2;
                    if (u8g_IsBBXIntersection(&u8g, 70, py - UI_FONT_SMALL_HEIGHT, 1, UI_FONT_SMALL_HEIGHT))
                        printU8GRow(0, py, cache[5]);

                    //divider lines
                    u8g_DrawHLine(&u8g,0, 32, u8g_GetWidth(&u8g));
                    if ( u8g_IsBBXIntersection(&u8g, 55, 0, 1, 32) )
                    {
                        u8g_draw_vline(&u8g, 112, 0, 32);
                        u8g_draw_vline(&u8g, 62, 0, 32);
                    }
                    u8g_SetFont(&u8g, UI_FONT_DEFAULT);
                }
                else
                {
#endif
                    for (y = 0; y < UI_ROWS; y++)
                        printRow(y, &cache[y][off[y]], NULL, UI_COLS);
#if UI_DISPLAY_TYPE == 5
                }
#endif
            }
#if UI_ANIMATION
            else
            {
                if (transition == 1)   // up
                {
                    if (scroll > UI_ROWS)
                    {
                        scroll = UI_ROWS;
                        l = loops;
                    }
                    for (y = 0; y < UI_ROWS - scroll; y++)
                    {
                        r = y + scroll;
                        printRow(y, &displayCache[r][off[r]], NULL, UI_COLS);
                    }
                    for (y=0; y < scroll; y++)
                    {
                        printRow(UI_ROWS - scroll + y, cache[y], NULL, UI_COLS);
                    }
                }
                else if (transition == 2)     // down
                {
                    if (scroll > UI_ROWS)
                    {
                        scroll = UI_ROWS;
                        l = loops;
                    }
                    for (y = 0; y < scroll; y++)
                    {
                        printRow(y, cache[UI_ROWS-scroll+y], NULL, UI_COLS);
                    }
                    for (y = 0; y < UI_ROWS - scroll; y++)
                    {
                        r = y + scroll;
                        printRow(y + scroll, &displayCache[y][off[y]], NULL, UI_COLS);
                    }
                }
                else if (transition == 3)     // left
                {
                    if (scroll > UI_COLS)
                    {
                        scroll = UI_COLS;
                        l = loops;
                    }
                    for (y = 0; y < UI_ROWS; y++)
                    {
                        printRow(y, &displayCache[y][off[y]+scroll], cache[y], UI_COLS - scroll);
                    }
                }
                else     // right
                {
                    if (scroll > UI_COLS)
                    {
                        scroll = UI_COLS;
                        l = loops;
                    }
                    for (y = 0; y < UI_ROWS; y++)
                    {
                        printRow(y, cache[y] + UI_COLS - scroll, &displayCache[y][off[y]], scroll);
                    }
                }
#if DISPLAY_TYPE != 5
                delay(transition < 3 ? 200 : 70);
#endif
                HAL::pingWatchdog();
            }
#endif // UI_ANIMATION
#if UI_DISPLAY_TYPE == 5
        }
        while (u8g_NextPage(&u8g));  //end picture loop
		toggleAnimation();
#endif
    } // for l
#if UI_ANIMATION
    // copy to last cache
    if (transition != 0)
        for (y = 0; y < UI_ROWS; y++)
            strcpy(displayCache[y], cache[y]);
    oldMenuLevel = menuLevel;
#endif
}

void UIDisplay::pushMenu(void *men, bool refresh)
{
    if (men == menu[menuLevel])
    {
        refreshPage();
        return;
    }

    if (menuLevel == 4)
		return;

    menuLevel++;
    menu[menuLevel] = men;
    menuTop[menuLevel] = menuPos[menuLevel] = 0;

    UIMenu *men2 = (UIMenu *)menu[menuLevel];
    if (men2->menuType == 1) // Open files list
        sdUpdateFileCount();

    if (refresh)
        refreshPage();
}

void UIDisplay::okAction()
{
	// ????
    if (IS_UI_ERROR_MESSAGE()) {
        SET_UI_ERROR_MESSAGE(false);
        return;
    }

#if UI_HAS_KEYS == 1
    if (menuLevel == 0)   // Enter menu
    {
        menuLevel = 1;
        menuTop[1] = menuPos[1] = 0;
        menu[1] = (void *)&ui_menu_main;
		// ????
        beep(BEEPER_SHORT_SEQUENCE);
        return;
    }

    UIMenu *men = (UIMenu *)menu[menuLevel];
    //uint8_t nr = menu->numEntries;
    uint8_t mtype = men->menuType;
    const UIMenuEntry * const *entries = men->entries;
    UIMenuEntry *ent = entries[menuPos[menuLevel]];
	unsigned char entType = ent->menuType; // 0 = Info, 1 = Headline, 2 = submenu ref, 3 = direct action command, 4 = modify action
    int action = ent->action;
    if (mtype == 3)   // action menu
    {
        action = men->id;
        finishAction(action);
        executeAction(UI_ACTION_BACK);
        return;
    }

    if ((mtype == 2) && (entType == 4))   // Modify action
    {
        if (activeAction)   // finish action
        {
            finishAction(action);
            activeAction = 0;
        }
        else
            activeAction = action;
        return;
    }

    if (mtype == 1)
    {
        if (menuPos[menuLevel] == 0)   // Selected back instead of file
        {
            executeAction(UI_ACTION_BACK);
            return;
        }

        if (!reprap.GetPrintMonitor()->IsPrinting())
            return;

        uint8_t filePos = menuPos[menuLevel] - 1;
        char filename[LONG_FILENAME_LENGTH + 1];

        sdGetFilenameAt(filePos, filename);
        if (sdIsDirname(filename))   // Directory change selected
        {
            sdGoDir(filename);
            menuTop[menuLevel] = 0;
            menuPos[menuLevel] = 1;
            refreshPage();
#if UI_ANIMATION
            oldMenuLevel = -1;
#endif
            return;
        }

        int16_t action;
        if (menuMode & MENU_MODE_AUTOMOUNT)
            action = UI_ACTION_SD_PRINT;
        else
        {
            men = (UIMenu *)menu[menuLevel - 1];
            entries = men->entries;
            ent = entries[menuPos[menuLevel - 1]];
            action = ent->action;
        }

        switch(action)
        {
		default :
        case UI_ACTION_SD_PRINT:
			sdPrintFile(filename);
			beep(BEEPER_LONG_SEQUENCE);
			menuLevel = 0;
            break;

        case UI_ACTION_SD_DELETE:
            if (sdDeleteFile(filename))
            {
				// ????
				// refresh
				beep(BEEPER_LONG_SEQUENCE);
            }
            break;
        }
        return;
    }

    if (entType == 2)   // Enter submenu
    {
        pushMenu((void *)action, false);
        beep(BEEPER_SHORT_SEQUENCE);
        return;
    }

    if (entType == 3)
    {
        executeAction(action);
        return;
    }
	// Default case?
    executeAction(UI_ACTION_BACK);
#endif  // UI_HAS_KEYS == 1
}

void UIDisplay::adjustMenuPos()
{
    if (menuLevel == 0)
		return;

    UIMenu *men = (UIMenu *)menu[menuLevel];
    const UIMenuEntry * const *entries = men->entries;
    uint8_t mtype = men->menuType;
    if (mtype != 2)
		return;

    while (menuPos[menuLevel] > 0)
    {
        if (entries[menuPos[menuLevel]]->showEntry())
            break;
        menuPos[menuLevel]--;
    }

    uint8_t skipped = 0;
    bool modified;
    if (menuTop[menuLevel] > menuPos[menuLevel])
        menuTop[menuLevel] = menuPos[menuLevel];
    do
    {
        skipped = 0;
        modified = false;
        for (uint8_t r = menuTop[menuLevel]; r < menuPos[menuLevel]; r++)
        {
            UIMenuEntry *ent = entries[r];
            if (!ent->showEntry())
                skipped++;
        }
        if ((menuTop[menuLevel] + skipped + UI_ROWS - 1) < menuPos[menuLevel])
		{
            menuTop[menuLevel] = menuPos[menuLevel] + 1 - UI_ROWS;
            modified = true;
        }
    }
    while (modified);
}

void UIDisplay::nextPreviousAction(int8_t next)
{
	// ????
    if (IS_UI_ERROR_MESSAGE()) {
        SET_UI_ERROR_MESSAGE(false);
        return;
    }

    millis_t actTime = millis();
    millis_t dtReal;
    millis_t dt = dtReal = actTime - lastNextPrev;
    lastNextPrev = actTime;
    if (dt < SPEED_MAX_MILLIS) dt = SPEED_MAX_MILLIS;
    if (dt > SPEED_MIN_MILLIS)
    {
        dt = SPEED_MIN_MILLIS;
        lastNextAccumul = 1;
    }

    float f = (float)(SPEED_MIN_MILLIS - dt) /
		(float)(SPEED_MIN_MILLIS-SPEED_MAX_MILLIS);
    lastNextAccumul = 1.0f + 100.0*f*f*f;

#if UI_HAS_KEYS == 1
    if (menuLevel == 0)
    {
        lastSwitch = millis();
        if (((UI_INVERT_MENU_DIRECTION) && (next < 0)) ||
			(!(UI_INVERT_MENU_DIRECTION) && (next > 0)))
        {
            menuPos[0]++;
            if (menuPos[0] >= UI_NUM_PAGES)
                menuPos[0] = 0;
        }
        else
        {
            menuPos[0] = (menuPos[0] == 0) ? UI_NUM_PAGES - 1 : menuPos[0] - 1;
        }
        return;
    }
    UIMenu *men = (UIMenu *)menu[menuLevel];
    uint8_t nr = men->numEntries;
    uint8_t mtype = men->menuType;
    const UIMenuEntry * const *entries = men->entries;
    UIMenuEntry *ent = entries[menuPos[menuLevel]];
    UIMenuEntry *testEnt;
    int action = ent->action;
    if ((mtype == 2) && (activeAction == 0))   // browse through menu items
    {
        if (((UI_INVERT_MENU_DIRECTION) && (next < 0)) ||
			(!(UI_INVERT_MENU_DIRECTION) && (next > 0)))
        {
            while ((menuPos[menuLevel] + 1) < nr)
            {
                menuPos[menuLevel]++;
                testEnt = entries[menuPos[menuLevel]];
                if (testEnt->showEntry())
                    break;
            }
        }
        else if (menuPos[menuLevel] > 0)
        {
            while (menuPos[menuLevel] > 0)
            {
                menuPos[menuLevel]--;
                testEnt = entries[menuPos[menuLevel]];
                if (testEnt->showEntry())
                    break;
            }
        }
        adjustMenuPos();
        return;
    }

    if (mtype == 1)   // SD listing
    {
        if (((UI_INVERT_MENU_DIRECTION) && (next < 0)) ||
			(!(UI_INVERT_MENU_DIRECTION) && (next > 0)))
        {
            if (menuPos[menuLevel] < nFilesOnCard)
				menuPos[menuLevel]++;
        }
        else if (menuPos[menuLevel] > 0)
            menuPos[menuLevel]--;
        if (menuTop[menuLevel] > menuPos[menuLevel])
            menuTop[menuLevel] = menuPos[menuLevel];
        else if ((menuTop[menuLevel] + UI_ROWS - 1) < menuPos[menuLevel])
            menuTop[menuLevel] = menuPos[menuLevel] + 1 - UI_ROWS;
        return;
    }

    if (mtype == 3)
		action = men->id;
    else
		action = activeAction;

    int8_t increment = next;
    switch (action)
    {
    case UI_ACTION_FANSPEED:
	{
#if COOLING_FAN0_PIN > -1
		float fv = platform->GetFanValue(0) + (float)(increment * 3)/256.0;
		if (fv > 1.0) fv = 1.0;
		else if (fv < 0.0) fv = 0.0;
		platform->SetFanValue(0, fv);
#endif
        break;
	}

    case UI_ACTION_XPOSITION:
		jog(0, (float)increment);
		break;

    case UI_ACTION_YPOSITION:
		jog(1, (float)increment);
		break;

    case UI_ACTION_ZPOSITION:
		jog(2, (float)increment);
		break;

    case UI_ACTION_EPOSITION:
		jog(3, (float)increment);
        break;

    case UI_ACTION_HEATED_BED_TEMP:
    {
        int tmp = (int)(heat->GetActiveTemperature(BED_HEATER));
        if (tmp < 0) tmp = 0;
        tmp += increment;
        if (tmp < 0) tmp = 0;
        else if (tmp > 140) tmp = 140;
		heat->SetActiveTemperature(BED_HEATER, (float)tmp);
    }
    break;

    case UI_ACTION_EXTRUDER0_TEMP:
    {
		Tool *tool = reprap.GetTool(0);
		if (tool && (tool->HeaterCount() > 0))
		{
			adjustTemp((int8_t)(0x7f & tool->Heater(0)), increment, 300);
		}
		break;
    }

    case UI_ACTION_EXTRUDER1_TEMP:
    {
		Tool *tool = reprap.GetTool(1);
		if (tool && (tool->HeaterCount() > 0))
		{
			adjustTemp((int8_t)(0x7f & tool->Heater(0)), increment, 300);
		}
		break;
    }

    case UI_ACTION_FEEDRATE_MULTIPLY:
    {
        int fr = (int)(reprap.GetGCodes()->GetSpeedFactor() * 100.0);
        INCREMENT_MIN_MAX(fr, 1, 25, 500);
		reprap.GetGCodes()->SetSpeedFactor((float)fr / 100.0);
    }
    break;

    case UI_ACTION_FLOWRATE_MULTIPLY:
    {
		size_t extruder = 0;
		Tool *t = reprap.GetCurrentTool();
		if (t)
		{
			extruder = (size_t)t->Number();
			if (extruder >= (DRIVES - AXES)) extruder = 0;
		}
		int fr = (int)(reprap.GetGCodes()->GetExtrusionFactor(extruder) * 100.0);
        INCREMENT_MIN_MAX(fr, 1, 25, 200);
		reprap.GetGCodes()->SetExtrusionFactor(extruder, (float)fr / 100.0);
    }
    break;

    case UI_ACTION_ACCEL_X:
		changeMaxAccel(0, increment);
		break;

    case UI_ACTION_ACCEL_Y:
		changeMaxAccel(1, increment);
        break;

    case UI_ACTION_ACCEL_Z:
		changeMaxAccel(2, increment);
        break;

    case UI_ACTION_ACCEL_E:
		changeMaxAccel(AXES, increment);
        break;

    case UI_ACTION_INSTANT_DV_X:
		changeInstantDv(0, increment);
        break;

    case UI_ACTION_INSTANT_DV_Y:
		changeInstantDv(1, increment);
        break;

    case UI_ACTION_INSTANT_DV_Z:
		changeInstantDv(2, increment);
        break;

    case UI_ACTION_MAX_FEEDRATE_X:
		changeMaxFeedrate(0, increment);
        break;

    case UI_ACTION_MAX_FEEDRATE_Y:
		changeMaxFeedrate(1, increment);
        break;

    case UI_ACTION_MAX_FEEDRATE_Z:
		changeMaxFeedrate(2, increment);
        break;

    case UI_ACTION_MAX_FEEDRATE_E:
        changeMaxFeedrate(AXES, increment);
        break;

    case UI_ACTION_STEPS_X:
		changeStepsPerMm(0, increment);
		break;

    case UI_ACTION_STEPS_Y:
		changeStepsPerMm(1, increment);
        break;

    case UI_ACTION_STEPS_Z:
		changeStepsPerMm(2, increment);
        break;

    case UI_ACTION_STEPS_E:
        changeStepsPerMm(AXES, increment);
        break;

    case UI_ACTION_BAUDRATE:
    {
		// Operate on the primary (USB) serial and not the auxilliary
		uint32_t baudrate = platform->GetBaudRate(0);
        uint32_t rate;
        int p = 0;
        do
        {
            rate = baudrates[p];
            if (rate == baudrate) break;
            p++;
        }
        while (rate != 0);
        if (rate == 0) p -= 2;
        p += increment;
        if (p < 0) p = 0;
        rate = baudrates[p];
        if (rate == 0) p--;
        rate = baudrates[p];
		if (rate != baudrate) platform->SetBaudRate(0, rate);
    }
    break;

    case UI_ACTION_PID_PGAIN:
	{
		// ???? Handle negative P
		size_t heater = currentHeater(1);
		PidParameters pp = platform->GetPidParameters(heater);
		float kP0 = pp.kP;
		INCREMENT_MIN_MAX(pp.kP, 0.1, 0.0, 200.0);
		if (kP0 != pp.kP)
			platform->SetPidParameters(heater, pp);
	}
	break;

    case UI_ACTION_PID_IGAIN:
	{
		size_t heater = currentHeater(1);
		PidParameters pp = platform->GetPidParameters(heater);
		pp.kI *= platform->HeatSampleTime();
		float kI0 = pp.kI;
		INCREMENT_MIN_MAX(pp.kI, 0.01, 0.0, 100.0);
		if (kI0 != pp.kI)
		{
			pp.kI /= platform->HeatSampleTime();
			platform->SetPidParameters(heater, pp);
		}
	}
	break;

    case UI_ACTION_PID_DGAIN:
	{
		size_t heater = currentHeater(1);
		PidParameters pp = platform->GetPidParameters(heater);
		pp.kD /= platform->HeatSampleTime();
		float kD0 = pp.kD;
			INCREMENT_MIN_MAX(pp.kD, 0.01, 0.0, 200.0);
		if (kD0 != pp.kD)
		{
			pp.kD *= platform->HeatSampleTime();
			platform->SetPidParameters(heater, pp);

		}
	}
	break;

    case UI_ACTION_PID_MIN:
	{
		size_t heater = currentHeater(1);
		PidParameters pp = platform->GetPidParameters(heater);
		float pidMin0 = pp.pidMin;
		INCREMENT_MIN_MAX(pp.pidMin, 1.0, 0.0, 255.0);
		if (pidMin0 != pp.pidMin)
			platform->SetPidParameters(heater, pp);
	}
	break;

    case UI_ACTION_PID_MAX:
	{
		size_t heater = currentHeater(1);
		PidParameters pp = platform->GetPidParameters(heater);
		float pidMax0 = pp.pidMax;
		INCREMENT_MIN_MAX(pp.pidMax, 1.0, 0.0, 255.0);
		if (pidMax0 != pp.pidMax)
			platform->SetPidParameters(heater, pp);
	}
	break;

    case UI_ACTION_X_OFFSET:
	{
		if (reprap.GetCurrentTool() != NULL)
		{
			const float *offsets0 = reprap.GetCurrentTool()->GetOffset();
			if (offsets0)
			{
				float offsets[AXES];
				memcpy(offsets, offsets0, sizeof(offsets));
				INCREMENT_MIN_MAX(offsets[0], 0.1, -200.0, 200.0);
				reprap.GetCurrentTool()->SetOffset(offsets);
			}
		}
        break;
	}

    case UI_ACTION_Y_OFFSET:
	{
		if (reprap.GetCurrentTool() != NULL)
		{
			const float *offsets0 = reprap.GetCurrentTool()->GetOffset();
			if (offsets0)
			{
				float offsets[AXES];
				memcpy(offsets, offsets0, sizeof(offsets));
				INCREMENT_MIN_MAX(offsets[1], 0.1, -200.0, 200.0);
				reprap.GetCurrentTool()->SetOffset(offsets);
			}
		}
        break;
	}

    case UI_ACTION_ADVANCE_K:
	{
		size_t drive = AXES;
		if (!whichDriveReally(&drive))
		{
			float ec = platform->GetElasticComp(drive);
			INCREMENT_MIN_MAX(ec, 0.01, 0.0, 10.0);
			platform->SetElasticComp(drive, ec);
		}
        break;
	}
    }

#if UI_AUTORETURN_TO_MENU_AFTER != 0
    ui_autoreturn_time = millis() + UI_AUTORETURN_TO_MENU_AFTER;
#endif
#endif
}

void UIDisplay::finishAction(int action)
{
}

// Actions are events from user input. Depending on the current state, each
// action can behave differently. Other actions do always the same like home, disable extruder etc.
void UIDisplay::executeAction(int action)
{
#if UI_HAS_KEYS == 1
    bool skipBeep = false;
    if (action & UI_ACTION_TOPMENU)   // Go to start menu
    {
        action -= UI_ACTION_TOPMENU;
        menuLevel = 0;
    }
    if ((action >= 2000) && (action < 3000))
    {
        setStatus(ui_action);
    }
    else
	{
        switch(action)
        {
        case UI_ACTION_OK:
            okAction();
            skipBeep = true; // Prevent double beep
            break;

        case UI_ACTION_BACK:
            if (menuLevel > 0) menuLevel--;
            setMode(MENU_MODE_AUTOMOUNT, false);
            activeAction = 0;
            break;

        case UI_ACTION_NEXT:
            nextPreviousAction(1);
            break;

        case UI_ACTION_PREVIOUS:
            nextPreviousAction(-1);
            break;

        case UI_ACTION_MENU_UP:
            if (menuLevel > 0) menuLevel--;
            break;

        case UI_ACTION_TOP_MENU:
            menuLevel = 0;
            break;

        case UI_ACTION_EMERGENCY_STOP:
			reprap.EmergencyStop();
            break;

        case UI_ACTION_HOME_ALL:
			buf.Put("M28\n", 4);
            break;

        case UI_ACTION_HOME_X:
			buf.Put("M28 X\n", 6);
            break;

        case UI_ACTION_HOME_Y:
			buf.Put("M28 Y\n", 6);
            break;

        case UI_ACTION_HOME_Z:
			buf.Put("M28 Z\n", 6);
            break;

        case UI_ACTION_SET_ORIGIN:
			buf.Put("G92 X0 Y0 Z0\n", 13);
            break;

#if ATX_POWER_PIN_D >= 0
        case UI_ACTION_POWER:
			buf.Put("M81\n", 4);
            break;
#endif

        case UI_ACTION_PREHEAT:
			heat->Standby(BED_HEATER);
            break;

        case UI_ACTION_COOLDOWN:
			heat->SwitchOffAll();
            break;

        case UI_ACTION_HEATED_BED_OFF:
			heat->SwitchOff(BED_HEATER);
            break;

        case UI_ACTION_EXTRUDER0_OFF:
			switchToolOff(0);
			break;

        case UI_ACTION_EXTRUDER1_OFF:
			switchToolOff(1);
            break;

        case UI_ACTION_EXTRUDER2_OFF:
			switchToolOff(2);
            break;

        case UI_ACTION_DISABLE_STEPPER:
			// Run this as a M18 so that proper state can be set in GCodes (e.g., axes not homed, etc.)
			buf.Put("M18\n", 4);
            break;

        case UI_ACTION_RESET_EXTRUDER:
			buf.Put("G92 E0\n", 7);
            break;

        case UI_ACTION_EXTRUDER_RELATIVE:
			buf.Put("M83\n", 4);
            break;

        case UI_ACTION_SELECT_EXTRUDER0:
			// Do not attempt to select if it doesn't exist
			// Selecting a non-existent tool will put the current tool on standby
			if (reprap.GetTool(0)) reprap.SelectTool(0);
            break;

        case UI_ACTION_SELECT_EXTRUDER1:
			// Do not attempt to select if it doesn't exist
			// Selecting a non-existent tool will put the current tool on standby
			if (reprap.GetTool(1)) reprap.SelectTool(1);
            break;

        case UI_ACTION_SELECT_EXTRUDER2:
			// Do not attempt to select if it doesn't exist
			// Selecting a non-existent tool will put the current tool on standby
			if (reprap.GetTool(2)) reprap.SelectTool(2);
            break;

        case UI_ACTION_SD_DELETE:
			// Need to know we have an SD card
			if (sdPresent())
            {
                pushMenu((void *)&ui_menu_sd_fileselector, false);
            }
            else
            {
                UI_ERROR(UI_TEXT_NOSDCARD);
            }
            break;

        case UI_ACTION_SD_PRINT:
            if (sdPresent())
            {
                pushMenu((void *)&ui_menu_sd_fileselector, false);
            }
            break;

        case UI_ACTION_SD_PAUSE:
			buf.Put("M25\n", 4);
            break;

        case UI_ACTION_SD_CONTINUE:
			buf.Put("M24\n", 4);
            break;

        case UI_ACTION_SD_STOP:
            buf.Put("M0\n", 3);
            break;

        case UI_ACTION_MENU_SDCARD:
            pushMenu((void *)&ui_menu_sd, false);
            break;

        case UI_ACTION_FAN_OFF:
#if COOLING_FAN0_PIN > -1
			platform->SetFanValue(0, 0.00);
#endif
            break;

        case UI_ACTION_FAN_25:
#if COOLING_FAN0_PIN > -1
			platform->SetFanValue(0, 0.25);
#endif
            break;

        case UI_ACTION_FAN_50:
#if COOLING_FAN0_PIN > -1
			platform->SetFanValue(0, 0.50);
#endif
            break;

        case UI_ACTION_FAN_75:
#if COOLING_FAN0_PIN > -1
			platform->SetFanValue(0, 0.75);
#endif
            break;

        case UI_ACTION_FAN_FULL:
#if COOLING_FAN0_PIN > -1
			platform->SetFanValue(0, 1.00);
#endif
            break;

        case UI_ACTION_MENU_XPOS:
            pushMenu((void *)&ui_menu_xpos, false);
            break;

        case UI_ACTION_MENU_YPOS:
            pushMenu((void *)&ui_menu_ypos, false);
            break;

        case UI_ACTION_MENU_ZPOS:
            pushMenu((void *)&ui_menu_zpos, false);
            break;

        case UI_ACTION_MENU_QUICKSETTINGS:
            pushMenu((void *)&ui_menu_quick, false);
            break;

        case UI_ACTION_MENU_EXTRUDER:
            pushMenu((void *)&ui_menu_extruder, false);
            break;

        case UI_ACTION_MENU_POSITIONS:
            pushMenu((void *)&ui_menu_positions, false);
            break;

        case UI_ACTION_X_UP:
			jog(0, 1.0);
            break;

        case UI_ACTION_X_DOWN:
			jog(0, -1.0);
            break;

        case UI_ACTION_Y_UP:
			jog(1, 1.0);
            break;

        case UI_ACTION_Y_DOWN:
			jog(1, -1.0);
            break;

        case UI_ACTION_Z_UP:
			jog(2, 1.0);
            break;

        case UI_ACTION_Z_DOWN:
			jog(2, -1.0);
            break;

        case UI_ACTION_EXTRUDER_UP:
			jog(3, 1.0);
            break;

        case UI_ACTION_EXTRUDER_DOWN:
			jog(3, -1.0);
            break;

        case UI_ACTION_EXTRUDER_TEMP_UP:
			adjustTemp((int8_t)(0x7f & currentHeater(1)), 1, 300);
			break;

        case UI_ACTION_EXTRUDER_TEMP_DOWN:
			adjustTemp((int8_t)(0x7f & currentHeater(1)), -1, 300);
			break;

        case UI_ACTION_HEATED_BED_UP:
			adjustTemp(BED_HEATER, 1, 140);
			break;

        case UI_ACTION_HEATED_BED_DOWN:
			adjustTemp(BED_HEATER, -1, 140);
			break;

        case UI_ACTION_FAN_UP:
#if COOLING_FAN0_PIN > -1
		{
			float fv = platform->GetFanValue(0) + 0.1;
			if (fv > 1.0) fv = 1.0;
			platform->SetFanValue(0, fv);
		}
#endif
		break;

        case UI_ACTION_FAN_DOWN:
#if COOLING_FAN0_PIN > -1
		{
			float fv = platform->GetFanValue(0) - 0.1;
			if (fv < 0.0) fv = 0.0;
			platform->SetFanValue(0, fv);
		}
#endif
		break;

        case UI_ACTION_KILL:
			reprap.EmergencyStop();
            break;

        case UI_ACTION_RESET:
			platform->SoftwareReset(0);  // user requested reset
            break;

        case UI_ACTION_PAUSE:
			buf.Put("M1\n", 3);
            break;

        }
	}

    refreshPage();

    if (!skipBeep)
        beep(BEEPER_SHORT_SEQUENCE);
#if UI_AUTORETURN_TO_MENU_AFTER != 0
	ui_autoreturn_time = millis() + UI_AUTORETURN_TO_MENU_AFTER;
#endif
#endif
}

void UIDisplay::mediumAction()
{
#if UI_HAS_I2C_ENCODER > 0
    checkSlowEncoder();
#endif
}

void UIDisplay::slowAction()
{
    unsigned long time = millis();
    uint8_t refresh = 0;
#if UI_HAS_KEYS == 1
    // Update key buffer
	forbidInterrupts();
    if ((flags & 9) == 0)
    {
        flags |= 8;
		allowInterrupts();
#if defined(UI_I2C_HOTEND_LED) || defined(UI_I2C_HEATBED_LED) || defined(UI_I2C_FAN_LED)
        {
            // check temps and set appropriate leds
			int heater;
			Heat::HeaterStatus hs;
            int led = 0;
#if defined(UI_I2C_HOTEND_LED)
			if ((heater = whichHeater(0, -1)) >= 0)
			{
				hs = heat->GetStatus(heater);
				if ((hs == HS_active) || (hs == HS_standby)) led |= UI_I2C_HOTEND_LED;
			}
#endif
#if defined(UI_I2C_HEATBED_LED)
			hs = heat->GetStatus(BED_HEATER);
			if ((hs == HS_active) || (hs == HS_standby)) led |= UI_I2C_HEATBED_LED;
#endif
#if COOLING_FAN0_PIN > -1 && defined(UI_I2C_FAN_LED)
			{
				float fv = platform->GetFanValue(0);
				if (fv > 0.0) led |= UI_I2C_FAN_LED;
			}
#endif
            // update the leds
            outputMask = ~led & (UI_I2C_HEATBED_LED | UI_I2C_HOTEND_LED | UI_I2C_FAN_LED);
        }
#endif
        int nextAction = 0;
        checkSlowKeys(nextAction);
        if (lastButtonAction != nextAction)
        {
            lastButtonStart = time;
            lastButtonAction = nextAction;
			forbidInterrupts();
            flags |= 2; // Mark slow action
        }
		forbidInterrupts();
        flags -= 8;
    }
	forbidInterrupts();
    if ((flags & 4) == 0)
    {
        flags |= 4;
        // Reset click encoder
		forbidInterrupts();
        int8_t epos = encoderPos;
        encoderPos = 0;
		allowInterrupts();
        if (epos)
        {
            nextPreviousAction(epos);
            beep(BEEPER_SHORT_SEQUENCE);
            refresh = 1;
        }
        if (lastAction != lastButtonAction)
        {
            if (lastButtonAction == 0)
            {
                if ((lastAction >= 2000) && (lastAction < 3000))
                {
                    statusMsg[0] = 0;
                }
                lastAction = 0;
				forbidInterrupts();
                flags &= ~3;
            }
            else if (time-lastButtonStart > UI_KEY_BOUNCETIME)     // New key pressed
            {
                lastAction = lastButtonAction;
                executeAction(lastAction);
                nextRepeat = time + UI_KEY_FIRST_REPEAT;
                repeatDuration = UI_KEY_FIRST_REPEAT;
            }
        }
        else if ((lastAction < 1000) && lastAction)     // Repeatable key
        {
            if ((time - nextRepeat) < 10000)
            {
                executeAction(lastAction);
                repeatDuration -= UI_KEY_REDUCE_REPEAT;
                if (repeatDuration < UI_KEY_MIN_REPEAT) repeatDuration = UI_KEY_MIN_REPEAT;
                nextRepeat = time + repeatDuration;
            }
        }
		forbidInterrupts();
        flags -= 4;
    }
	allowInterrupts();
#endif
#if UI_AUTORETURN_TO_MENU_AFTER != 0
    if ((menuLevel > 0) && (ui_autoreturn_time < time))
    {
        lastSwitch = time;
        menuLevel = 0;
        activeAction = 0;
    }
#endif
    if ((menuLevel == 0) && (time > 4000))
    {
        if ((time - lastSwitch) > UI_PAGES_DURATION)
        {
            lastSwitch = time;
#if !defined(UI_DISABLE_AUTO_PAGESWITCH) || !UI_DISABLE_AUTO_PAGESWITCH
            menuPos[0]++;
            if(menuPos[0] >= UI_NUM_PAGES)
                menuPos[0] = 0;
#endif
            refresh = 1;
        }
        else if ((time - lastRefresh) >= 1000) refresh = 1;
    }
    else if ((time - lastRefresh) >= 800)
    {
        refresh = 1;
    }
    if (refresh)
    {
        if ((menuLevel > 1) || (menuMode & MENU_MODE_AUTOMOUNT))
        {
            shift++;
            if ((shift + UI_COLS) > (MAX_COLS + 1))
                shift = -2;
        }
        else
            shift = -2;

        refreshPage();
        lastRefresh = time;
    }
}

void UIDisplay::fastAction()
{
#if UI_HAS_KEYS == 1
    // Check keys
	forbidInterrupts();
    if ((flags & 10) == 0)
    {
        flags |= 8;
		allowInterrupts();
        int nextAction = 0;
        checkKeys(nextAction);
        if (lastButtonAction != nextAction)
        {
            lastButtonStart = millis();
            lastButtonAction = nextAction;
			forbidInterrupts();
            flags |= 1;
        }
		forbidInterrupts();
        flags -= 8;
    }
	allowInterrupts();
#endif
}

void UIDisplay::Spin()
{
	if (!active)
		return;
	slowAction();
}

void UIDisplay::jog(size_t axis, float distance)
{
	char cmd[32];
	size_t drive;
	int feedrate;
	static const char axes[] = "XYZE";

	drive = axis;
	if (drive >= 3)
	{
		whichDriveReally(&drive);
		axis = 3; // use 'E' which moves the current tool
	}

	// Use 1/3 the maximum feedrate for the drive
	// Also, multiply by 60 seconds/minute
	feedrate = (int)(platform->MaxFeedrate(drive) * 20.0);

	// M120; save state
	// G91; relative coordinates
	buf.Put("M120\nG91\n", 9);

	// G1 S1 <axis>ddd.dd Fnnnn
	// Move at 1/3 max feedrate but converted to mm/minute
	snprintf(cmd, sizeof(cmd), "G1 %c%.2f F%d S1\n", axes[axis], distance, feedrate);
	buf.Put(cmd);

	// M121; restore state
	buf.Put("M121\n", 5);

	// We've used about 9 + 20 + 5 = 34 bytes
	// That's assuming a fast feedrate requiring 6 decimal digits (1666.66 mm.s)
}

static int whichHeater(int toolnum, int def)
{
	Tool *tool = reprap.GetTool(toolnum);
	if ((tool != NULL) && tool->HeaterCount() > 0)
		return tool->Heater(0);
	else 
		return def;
	
}

static size_t currentHeater(size_t def)
{
	Tool *tool = reprap.GetCurrentTool();
	if ((tool != NULL) && (tool->HeaterCount() > 0))
		return (size_t)tool->Heater(0);
	else
		return def;
}

static bool whichDriveReally(size_t *drive)
{
	if (*drive < AXES)
		// Drive is X, Y, or Z
		return true;

	// Currently selected tool?
	Tool *tool = reprap.GetCurrentTool();
	if ((tool == NULL) || (tool->DriveCount() <= 0))
	{
		*drive = AXES; // default to Tool 0 with drive AXES (3)
		return true;
	}

	// Use the first drive for the current tool
	*drive = (size_t)(tool->Drive(0) + AXES);
	return false;
}

void UIDisplay::changeMaxAccel(size_t drive, int8_t increment)
{
	bool justDoIt = whichDriveReally(&drive);
	int accel = (int)platform->Acceleration(drive);

	INCREMENT_MIN_MAX(accel, 100, 0, 10000);

	if (justDoIt)
	{
		platform->SetAcceleration(drive, (float)accel);
	}
	else
	{
		Tool *tool = reprap.GetCurrentTool();
		if (tool != NULL) // shouldn't happen owing to check in whichDriveReally()
		{
			for (int i = 0; i < (int)tool->DriveCount(); i++)
			{
				platform->SetAcceleration((size_t)(tool->Drive(i) + AXES), (float)accel);
			}
		}
	}
}

void UIDisplay::changeMaxFeedrate(size_t drive, int8_t increment)
{
	bool justDoIt = whichDriveReally(&drive);
	int fr = (int)platform->MaxFeedrate(drive);

	INCREMENT_MIN_MAX(fr, 5, 1, 500);

	if (justDoIt)
	{
		platform->SetMaxFeedrate(drive, (float)fr);
	}
	else
	{
		Tool *tool = reprap.GetCurrentTool();
		if (tool != NULL) // shouldn't happen owing to check in whichDriveReally()
		{
			for (int i = 0; i < (int)tool->DriveCount(); i++)
			{
				platform->SetMaxFeedrate((size_t)(tool->Drive(i) + AXES), (float)fr);
			}
		}
	}
}

void UIDisplay::changeInstantDv(size_t drive, int8_t increment)
{
	bool justDoIt = whichDriveReally(&drive);
	float dv = platform->ConfiguredInstantDv(drive);

	INCREMENT_MIN_MAX(dv, 1.0, 0.0, 100.0);

	if (justDoIt)
	{
		platform->SetInstantDv(drive, (float)dv);
	}
	else
	{
		Tool *tool = reprap.GetCurrentTool();
		if (tool != NULL) // shouldn't happen owing to check in whichDriveReally()
		{
			for (int i = 0; i < (int)tool->DriveCount(); i++)
			{
				platform->SetInstantDv((size_t)(tool->Drive(i) + AXES), (float)dv);
			}
		}
	}
}

void UIDisplay::changeStepsPerMm(size_t drive, int8_t increment)
{
	whichDriveReally(&drive);

	const float *spus = platform->GetDriveStepsPerUnit();
	float spu = spus[drive];

	INCREMENT_MIN_MAX(spu, 0.1, 0.1, 999.0);

	if (fabsf(spu - spus[drive]) > 0.001)
	{
		// We effect this using interpretation of a Mxxx command
		// That avoids having to get at the private
		// GCodes::SetPositions(float[]) member function which
		// needs to be called for deltas after changing the steps/mm
		char dl, str[18];
		dl = (drive < AXES) ? (char)((uint8_t)drive + 'X') : 'E';
		snprintf(str, sizeof(str), "M92 %c%.5f\n", dl, spu);
		buf.Put(str);
	}
}

void UIDisplay::switchToolOff(int t)
{
	Tool *tool = reprap.GetTool(t);

	if (!tool)
		return;

	for (int i = 0; i < (int)tool->HeaterCount(); i++)
	{
		heat->SwitchOff((int8_t)(0x7f & tool->Heater(i)));
	}
}

void UIDisplay::adjustTemp(int8_t heater, int increment, int maxtemp)
{
	if ((heater < 0) || (heater >= HEATERS))
		return;

	int temp = (int)heat->GetActiveTemperature(heater);
	if (temp < 0) temp = 0;
	temp += increment;

	if (temp <= 0)
	{
		heat->SwitchOff(heater);
	}
	else
	{
		if (temp > maxtemp) temp = maxtemp;
		heat->SetActiveTemperature(heater, (float)temp);
	}
}

static bool sdPresent(void)
{
	return (reprap.GetPlatform()->GetMassStorage()->FileSystemAvailable());
}

static inline void allowInterrupts()
{
	__enable_irq();
}

static inline void forbidInterrupts()
{
	__disable_irq();
}
