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

#include "braillecanvas.h"
#include <stdio.h>
#include <stdlib.h>

#define BRAILLE_PIXELS_WIDTH 2
#define BRAILLE_PIXELS_HEIGHT 4
#define BRAILLE_UNICODE 0x2800

static const uint32_t UNICODE_BRAILLE_PATTERN[4][2] = {
    {0x01, 0x08},
    {0x02, 0x10},
    {0x04, 0x20},
    {0x40, 0x80}
};

void BrailleCanvas_Create(BrailleCanvas* canvas, uint8_t X, uint8_t Y, uint8_t W, uint8_t H)
{
    SetupTerminal(); // sets a font that accepts braille characters and changes encoding to UTF-8

    canvas->CharacterTop = Y;
    canvas->CharacterLeft = X;
    canvas->CharacterWidth = W;
    canvas->CharacterHeight = H;

    // transforms the character-measured size in pixel-measured size
    canvas->PixelsWidth = canvas->CharacterWidth*BRAILLE_PIXELS_WIDTH;
    canvas->PixelsHeight = canvas->CharacterHeight*BRAILLE_PIXELS_HEIGHT;

    // default style
    canvas->FillStyle = CONSOLE_STYLE_TEXT_WHITE;
    canvas->BackgroundStyle = CONSOLE_STYLE_BACKGROUND_RED;

    // allocate a buffer for the pixel information
    canvas->PixelBuffer = (uint8_t*)calloc(canvas->PixelsHeight * canvas->PixelsWidth, sizeof(uint8_t)); // allocate memory for rows (Y=0...H)

    BrailleCanvas_WipeClean(canvas); // make sure no memory garbage on the recently allocated blocks
}

// frees the resources used by the canvas
void BrailleCanvas_Destroy(BrailleCanvas* canvas)
{
    free(canvas->PixelBuffer);
}

// sets all pixels to 0
void BrailleCanvas_WipeClean(BrailleCanvas* canvas)
{
    memset(canvas->PixelBuffer, 0, canvas->PixelsWidth * canvas->PixelsHeight * sizeof(uint8_t));
}

#define setPixel(x,y) canvas->PixelBuffer[(x) + (y)*canvas->PixelsWidth] = 1; // unsafe set pixel in buffer (may overflow)
#define safeSetPixel(x, y) if (x < canvas->PixelsWidth && y < canvas->PixelsHeight) setPixel(x,y) // safe set pixel (some functions have betters ways to prevent overflow ... such as "break" statements )
#define getPixel(x,y) (canvas->PixelBuffer[(x) + (y)*canvas->PixelsWidth])

void BrailleCanvas_GetCharacter(BrailleCanvas* canvas, uint16_t row, uint16_t col, uint32_t * unicode)
{
    uint16_t x = col*BRAILLE_PIXELS_WIDTH;
    uint16_t y = row*BRAILLE_PIXELS_HEIGHT;

    *unicode = BRAILLE_UNICODE;

    // build the unicode code-point by matching the pixels in the block with the braille pattern
    *unicode += UNICODE_BRAILLE_PATTERN[0][0]*getPixel(x+0,y+0);  *unicode += UNICODE_BRAILLE_PATTERN[0][1]*getPixel(x+1,y+0);
    *unicode += UNICODE_BRAILLE_PATTERN[1][0]*getPixel(x+0,y+1);  *unicode += UNICODE_BRAILLE_PATTERN[1][1]*getPixel(x+1,y+1);
    *unicode += UNICODE_BRAILLE_PATTERN[2][0]*getPixel(x+0,y+2);  *unicode += UNICODE_BRAILLE_PATTERN[2][1]*getPixel(x+1,y+2);
    *unicode += UNICODE_BRAILLE_PATTERN[3][0]*getPixel(x+0,y+3);  *unicode += UNICODE_BRAILLE_PATTERN[3][1]*getPixel(x+1,y+3);
}

// converts pixel groups to braille characters and prints them on screen, character by character
void BrailleCanvas_Render_ByCallback(BrailleCanvas* canvas, void* object,void(*SetFunc)(uint8_t, uint8_t, uint8_t, uint8_t, char*, void*))
{
    // WARNING
    // BACAUSE THIS FUNCTION IS INTENDED TO BE USED NESTED INSIDE ANOTHER RENDERING FUNCTION, IT WILL NOT CHANGE THE CURSOR POSTION OR THE CONSOLE STYLE
    char utf8[5];
    for (int16_t row = 0; row < canvas->CharacterHeight; row++)
    {
        for (int16_t col = 0; col < canvas->CharacterWidth; col++)
        {
            uint32_t unicode;
            BrailleCanvas_GetCharacter(canvas, row, col, &unicode);

            if (unicode != BRAILLE_UNICODE)
            {
                utf8_encode(&utf8[0], unicode);
                SetFunc(col,row,canvas->CharacterWidth,canvas->CharacterHeight,utf8,object);
            }
        }
    }
}

