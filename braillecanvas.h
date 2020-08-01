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

#ifndef _BRAILLE_CANVAS_H_
#define _BRAILLE_CANVAS_H_

#include <stdint.h>
#include "terminal.h"

typedef struct
{
    // placement of this canvas inside the terminal
    uint8_t CharacterTop;
    uint8_t CharacterLeft;
    uint8_t CharacterWidth;
    uint8_t CharacterHeight;

    uint16_t PixelsWidth;
    uint16_t PixelsHeight;

    ConsoleStyleText FillStyle;
    ConsoleStyleBackground BackgroundStyle;

    uint8_t *PixelBuffer;
} BrailleCanvas;

void BrailleCanvas_Create(BrailleCanvas*, uint8_t, uint8_t, uint8_t, uint8_t);
void BrailleCanvas_Destroy(BrailleCanvas*);
void BrailleCanvas_Render(BrailleCanvas*);
void BrailleCanvas_Render_ByCallback(BrailleCanvas* canvas, void*object, void(*SetFunc)(uint8_t, uint8_t, uint8_t, uint8_t, char*,void*object));

void BrailleCanvas_WipeClean(BrailleCanvas*);
void BrailleCanvas_FillRectangle(BrailleCanvas*, uint16_t, uint16_t, uint16_t, uint16_t);
void BrailleCanvas_FillCircle(BrailleCanvas*, uint16_t, uint16_t, uint16_t);

void BrailleCanvas_StrokeRectangle(BrailleCanvas*, uint16_t, uint16_t, uint16_t, uint16_t);
void BrailleCanvas_StrokeCircle(BrailleCanvas*, uint16_t, uint16_t, uint16_t);
void BrailleCanvas_StrokeLine(BrailleCanvas*, uint16_t, uint16_t, uint16_t, uint16_t);

#endif // _BRAILLE_CANVAS_H_
