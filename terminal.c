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

#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// UNIX SPECIFIC CODE
//===========================================================================================
#if defined(unix) || defined(__unix__) || defined(__unix)
void SetupTerminal()
{
    // does not require setup
}

void Terminal_ClearArea(uint8_t X, uint8_t Y, uint8_t W, uint8_t H)
{
    for (uint8_t currY = Y; currY < Y+H; currY++) // rows
    {
        Terminal_SetCursorPosition(X, currY);
        for (uint8_t currX = X; currX < X+W; currX++)
            printf(" ");
    }
}

// VT100 escape codes:
void Terminal_SetCursorPosition(uint16_t X, uint16_t Y)
{
    putc(0x1B, stdout);
    printf("[%u;%uf", Y, X);
}

void Terminal_SetStyle(ConsoleStyleText text, ConsoleStyleBackground bg)
{
    putc(0x1B, stdout);
    if (text == CONSOLE_STYLE_TEXT_WHITE)
        printf("[37;1m");
    else
        printf("[%um", text);

    putc(0x1B, stdout);
    printf("[%um", bg);
}

void Terminal_SaveCursorPosition()
{
    printf("\x1B"); printf("7");
}

void Terminal_RestoreCursorSavedPosition()
{
    printf("\x1B"); printf("8");
}

void Terminal_GetSize(uint8_t *WidthColumns, uint8_t *RowsHeight)
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    *RowsHeight = (uint8_t)w.ws_row; // atoi(getenv("LINES"));
    *WidthColumns = (uint8_t)w.ws_col; //atoi(getenv("COLUMNS"));
}

#endif // defined
//===========================================================================================


// WINDOWS SPECIFIC CODE
//===========================================================================================
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__))

//#include "winfontinstall.h"

void SetupTerminal()
{
    /*uint8_t myFontIndex = 0;

    // CONFIGURE WINDOWS TO PRINT UTF8 CHARACTER
    if (!CheckFontInstalled(myFontIndex))
        if (InstallFont(myFontIndex))
            goto SETFONT;
        else
        {
            ExportFont(myFontIndex); // if we can't install ourselves - then just extract the file for manual installation
            return;
        }
    else
        goto SETFONT;

    SETFONT:
    if (SetWindowsConsoleFont(myFontIndex)) // A FONT THAT SUPPORTS BOTH BRAILLE AND EXTENDED-ASCII*/
        SetConsoleOutputCP(CP_UTF8); // SET CODEPAGE TO UTF-8
}
void Terminal_GetSize(uint8_t *WidthColumns, uint8_t *HeightRows)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
    {
        *WidthColumns = (uint8_t)csbi.dwSize.X;
        *HeightRows = (uint8_t)(csbi.srWindow.Bottom - csbi.srWindow.Top + 1);//csbi.dwSize.Y;
    }
}

void Terminal_ClearArea(uint8_t X, uint8_t Y, uint8_t W, uint8_t H)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    for (uint8_t line = 0; line < H; line++)
    {
        DWORD cCharsWritten;
        COORD coordScreen = { X, Y+line };
        FillConsoleOutputCharacter( hConsole, (TCHAR) ' ', (DWORD)W, coordScreen, &cCharsWritten ); // print the characters
        GetConsoleScreenBufferInfo( hConsole, &csbi );
        FillConsoleOutputAttribute( hConsole, csbi.wAttributes, (DWORD)W, coordScreen, &cCharsWritten ); // print the style (color, bg, etc..)
    }
}

void Terminal_SetCursorPosition(uint16_t X, uint16_t Y)
{
    fflush(stdout);
    COORD coordScreen = { X, Y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordScreen);
}

uint16_t ___private_saved_cursor_x;
uint16_t ___private_saved_cursor_y;
void Terminal_SaveCursorPosition()
{
    fflush(stdout);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
    {
        ___private_saved_cursor_x = csbi.dwCursorPosition.X;
        ___private_saved_cursor_y = csbi.dwCursorPosition.Y;
    }
}

void Terminal_RestoreCursorSavedPosition()
{
    Terminal_SetCursorPosition(___private_saved_cursor_x, ___private_saved_cursor_y);
}

void Terminal_SetStyle(ConsoleStyleText text, ConsoleStyleBackground bg)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), text | bg);
}

#endif

// converts code-point to utf8 encoding and returns the number of bytes used
int utf8_encode(char *out, uint32_t utf)
{
    if (utf <= 0x7F)
    {
        // Plain ASCII
        out[0] = (char) utf;
        out[1] = 0;
        return 1;
    }
    else if (utf <= 0x07FF)
    {
        // 2-byte unicode
        out[0] = (char) (((utf >> 6) & 0x1F) | 0xC0);
        out[1] = (char) (((utf >> 0) & 0x3F) | 0x80);
        out[2] = 0;
        return 2;
    }
    else if (utf <= 0xFFFF)
    {
        // 3-byte unicode
        out[0] = (char) (((utf >> 12) & 0x0F) | 0xE0);
        out[1] = (char) (((utf >>  6) & 0x3F) | 0x80);
        out[2] = (char) (((utf >>  0) & 0x3F) | 0x80);
        out[3] = 0;
        return 3;
    }
    else if (utf <= 0x10FFFF)
    {
        // 4-byte unicode
        out[0] = (char) (((utf >> 18) & 0x07) | 0xF0);
        out[1] = (char) (((utf >> 12) & 0x3F) | 0x80);
        out[2] = (char) (((utf >>  6) & 0x3F) | 0x80);
        out[3] = (char) (((utf >>  0) & 0x3F) | 0x80);
        out[4] = 0;
        return 4;
    }
    else
    {
        // error - use replacement character
        out[0] = (char) 0xEF;
        out[1] = (char) 0xBF;
        out[2] = (char) 0xBD;
        out[3] = 0;
        return 0;
    }
}

void Terminal_Clear()
{
    fflush(stdout); // guarantee no characters will be written from the buffer after the screen is clear

    uint8_t W, H;
    Terminal_GetSize(&W, &H);
    Terminal_ClearArea(0, 0, W, H);

    return;
}

static char bTerminalMutexInitialized = 0;
static pthread_mutex_t terminal_lock; //= NULL;
int Terminal_Lock()
{
    if (!bTerminalMutexInitialized) // MUTEX NOT YET CREATED
    {
        if (pthread_mutex_init(&terminal_lock, NULL) != 0) // TRY CREATE IT
        {
            fprintf(stderr, "\nTerminal mutex init has failed\n");
            return -1;
        }
        else
            bTerminalMutexInitialized = 1;
    }

    return pthread_mutex_lock(&terminal_lock);
}

int Terminal_Unlock()
{
    if (!bTerminalMutexInitialized)
        return -1;

    return pthread_mutex_unlock(&terminal_lock);
}
//===========================================================================================
