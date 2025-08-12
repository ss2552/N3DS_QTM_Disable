#include <string.h>
#include <stdio.h>
#include <3ds.h>

// 結果のリザルト
extern u32 result;
// 有効又は無効か
extern bool qtmDisabled;

int main(void){

    // init
	gfxInitDefault();
    PrintConsole topScreenConsole;
	consoleInit(GFX_TOP, &topScreenConsole);

	// 背景を色々に設定 文字色を黒に設定
    topScreenConsole.bg = CONSOLE_WHITE;
	topScreenConsole.fg = CONSOLE_BLACK;
    consoleClear();
    //

    // new 3dsか確認
    s64 out;
	if(svcGetSystemInfo(&out, 0x10001, 0) != 0){
		printf("N3ds専用");
		goto deinit;
	}

	// QTMを有効または無効化
	rpDoQTMPatchAndToggle();

	// 初期 : 白  null
	// 成功 : 緑     1
	// 失敗 : 赤    -1
	topScreenConsole.bg = result > 0 ? CONSOLE_GREEN : CONSOLE_RED;

	const char msg[2][7] = {
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

    return 0;
}

