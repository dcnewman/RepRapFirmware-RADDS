#ifndef UI_SYMBOLS_H__
#define UI_SYMBOLS_H__

// Menu up sign - code 1
// ..*..  4
// .***. 14
// *.*.* 21
// ..*..  4
// ..*..  4
// ***.. 28
// .....  0
// .....  0
const uint8_t character_back[8] = { 4, 14, 21,  4,  4, 28,  0,  0};

// Degrees sign - code 2
// ..*..  4
// .*.*. 10
// ..*..  4
// .....  0
// .....  0
// .....  0
// .....  0
// .....  0
const uint8_t character_degree[8] = { 4, 10,  4,  0,  0,  0,  0,  0};

// selected - code 3
// .....  0
// ***** 31
// ***** 31
// ***** 31
// ***** 31
// ***** 31
// ***** 31
// .....  0
// .....  0
const uint8_t character_selected[8] = { 0, 31, 31, 31, 31, 31,  0,  0};

// unselected - code 4
// .....  0
// ***** 31
// *...* 17
// *...* 17
// *...* 17
// *...* 17
// ***** 31
// .....  0
// .....  0
const uint8_t character_unselected[8] = { 0, 31, 17, 17, 17, 31,  0,  0};

#if !defined(UI_SYMBOLS_PARTIAL)

// unselected - code 5
// ..*..  4
// .*.*. 10
// .*.*. 10
// .*.*. 10
// .*.*. 10
// .***. 14
// ***** 31
// ***** 31
// .***. 14
const uint8_t character_temperature[8] = { 4, 10, 10, 10, 14, 31, 31, 14};

// unselected - code 6
// .....  0
// ***.. 28
// ***** 31
// *...* 17
// *...* 17
// ***** 31
// .....  0
// .....  0
const uint8_t character_folder[8] = { 0, 28, 31, 17, 17, 31,  0,  0};

// printer ready - code 7
// *...* 17
// .*.*. 10
// ..*..  4
// *...* 17
// ..*..  4
// .*.*. 10
// *...* 17
// *...* 17
const byte character_ready[8] = { 17, 10,  4, 17,  4, 10, 17, 17};

#endif // !UI_SYMBOLS_PARTIAL
#endif // UI_SYMBOLS_H__
