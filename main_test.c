#include "braillecanvas.h"
#include "math.h"
#include <stdio.h>

#ifdef WIN32
    #include <windows.h>
    void SLEEPMS(uint32_t ms) { Sleep(ms); }
#else
    #include <unistd.h>
    void SLEEPMS(uint32_t ms) { usleep(ms*1000); }
#endif

const int backstyles[] = {
    CONSOLE_STYLE_BACKGROUND_BLACK,
    CONSOLE_STYLE_BACKGROUND_RED,
    CONSOLE_STYLE_BACKGROUND_GREEN,
    CONSOLE_STYLE_BACKGROUND_BLUE,
    CONSOLE_STYLE_BACKGROUND_YELLOW,
    CONSOLE_STYLE_BACKGROUND_MAGENTA,
    CONSOLE_STYLE_BACKGROUND_CYAN,
};

const int forestyles[] = {
    CONSOLE_STYLE_TEXT_WHITE,
    CONSOLE_STYLE_TEXT_RED,
    CONSOLE_STYLE_TEXT_GREEN,
    CONSOLE_STYLE_TEXT_BLUE,
    CONSOLE_STYLE_TEXT_YELLOW,
    CONSOLE_STYLE_TEXT_MAGENTA,
    CONSOLE_STYLE_TEXT_CYAN,
};

const int styles[][2] = {
    {CONSOLE_STYLE_BACKGROUND_BLACK, CONSOLE_STYLE_TEXT_WHITE},
    {CONSOLE_STYLE_BACKGROUND_BLACK, CONSOLE_STYLE_TEXT_RED},
    {CONSOLE_STYLE_BACKGROUND_BLACK, CONSOLE_STYLE_TEXT_GREEN},
    {CONSOLE_STYLE_BACKGROUND_BLACK, CONSOLE_STYLE_TEXT_BLUE},
    {CONSOLE_STYLE_BACKGROUND_BLACK, CONSOLE_STYLE_TEXT_YELLOW},
    {CONSOLE_STYLE_BACKGROUND_BLACK, CONSOLE_STYLE_TEXT_MAGENTA},
    {CONSOLE_STYLE_BACKGROUND_BLACK, CONSOLE_STYLE_TEXT_CYAN},

    {CONSOLE_STYLE_BACKGROUND_RED, CONSOLE_STYLE_TEXT_WHITE},
    //{CONSOLE_STYLE_BACKGROUND_RED, CONSOLE_STYLE_TEXT_RED},
    {CONSOLE_STYLE_BACKGROUND_RED, CONSOLE_STYLE_TEXT_GREEN},
    {CONSOLE_STYLE_BACKGROUND_RED, CONSOLE_STYLE_TEXT_BLUE},
    {CONSOLE_STYLE_BACKGROUND_RED, CONSOLE_STYLE_TEXT_YELLOW},
    {CONSOLE_STYLE_BACKGROUND_RED, CONSOLE_STYLE_TEXT_MAGENTA},
    {CONSOLE_STYLE_BACKGROUND_RED, CONSOLE_STYLE_TEXT_CYAN},

    {CONSOLE_STYLE_BACKGROUND_GREEN, CONSOLE_STYLE_TEXT_WHITE},
    {CONSOLE_STYLE_BACKGROUND_GREEN, CONSOLE_STYLE_TEXT_RED},
    //{CONSOLE_STYLE_BACKGROUND_GREEN, CONSOLE_STYLE_TEXT_GREEN},
    {CONSOLE_STYLE_BACKGROUND_GREEN, CONSOLE_STYLE_TEXT_BLUE},
    {CONSOLE_STYLE_BACKGROUND_GREEN, CONSOLE_STYLE_TEXT_YELLOW},
    {CONSOLE_STYLE_BACKGROUND_GREEN, CONSOLE_STYLE_TEXT_MAGENTA},
    {CONSOLE_STYLE_BACKGROUND_GREEN, CONSOLE_STYLE_TEXT_CYAN},

    {CONSOLE_STYLE_BACKGROUND_BLUE, CONSOLE_STYLE_TEXT_WHITE},
    {CONSOLE_STYLE_BACKGROUND_BLUE, CONSOLE_STYLE_TEXT_RED},
    {CONSOLE_STYLE_BACKGROUND_BLUE, CONSOLE_STYLE_TEXT_GREEN},
    //{CONSOLE_STYLE_BACKGROUND_BLUE, CONSOLE_STYLE_TEXT_BLUE},
    {CONSOLE_STYLE_BACKGROUND_BLUE, CONSOLE_STYLE_TEXT_YELLOW},
    {CONSOLE_STYLE_BACKGROUND_BLUE, CONSOLE_STYLE_TEXT_MAGENTA},
    {CONSOLE_STYLE_BACKGROUND_BLUE, CONSOLE_STYLE_TEXT_CYAN},
};

int main(int argc, char** argv)
{
    // get terminal dimensions
    uint8_t w, h;
    Terminal_GetSize(&w, &h);

    // create a canvas inside terminal - leave a border
    BrailleCanvas canvas;
    BrailleCanvas_Create(&canvas, 1, 1, w - 2, h - 2 );

    // animation
    int numstyles = 25;
    int numframes = numstyles*4;

    //getc(stdin);

    for (int i = 0; i < numframes; i++)
    {
        int style_index = i/4 % numstyles;
        canvas.BackgroundStyle = styles[style_index][0];
        canvas.FillStyle = styles[style_index][1];

        // erase previous frame
        BrailleCanvas_WipeClean(&canvas);

        // draw some boxes
        BrailleCanvas_StrokeRectangle(&canvas, 5+i, 5+i, 10, 10);
        BrailleCanvas_FillRectangle(&canvas, canvas.PixelsWidth - 15 - i, canvas.PixelsHeight - 15 - i, 10, 10);

        // draw some circles
        BrailleCanvas_StrokeCircle(&canvas, canvas.PixelsWidth - 15 - i, 5+i, 4 + i/2);
        BrailleCanvas_FillCircle(&canvas, 5+i, canvas.PixelsHeight - 15 - i, 4 + i/2);

        // draw some lines
        BrailleCanvas_StrokeLine(&canvas, canvas.PixelsWidth/2, canvas.PixelsHeight/2, (1 + 0.5f*cos(2*M_PI*i/numframes))*canvas.PixelsWidth/2, (1 + 0.5f*sin(2*M_PI*i/numframes))*canvas.PixelsHeight/2);

        BrailleCanvas_Render(&canvas);
        SLEEPMS(250);
    }

    // free
    BrailleCanvas_Destroy(&canvas);
    return 0;
}
