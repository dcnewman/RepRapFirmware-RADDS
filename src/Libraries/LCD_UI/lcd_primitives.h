#if !defined(LCD_PRIMITIVES_H__)
#define LCD_PRIMITIVES_H__

#define LCD_ENTRYMODE			0x04			/**< Set entrymode */

/** @name GENERAL COMMANDS */
/*@{*/
#define LCD_CLEAR			0x01	/**< Clear screen */
#define LCD_HOME			0x02	/**< Cursor move to first digit */
/*@}*/

/** @name ENTRYMODES */
/*@{*/
#define LCD_ENTRYMODE                       0x04   	/**< Set entrymode */
#define LCD_INCREASE		LCD_ENTRYMODE | 0x02	/**<	Set cursor move direction -- Increase */
#define LCD_DECREASE		LCD_ENTRYMODE | 0x00	/**<	Set cursor move direction -- Decrease */
#define LCD_DISPLAYSHIFTON	LCD_ENTRYMODE | 0x01	/**<	Display is shifted */
#define LCD_DISPLAYSHIFTOFF	LCD_ENTRYMODE | 0x00	/**<	Display is not shifted */
/*@}*/

/** @name DISPLAYMODES */
/*@{*/
#define LCD_DISPLAYMODE		                  0x08	/**< Set displaymode */
#define LCD_DISPLAYON		LCD_DISPLAYMODE | 0x04	/**<	Display on */
#define LCD_DISPLAYOFF		LCD_DISPLAYMODE | 0x00	/**<	Display off */
#define LCD_CURSORON		LCD_DISPLAYMODE | 0x02	/**<	Cursor on */
#define LCD_CURSOROFF		LCD_DISPLAYMODE | 0x00	/**<	Cursor off */
#define LCD_BLINKINGON		LCD_DISPLAYMODE | 0x01	/**<	Blinking on */
#define LCD_BLINKINGOFF		LCD_DISPLAYMODE | 0x00	/**<	Blinking off */
/*@}*/

/** @name SHIFTMODES */
/*@{*/
#define LCD_SHIFTMODE			            0x10	/**< Set shiftmode */
#define LCD_DISPLAYSHIFT	LCD_SHIFTMODE | 0x08	/**<	Display shift */
#define LCD_CURSORMOVE		LCD_SHIFTMODE | 0x00	/**<	Cursor move */
#define LCD_RIGHT		    LCD_SHIFTMODE | 0x04	/**<	Right shift */
#define LCD_LEFT		    LCD_SHIFTMODE | 0x00	/**<	Left shift */
/*@}*/

/** @name DISPLAY_CONFIGURATION */
/*@{*/
#define LCD_CONFIGURATION	            	0x20  	/**< Set function */
#define LCD_8BIT		LCD_CONFIGURATION | 0x10	/**<	8 bits interface */
#define LCD_4BIT		LCD_CONFIGURATION | 0x00	/**<	4 bits interface */
#define LCD_2LINE		LCD_CONFIGURATION | 0x08	/**<	2 line display */
#define LCD_1LINE		LCD_CONFIGURATION | 0x00	/**<	1 line display */
#define LCD_5X10		LCD_CONFIGURATION | 0x04	/**<	5 X 10 dots */
#define LCD_5X7			LCD_CONFIGURATION | 0x00	/**<	5 X 7 dots */

#define LCD_SETCGRAMADDR 0x40

#define lcdPutChar(value) lcdWriteByte(value, 1)
#define lcdCommand(value) lcdWriteByte(value, 0)

static const uint8_t LCDLineOffsets[] = UI_LINE_OFFSETS;

#if UI_DISPLAY_TYPE == 3

// ============= I2C LCD Display driver ================
inline static void lcdStartWrite()
{
    i2cStartWait(UI_DISPLAY_I2C_ADDRESS + I2C_WRITE);
#if UI_DISPLAY_I2C_CHIPTYPE == 1
    i2cWrite(0x14); // Start at port a
#endif
}

inline static void lcdStopWrite()
{
    i2cStop();
}

static void lcdWriteNibble(uint8_t value)
{
#if UI_DISPLAY_I2C_CHIPTYPE == 0
    value |= outputMask;
#if UI_DISPLAY_D4_PIN == 1 && UI_DISPLAY_D5_PIN == 2 && UI_DISPLAY_D6_PIN == 4 && UI_DISPLAY_D7_PIN == 8
    i2cWrite((value) | UI_DISPLAY_ENABLE_PIN);
    i2cWrite(value);
#else
    uint8_t v =
		((value & 1) ? UI_DISPLAY_D4_PIN : 0) |
		((value & 2) ? UI_DISPLAY_D5_PIN : 0) |
		((value & 4) ? UI_DISPLAY_D6_PIN : 0) |
		((value & 8) ? UI_DISPLAY_D7_PIN : 0);
    i2cWrite((v) | UI_DISPLAY_ENABLE_PIN);
    i2cWrite(v);
#endif // UI_DISPLAY_D4_PIN == 1 && UI_DISPLAY_D5_PIN == 2 && UI_DISPLAY_D6_PIN == 4 && UI_DISPLAY_D7_PIN == 8
#endif // UI_DISPLAY_I2C_CHIPTYPE == 0

#if UI_DISPLAY_I2C_CHIPTYPE == 1
    unsigned int v =
		((value & 1) ? UI_DISPLAY_D4_PIN : 0) |
		((value & 2) ? UI_DISPLAY_D5_PIN : 0) |
		((value & 4) ? UI_DISPLAY_D6_PIN : 0) |
		((value & 8) ? UI_DISPLAY_D7_PIN : 0) | outputMask;
    unsigned int v2 = v | UI_DISPLAY_ENABLE_PIN;
    i2cWrite(v2 & 255);
    i2cWrite(v2 >> 8);
    i2cWrite(v & 255);
    i2cWrite(v >> 8);
#endif // UI_DISPLAY_I2C_CHIPTYPE == 1
}

