#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <3ds.h>
#include "memory.h"
#include "service_manager.h"
#include "utils.h"
#include "MyThread.h"

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
	if(rpDoQTMPatchAndToggle()){
		
		topScreenConsole.bg = CONSOLE_GREEN;
	}else{
		topScreenConsole.bg = CONSOLE_RED;
	}
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

