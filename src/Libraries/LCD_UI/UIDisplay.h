#ifndef UIDISPLAY_H__
#define UIDISPLAY_H__

#include "UIBuffer.h"

typedef unsigned long millis_t;

#define SD_MAX_FOLDER_DEPTH 2
#define LONG_FILENAME_LENGTH (13 * 2 + 1)

#define MENU_MODE_SD_MOUNTED   0x01
#define MENU_MODE_SD_PRINTING  0x02
#define MENU_MODE_SD_PAUSED    0x04
#define MENU_MODE_FAN_RUNNING  0x08
#define MENU_MODE_PRINTING     0x10
#define MENU_MODE_AUTOMOUNT    0x20

// Maximum size of a row - if row is larger, text gets scrolled
#define MAX_COLS 28

class UIDisplay {

public:
    UIDisplay(Platform *p, Heat *h, GCodes *g);
	void Spin();
    void Init(); // Initialize display and keys
	void setMode(uint16_t flags, bool set);
    void slowAction();
    void fastAction();
    void mediumAction();

private:
	bool active;

	// 1 = fast key action
	// 2 = slow key action
	// 4 = slow action running
	volatile uint8_t flags;

	int8_t encoderPos;
	int8_t encoderLast;

	char displayCache[UI_ROWS][MAX_COLS+1];
    uint8_t col;         // current col for buffer prefill
    int8_t shift;        // Display shift for scrolling text

    uint8_t menuPos[5];  // Positions in menu
    void *menu[5];       // Menus active
    uint8_t menuTop[5];  // Top row in menu
    uint8_t menuLevel;   // current menu level

#if UI_ANIMATION
    int8_t oldMenuLevel; // used to control transition behaviors
#endif

    char statusMsg[17];
    void *errorMsg;
	const char *statusText;

	uint16_t activeAction;     // action for ok/next/previous
    uint16_t lastAction;
	millis_t lastSwitch;       // Last time display switched pages
    millis_t lastRefresh;
    uint16_t lastButtonAction;
    millis_t lastButtonStart;
    millis_t nextRepeat;       // Time of next autorepeat
    millis_t lastNextPrev;     // for increasing speed settings
    float lastNextAccumul;     // Accumulated value
    unsigned int outputMask;   // Output mask for backlight, leds etc.
    int repeatDuration;        // Time beween twp actions if autorepeat is enabled
    uint8_t encoderStartScreen;

	// Save these pointers to cut down on dereferencing "reprap."
	Platform *platform;
	Heat *heat;

	// GCode buffer: commands to be processed by GCodes::Spin()
	UIBuffer buf;

	// SD card folder state
    uint8_t folderLevel;
	int cwd_skip;
    char cwd[(SD_MAX_FOLDER_DEPTH) * (LONG_FILENAME_LENGTH) + 2];

#if UI_AUTORETURN_TO_MENU_AFTER != 0
	millis_t ui_autoreturn_time;
#endif

	// SD card navigation and actions
	const char *my_cwd();
    void sdUpdateFileCount();
    void sdGoDir(const char *name);
    bool sdIsDirname(const char *name);
	void sdGetFilenameAt(uint8_t filePos, char *filename);
	void sdPrintFile(const char *filename);
	bool sdDeleteFile(const char *filename);
	void sdRefresh(uint8_t &r, char cache[UI_ROWS][MAX_COLS+1]);

	// Convenience routines to perform requested actions
	void jog(size_t axis, float distance);
	void changeMaxAccel(size_t drive, int8_t increment);
	void changeMaxFeedrate(size_t drive, int8_t increment);
	void changeInstantDv(size_t drive, int8_t increment);
	void changeStepsPerMm(size_t drive, int8_t increment);
	void switchToolOff(int t);
	void adjustTemp(int8_t heater, int increment, int maxtemp);

	// Format data for the LCD display
    void addInt(int value, uint8_t digits, char fillChar=' '); // Print int into printCols
    void addLong(long value,char digits);
    void addFloat(float number, char fixdigits, uint8_t digits);
    void addString(const char *text);

    void okAction();
    void nextPreviousAction(int8_t next);

    void waitForKey();
    void printRow(uint8_t r, const char *txt, const char *txt2,
				  uint8_t changeAtCol); // Print row on display
    void printRow(uint8_t r, const char *txt);
    void parse(const char *txt); // Parse output and write to printCols;
    void refreshPage();
    void executeAction(int action);
    void finishAction(int action);
    void pushMenu(void *men, bool refresh);
    void adjustMenuPos();
    void setStatus(const char *txt, bool error = false);
    inline void setOutputMaskBits(unsigned int bits) { outputMask |= bits; }
    inline void unsetOutputMaskBits(unsigned int bits) { outputMask &= ~bits; }

	void checkKeys(int &action);
	void checkSlowEncoder(void);
	void checkSlowKeys(int &action);

	void initializeLCD();
	void initKeys();
	void resetTimes() { lastSwitch = lastRefresh = millis(); }
    void createChar(uint8_t location,const uint8_t charmap[]);
};

#endif // UIDISPLAY_H__
