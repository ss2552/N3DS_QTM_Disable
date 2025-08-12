#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 結果のリザルト
extern int result;
// 有効又は無効か
extern bool qtmDisabled;
// 
extern PrintConsole topScreenConsole;

// https://github.com/devkitPro/3ds-examples/tree/master/qtm

int main(void){

    // デフォルトでグラフィック初期化
	gfxInitDefault();
	// コンソールの初期化
	consoleInit(GFX_TOP, &topScreenConsole);

	// 背景を色々に設定 文字色を黒に設定
    topScreenConsole.bg = CONSOLE_WHITE;
	topScreenConsole.fg = 0;
	
	// 一応 コンソールをクリア
    consoleClear();
    //

    // new 3dsか確認
    s64 out;
	if(svcGetSystemInfo(&out, 0x10001, 0) != 0){
		printf("N3ds専用¥n");
		goto deinit;
	}

	// QTMを有効または無効化
	rpDoQTMPatchAndToggle();

	// 初期 : 白  null
	// 成功 : 緑     1
	// 失敗 : 赤    -1
	if(result > 0){
		topScreenConsole.bg = CONSOLE_GREEN;
	}else{
		if(result == 0){
			printf("結果無し");
		}
        topScreenConsole.bg = CONSOLE_RED;
	}

	const char* msg[2][10] = {
		"有効化",
		"無効化"
	};
	
	printf("QTMを%c", msg[qtmDisabled]);
	
	//
	gfxFlushBuffers();();

    // 画面描画が完了するまで待機
    gspWaitForVBlank();

    // ボタンが押されるまで待機
    hidScanInput();

deinit:

	gfxExit();

    return 0;
}

