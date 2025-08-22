#include <3ds/types.h>
#include <3ds/console.h>
#include <3ds/gfx.h>
#include <3ds/services/hid.h>
#include <3ds/services/apt.h>
#include <3ds/svc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

void rpDoQTMPatchAndToggle(void);
void print(char *msg, ...);

// https://rgbcolorpicker.com/0-1
#define BLACK_COLOR 0b1000
#define ERROR_COLOR 0b1001
#define SUCCESS_COLOR 0b1010
#define WHITE_COLOR 0b1111

// ? b g r
// 0b0111 灰色
// 0b0101 紫色

bool qtmDisabled = 0;
PrintConsole topScreenConsole;

int main(void)
{
	gfxInitDefault();
	consoleInit(GFX_TOP, &topScreenConsole);
	topScreenConsole.bg = ERROR_COLOR;
	topScreenConsole.fg = WHITE_COLOR;

    print("わ");
    svcSleepThread(1 * 1000 * 1000);
    print("っ");
    svcSleepThread(1 * 1000 * 1000);
    print("ぴ");
    svcSleepThread(1 * 1000 * 1000);
    print("｜|");
    svcSleepThread(5 * 1000 * 1000);

    consoleClear();

    rpDoQTMPatchAndToggle();

    if(qtmDisabled){
        topScreenConsole.bg = SUCCESS_COLOR;
        consoleClear();
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

void print(char *msg, ...){
    va_list args;
    char s[100 ] = {0};
    va_start(args, msg);
    vsprintf(s, msg, args);
    printf("\x1b[%u;1H %s", ++y, s);
    va_end(args);
}