static void lcdWriteByte(uint8_t c,uint8_t rs)
{
#if UI_DISPLAY_I2C_CHIPTYPE == 0
    uint8_t mod = (rs ? UI_DISPLAY_RS_PIN : 0) | outputMask; // | (UI_DISPLAY_RW_PIN);
#if UI_DISPLAY_D4_PIN == 1 && UI_DISPLAY_D5_PIN == 2 && \
	UI_DISPLAY_D6_PIN == 4 && UI_DISPLAY_D7_PIN == 8
    uint8_t value = (c >> 4) | mod;
    i2cWrite((value) | UI_DISPLAY_ENABLE_PIN);
    i2cWrite(value);
    value = (c & 15) | mod;
    i2cWrite((value) | UI_DISPLAY_ENABLE_PIN);
    i2cWrite(value);
#else
    uint8_t value =
		((c & 16) ? UI_DISPLAY_D4_PIN : 0) |
		((c & 32) ? UI_DISPLAY_D5_PIN : 0) |
		((c & 64) ? UI_DISPLAY_D6_PIN : 0) |
		((c & 128) ? UI_DISPLAY_D7_PIN : 0) | mod;
    i2cWrite((value) | UI_DISPLAY_ENABLE_PIN);
    i2cWrite(value);
    value =
		((c & 1) ? UI_DISPLAY_D4_PIN : 0) |
		((c & 2) ? UI_DISPLAY_D5_PIN : 0) |
		((c & 4) ? UI_DISPLAY_D6_PIN : 0) |
		((c & 8) ? UI_DISPLAY_D7_PIN : 0) | mod;
    i2cWrite((value) | UI_DISPLAY_ENABLE_PIN);
    i2cWrite(value);
#endif
#endif // UI_DISPLAY_I2C_CHIPTYPE == 0

#if UI_DISPLAY_I2C_CHIPTYPE == 1
    unsigned int mod = (rs ? UI_DISPLAY_RS_PIN : 0) | outputMask; // | (UI_DISPLAY_RW_PIN);
    unsigned int value =
		((c & 16) ? UI_DISPLAY_D4_PIN : 0) |
		((c & 32) ? UI_DISPLAY_D5_PIN : 0) |
		((c & 64) ? UI_DISPLAY_D6_PIN : 0) |
		((c & 128) ? UI_DISPLAY_D7_PIN : 0) | mod;
    unsigned int value2 = (value) | UI_DISPLAY_ENABLE_PIN;
    i2cWrite(value2 & 255);
    i2cWrite(value2 >>8);
    i2cWrite(value & 255);
    i2cWrite(value>>8);
    value =
		((c & 1) ? UI_DISPLAY_D4_PIN : 0) |
		((c & 2) ? UI_DISPLAY_D5_PIN : 0) |
		((c & 4) ? UI_DISPLAY_D6_PIN : 0) |
		((c & 8) ? UI_DISPLAY_D7_PIN : 0) | mod;
    value2 = (value) | UI_DISPLAY_ENABLE_PIN;
    i2cWrite(value2 & 255);
    i2cWrite(value2 >>8);
    i2cWrite(value & 255);
    i2cWrite(value>>8);
#endif // UI_DISPLAY_I2C_CHIPTYPE == 1
}

#endif // UI_DISPLAY_TYPE == 3

#if UI_DISPLAY_TYPE == 1 || UI_DISPLAY_TYPE == 2

static void lcdWriteNibble(uint8_t value)
{
    WRITE(UI_DISPLAY_D4_PIN, value & 1);
    WRITE(UI_DISPLAY_D5_PIN, value & 2);
    WRITE(UI_DISPLAY_D6_PIN, value & 4);
    WRITE(UI_DISPLAY_D7_PIN, value & 8);
    WRITE(UI_DISPLAY_ENABLE_PIN, HIGH);// enable pulse must be >450ns
    delayMicroseconds(1);
    WRITE(UI_DISPLAY_ENABLE_PIN, LOW);
    delayMicroseconds(1);
}

