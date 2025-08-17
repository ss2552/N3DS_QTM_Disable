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

int result = 0;

int qtmDisabled = 0;

int main(void)
{

	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);

	const char *msg[] = {"mukou ka", "yuukou ka"};
	printf("\x1b[2;10HQTM wo %s shimasu", msg[qtmDisabled]);

 rpDoQTMPatchAndToggle();

	printf("\x1b[4;10H%s shimashita (%i)", msg[!qtmDisabled], result);

	while(aptMainLoop())
	{
		hidScanInput();
		if(hidKeysDown())
			break;
	}

	gfxExit();
	return 0;
}
