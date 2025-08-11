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

    Draw_Init();

    // QTMの確認
    if(){
        
        // QTMを無効化
        rpDoQTMPatchAndToggle();
        if(){
            topScreenConsole.bg = CONSOLE_GREEN;
            consoleClear();
        }else{
        }
            
    }else{
        // QTMを有効化
        rpDoQTMPatchAndToggle();
        if(){
            topScreenConsole.bg = CONSOLE_RED;
            consoleClear();
        }else{
        }
    }

    // 画面を更新
    gspWaitForVBlank();

    // ボタンが押されるまで待機
    hidScanInput();

deinit:

	romfsExit();
	gfxExit();

exlt:

    return 0;
}