static void lcdWriteByte(uint8_t c, uint8_t rs)
{
#if UI_DISPLAY_RW_PIN < 0
    delayMicroseconds(UI_DELAYPERCHAR);
#else
    SET_INPUT(UI_DISPLAY_D4_PIN);
    SET_INPUT(UI_DISPLAY_D5_PIN);
    SET_INPUT(UI_DISPLAY_D6_PIN);
    SET_INPUT(UI_DISPLAY_D7_PIN);
    WRITE(UI_DISPLAY_RW_PIN, HIGH);
    WRITE(UI_DISPLAY_RS_PIN, LOW);
    uint8_t busy;
    do
    {
        WRITE(UI_DISPLAY_ENABLE_PIN, HIGH);
		delayMicroseconds(1);
        busy = READ(UI_DISPLAY_D7_PIN);
        WRITE(UI_DISPLAY_ENABLE_PIN, LOW);
        delayMicroseconds(1);
        WRITE(UI_DISPLAY_ENABLE_PIN, HIGH);
		delayMicroseconds(1);
        WRITE(UI_DISPLAY_ENABLE_PIN, LOW);
        delayMicroseconds(1);
    }
    while (busy);
    SET_OUTPUT(UI_DISPLAY_D4_PIN);
    SET_OUTPUT(UI_DISPLAY_D5_PIN);
    SET_OUTPUT(UI_DISPLAY_D6_PIN);
    SET_OUTPUT(UI_DISPLAY_D7_PIN);
    WRITE(UI_DISPLAY_RW_PIN, LOW);
#endif // UI_DISPLAY_RW_PIN < 0

    WRITE(UI_DISPLAY_RS_PIN, rs);

    WRITE(UI_DISPLAY_D4_PIN, c & 0x10);
    WRITE(UI_DISPLAY_D5_PIN, c & 0x20);
    WRITE(UI_DISPLAY_D6_PIN, c & 0x40);
    WRITE(UI_DISPLAY_D7_PIN, c & 0x80);
    WRITE(UI_DISPLAY_ENABLE_PIN, HIGH);   // enable pulse must be >450ns
    delayMicroseconds(1);
    WRITE(UI_DISPLAY_ENABLE_PIN, LOW);
    delayMicroseconds(1);

    WRITE(UI_DISPLAY_D4_PIN, c & 0x01);
    WRITE(UI_DISPLAY_D5_PIN, c & 0x02);
    WRITE(UI_DISPLAY_D6_PIN, c & 0x04);
    WRITE(UI_DISPLAY_D7_PIN, c & 0x08);
    WRITE(UI_DISPLAY_ENABLE_PIN, HIGH);   // enable pulse must be >450ns
    delayMicroseconds(1);
    WRITE(UI_DISPLAY_ENABLE_PIN, LOW);
    delayMicroseconds(1);
}

#endif // UI_DISPLAY_TYPE == 1 || UI_DISPLAY_TYPE == 2

#if UI_DISPLAY_TYPE == 4

// Use LiquidCrystal library instead
#include <LiquidCrystal.h>

static LiquidCrystal lcd(UI_DISPLAY_RS_PIN,
						 UI_DISPLAY_RW_PIN,
						 UI_DISPLAY_ENABLE_PIN,
						 UI_DISPLAY_D4_PIN,
						 UI_DISPLAY_D5_PIN,
						 UI_DISPLAY_D6_PIN,
						 UI_DISPLAY_D7_PIN);

#endif // UI_DISPLAY_TYPE == 4

#if UI_DISPLAY_TYPE == 5

//u8glib
#ifdef U8GLIB_ST7920
#define UI_SPI_SCK UI_DISPLAY_D4_PIN
#define UI_SPI_MOSI UI_DISPLAY_ENABLE_PIN
#define UI_SPI_CS UI_DISPLAY_RS_PIN
#endif
#include "u8glib_ex.h"
static u8g_t u8g;
static u8g_uint_t u8_tx = 0;
static u8g_uint_t u8_ty = 0;

static void u8PrintChar(char c)
{
    switch(c)
    {
    case 0x7E: // right arrow
        u8g_SetFont(&u8g, u8g_font_6x12_67_75);
        u8_tx += u8g_DrawGlyph(&u8g, u8_tx, u8_ty, 0x52);
        u8g_SetFont(&u8g, UI_FONT_DEFAULT);
        break;

    case CHAR_SELECTOR:
        u8g_SetFont(&u8g, u8g_font_6x12_67_75);
        u8_tx += u8g_DrawGlyph(&u8g, u8_tx, u8_ty, 0xb7);
        u8g_SetFont(&u8g, UI_FONT_DEFAULT);
        break;

    case CHAR_SELECTED:
        u8g_SetFont(&u8g, u8g_font_6x12_67_75);
        u8_tx += u8g_DrawGlyph(&u8g, u8_tx, u8_ty, 0xb6);
        u8g_SetFont(&u8g, UI_FONT_DEFAULT);
        break;

    default:
        u8_tx += u8g_DrawGlyph(&u8g, u8_tx, u8_ty, c);
    }
}

static void printU8GRow(uint8_t x, uint8_t y, const char *text)
{
    char c;
    while ((c = *text++) != 0)
        x += u8g_DrawGlyph(&u8g, x, y, c);
}

#endif // UI_DISPLAY_TYPE == 5
#endif
