// ===================================================================================  //
//    This program is free software: you can redistribute it and/or modify              //
//    it under the terms of the GNU General Public License as published by              //
//    the Free Software Foundation, either version 3 of the License, or                 //
//    (at your option) any later version.                                               //
//                                                                                      //
//    This program is distributed in the hope that it will be useful,                   //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of                    //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                     //
//    GNU General Public License for more details.                                      //
//                                                                                      //
//    You should have received a copy of the GNU General Public License                 //
//    along with this program.  If not, see <https://www.gnu.org/licenses/>5.           //
//                                                                                      //
//    Copyright: Luiz Gustavo Pfitscher e Feldmann, 2020                                //
// ===================================================================================  //

#ifndef _TERMINAL_CANVAS_H_
#define _TERMINAL_CANVAS_H_

#include <stdint.h>

// UNIX
//===========================================================================================
#if defined(unix) || defined(__unix__) || defined(__unix)
#include <sys/ioctl.h>
#include <unistd.h>

#ifndef min
    #define min(a,b) ( ((a) < (b)) ? (a) : (b))
#endif

#ifndef max
    #define max(a,b) ( ((a) > (b)) ? (a) : (b))
#endif

typedef enum {
    CONSOLE_STYLE_BACKGROUND_WHITE = 107,
    CONSOLE_STYLE_BACKGROUND_BLACK = 40,
    CONSOLE_STYLE_BACKGROUND_RED = 41,
    CONSOLE_STYLE_BACKGROUND_GREEN = 42,
    CONSOLE_STYLE_BACKGROUND_BLUE = 44,
    CONSOLE_STYLE_BACKGROUND_YELLOW = 43,
    CONSOLE_STYLE_BACKGROUND_MAGENTA = 45,
    CONSOLE_STYLE_BACKGROUND_CYAN = 46,
    CONSOLE_STYLE_BACKGROUND_GREY = 47,
} ConsoleStyleBackground;

typedef enum {
    CONSOLE_STYLE_TEXT_WHITE = 1,
    CONSOLE_STYLE_TEXT_BLACK = 30,
    CONSOLE_STYLE_TEXT_RED = 31,
    CONSOLE_STYLE_TEXT_GREEN = 32,
    CONSOLE_STYLE_TEXT_BLUE = 34,
    CONSOLE_STYLE_TEXT_YELLOW = 33,
    CONSOLE_STYLE_TEXT_MAGENTA = 35,
    CONSOLE_STYLE_TEXT_CYAN = 36,
} ConsoleStyleText;

#define TERMINAL_ORIGIN 1

#endif
//===========================================================================================


// WINDOWS
//===========================================================================================
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__))
#include<windows.h>

typedef enum {
    CONSOLE_STYLE_BACKGROUND_WHITE = BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN,
    CONSOLE_STYLE_BACKGROUND_BLACK = 0,
    CONSOLE_STYLE_BACKGROUND_RED = BACKGROUND_RED,
    CONSOLE_STYLE_BACKGROUND_GREEN = BACKGROUND_GREEN,
    CONSOLE_STYLE_BACKGROUND_BLUE = BACKGROUND_BLUE,
    CONSOLE_STYLE_BACKGROUND_YELLOW = BACKGROUND_RED | BACKGROUND_GREEN,
    CONSOLE_STYLE_BACKGROUND_MAGENTA = BACKGROUND_RED | BACKGROUND_BLUE,
    CONSOLE_STYLE_BACKGROUND_CYAN = BACKGROUND_BLUE | BACKGROUND_GREEN,
    CONSOLE_STYLE_BACKGROUND_GREY = BACKGROUND_INTENSITY,
} ConsoleStyleBackground;

typedef enum {
    CONSOLE_STYLE_TEXT_WHITE = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN,
    CONSOLE_STYLE_TEXT_BLACK = 0,
    CONSOLE_STYLE_TEXT_RED = FOREGROUND_RED,
    CONSOLE_STYLE_TEXT_GREEN = FOREGROUND_GREEN,
    CONSOLE_STYLE_TEXT_BLUE = FOREGROUND_BLUE,
    CONSOLE_STYLE_TEXT_YELLOW = FOREGROUND_RED | FOREGROUND_GREEN,
    CONSOLE_STYLE_TEXT_MAGENTA = FOREGROUND_RED | FOREGROUND_BLUE,
    CONSOLE_STYLE_TEXT_CYAN = FOREGROUND_BLUE | FOREGROUND_GREEN,
} ConsoleStyleText;

#define TERMINAL_ORIGIN 0

#endif

void Terminal_GetSize(uint8_t *, uint8_t *);
void Terminal_ClearArea(uint8_t X, uint8_t Y, uint8_t W, uint8_t H);
void Terminal_SetCursorPosition(uint16_t X, uint16_t Y);
void Terminal_SetStyle(ConsoleStyleText, ConsoleStyleBackground);
void Terminal_SaveCursorPosition();
void Terminal_RestoreCursorSavedPosition();
void SetupTerminal();
void Terminal_PrintUnicode(uint32_t);
void Terminal_Clear();
int Terminal_Lock();
int Terminal_Unlock();
int utf8_encode(char *out, uint32_t utf);

#endif
