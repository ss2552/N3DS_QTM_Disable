#include <3ds/types.h>
#include <3ds/console.h>
#include <3ds/gfx.h>
#include <3ds/services/hid.h>
#include <3ds/services/apt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void rpDoQTMPatchAndToggle(void);
void print(char *msg);

// https://rgbcolorpicker.com/0-1
#define BLACK_COLOR 0b0000
#define GREEN_COLOR 0b0001
#define MAGENTA_COLOR 0b0010
#define WHITE_COLOR 0b1111

// ? b g r
// 0b0101 紫色

bool qtmDisabled = 0;
PrintConsole topScreenConsole;

int main(void)
{
	gfxInitDefault();
	consoleInit(GFX_TOP, &topScreenConsole);
	topScreenConsole.bg = BLACK_COLOR;
	topScreenConsole.fg = WHITE_COLOR;

    rpDoQTMPatchAndToggle();

    topScreenConsole.bg = qtmDisabled  ?GREEN_COLOR : MAGENTA_COLOR;

    if(qtmDisabled){
        print("QTM Disabled Success");
    }

	while (aptMainLoop()){
		hidScanInput();
		if (hidKeysDown()){
            break;
        }
	}
	gfxExit();
	return 0;
}

u8 y = 0;

void print(char *msg){
    printf("\x1b[%u;1H %s", ++y, msg);
}