// converts pixel groups to braille characters and prints them on screen, row by row
void BrailleCanvas_Render(BrailleCanvas* canvas)
{
    Terminal_SaveCursorPosition(); // let's save the current state before we do anything

    Terminal_SetStyle(canvas->FillStyle, canvas->BackgroundStyle); // set the style
    Terminal_ClearArea(canvas->CharacterLeft, canvas->CharacterTop, canvas->CharacterWidth, canvas->CharacterHeight); // erase previous render

    char utf8[5]; // holds the bytes to encode one single braille character
    char print_line_buffer[canvas->CharacterWidth * 5]; // buffer an entire row - it's faster than printing one character at a time

    for (int16_t row = 0; row < canvas->CharacterHeight; row++) // iterate over the rows and print along the lines (natural printing left to right)
    {
        memset(print_line_buffer, 0, sizeof(print_line_buffer)); // get rid of values from the last cycle

        for (int16_t col = 0; col < canvas->CharacterWidth; col++)
        {
            // coordinates of the top-left-most pixel in the braille dot-pattern
            uint32_t unicode;
            BrailleCanvas_GetCharacter(canvas, row, col, &unicode);

            if (unicode != BRAILLE_UNICODE) // only plot unicode if there is an actual symbol ...
            {
                utf8_encode(&utf8[0], unicode); // tranform code-point to utf8
                strcat(print_line_buffer, utf8); // append character to the line
            }
            else
                strcat(print_line_buffer, " "); // blank braille symbols will be an ascii empty space
        }

        Terminal_SetCursorPosition(canvas->CharacterLeft, canvas->CharacterTop + row); // move to the correct row
        fwrite(print_line_buffer, strlen(print_line_buffer)*sizeof(char), 1, stdout); // faster than printf
        //printf("%s", print_line_buffer); // print one row
    }

    Terminal_RestoreCursorSavedPosition(); // move the cursor back to where it was before
}

void BrailleCanvas_FillRectangle(BrailleCanvas* canvas, uint16_t X, uint16_t Y, uint16_t W, uint16_t H)
{
    for (uint16_t currX = X; currX < min(X+W, canvas->PixelsWidth-1); currX++)
        for (uint16_t currY = Y; currY < min(Y+H, canvas->PixelsHeight-1); currY++)
            setPixel(currX,currY);
}

void BrailleCanvas_StrokeRectangle(BrailleCanvas* canvas, uint16_t X, uint16_t Y, uint16_t W, uint16_t H)
{
    BrailleCanvas_StrokeLine(canvas, X, Y, X, Y+H); // left side
    BrailleCanvas_StrokeLine(canvas, X+W, Y, X+W, Y+H); // right side
    BrailleCanvas_StrokeLine(canvas, X, Y, X+W, Y); // top side
    BrailleCanvas_StrokeLine(canvas, X, Y+H, X+W, Y+H); // bottom side
}

// Bresenham's circle algorithm
void BrailleCanvas_BresenhamCircle(BrailleCanvas* canvas, uint16_t x0, uint16_t y0, uint16_t r, uint8_t fillOrStroke)
{
    int f = 1 - r;
    int ddF_x = 1;
    int ddF_y = -2 * r;
    int x = 0;
    int y = r;

    if (fillOrStroke)
    {
        //BrailleCanvas_StrokeLine(canvas, x0, y0 + r, x0, y0 - r);
        BrailleCanvas_StrokeLine(canvas, x0 + r, y0, x0 - r, y0);
    }
    else
    {
        safeSetPixel (x0, y0 + r);
        safeSetPixel (x0, y0 - r);
        safeSetPixel (x0 + r, y0);
        safeSetPixel (x0 - r, y0);
    }

    while (x < y)
    {
        if (f >= 0)
        {
          y--;
          ddF_y += 2;
          f += ddF_y;
        }

        x++;
        ddF_x += 2;
        f += ddF_x;

        if (fillOrStroke)
        {
            BrailleCanvas_StrokeLine(canvas, x0 + x, y0 + y, x0 - x, y0 + y);
            BrailleCanvas_StrokeLine(canvas, x0 + x, y0 - y, x0 - x, y0 - y);

            BrailleCanvas_StrokeLine(canvas, x0 + y, y0 + x, x0 - y, y0 + x);
            BrailleCanvas_StrokeLine(canvas, x0 + y, y0 - x, x0 - y, y0 - x);
        }
        else
        {
            safeSetPixel (x0 + x, y0 + y);
            safeSetPixel (x0 - x, y0 + y);

            safeSetPixel (x0 + x, y0 - y);
            safeSetPixel (x0 - x, y0 - y);

            safeSetPixel (x0 + y, y0 + x);
            safeSetPixel (x0 - y, y0 + x);

            safeSetPixel (x0 + y, y0 - x);
            safeSetPixel (x0 - y, y0 - x);
        }
    }
}

void BrailleCanvas_FillCircle(BrailleCanvas* canvas, uint16_t X, uint16_t Y, uint16_t R) { BrailleCanvas_BresenhamCircle(canvas, X, Y, R, 1); }
void BrailleCanvas_StrokeCircle(BrailleCanvas* canvas, uint16_t X, uint16_t Y, uint16_t R) { BrailleCanvas_BresenhamCircle(canvas, X, Y, R, 0); }

// Bresenham's line algorithm
void BrailleCanvas_StrokeLine(BrailleCanvas* canvas, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    for (;;)
    {
        if (x0 >= canvas->PixelsWidth || y0 >= canvas->PixelsHeight) break; // do not overflow buffer -- trim the line

        setPixel (x0,y0);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}
