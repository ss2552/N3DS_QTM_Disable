#include <string.h>
#include <stdio.h>
#include <3ds.h>

extern u32 result = 0;
extern bool qtmDisabled = false;

int main(void)
{

    // new 3dsか確認
    s64 out;
    if(svcGetSystemInfo(&out, 0x10001, 0) != 0)
        goto exit;

    // init

    PrintConsole topScreenConsole;
	gfxInitDefault();
	consoleInit(GFX_TOP, &topScreenConsole);

    topScreenConsole.bg = CONSOLE_WHITE;
    consoleClear();
    
    //

	// QTMを有効または無効化
	rpDoQTMPatchAndToggle();
		
	topScreenConsole.bg = result > 0 ? CONSOLE_GREEN : CONSOLE_RED;

	char msg[2][] = {
		"有効化",
		"無効化"
	}
	
	printf("QTMを%c", msg[qtmDisabled]);
	
	// 
	consoleClear();

    // 画面を更新
    gspWaitForVBlank();

    // ボタンが押されるまで待機
    hidScanInput();

deinit:

	gfxExit();

exlt:

    return 0;
}

