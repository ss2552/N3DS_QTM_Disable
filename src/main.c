#include <3ds/console.h>
#include <3ds/gfx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// https://rgbcolorpicker.com/565
#define BLACK_COLOR	RGB565(  0,  0,  0)
#define GREEN_COLOR RGB565(  0, 63,  0)
#define RED_COLOR   RGB565( 63,  0,  0)
#define WHITE_COLOR RGB565( 63, 63, 63)

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
// PrintConsole topScreenConsole;
	consoleInit(GFX_TOP, &topScreenConsole);

	// 背景を白色に設定 文字色を黒に設定
    topScreenConsole.bg = WHITE_COLOR;
	topScreenConsole.fg = WHITE_BLACK;

	// 画面を空白埋め
	consoleClear();

    // new 3dsか確認
	// PTM:CheckNew3DS https://www.3dbrew.org/wiki/PTM:CheckNew3DS
    bool is_n3ds;
	PTMSYSM_CheckNew3DS(&is_n3ds);
	if(is_n3ds != 1){
		printf("N3ds専用¥n");
		goto deinit;
	}

	// QTMを有効または無効化
	rpDoQTMPatchAndToggle();

	// 初期 : 白  null
	// 成功 : 緑     1
	// 失敗 : 赤    -1
	if(result > 0){
		topScreenConsole.bg = GREEN_COLOR;
	}else{
		if(result == 0){
			printf("結果無し");
		}
        topScreenConsole.bg = RED_COLOR;
	}

	const char* msg[] = {
		"QTMを有効化",
		"QTMを無効化"
	};
	
	printf("・%s\n", msg[qtmDisabled]);
	
	//
	gfxFlushBuffers();

    // 画面描画が完了するまで待機
    gspWaitForVBlank();

    // ボタンが押されるまで待機
    hidScanInput();

deinit:

	gfxExit();

    return 0;
}

