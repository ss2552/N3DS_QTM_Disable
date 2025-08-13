#include <3ds/services/ptmsysm.h>
#include <3ds/services/apt.h>
#include <3ds/console.h>
#include <3ds/gfx.h>
#include <3ds/svc.h>
#include <3ds/types.h>
#include <3ds/services/hid.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void rpDoQTMPatchAndToggle(void);

// https://rgbcolorpicker.com/0-1
#define BLACK_COLOR 0b0000
#define GREEN_COLOR 0b1010
#define RED_COLOR 0b0101
#define WHITE_COLOR 0b1111

// 結果のリザルト
int result = 0;
// 有効又は無効か
bool qtmDisabled = false;
//
bool isN3DS = false;
//
PrintConsole topScreenConsole;
//
bool skip = false;

// https://github.com/devkitPro/3ds-examples/tree/master/qtm

int main(void)
{

	// --------------------------------------------------

	// デフォルトでグラフィック初期化
	gfxInitDefault();
	// コンソールの初期化
	consoleInit(GFX_TOP, &topScreenConsole);

	// 背景を白色に設定 文字色を黒に設定
	topScreenConsole.bg = RED_COLOR;
	topScreenConsole.fg = BLACK_COLOR;

	// 画面を空白埋め
	consoleClear();

	// --------------------------------------------------

	// new 3dsか確認
	// PTM:CheckNew3DS https://www.3dbrew.org/wiki/PTM:CheckNew3DS
	if (PTMSYSM_CheckNew3DS(&isN3DS) < 0)
	{
		printf("\x1b[3;1HErr\n");
		skip = true;
	}
	else if (isN3DS != 1)
	{
		printf("\x1b[6;1HN3ds Only\n");
		skip = true;
	}

	// QTMを有効または無効化
	if (!skip)
		rpDoQTMPatchAndToggle();

	// 結果
	// --------------------------------------------------
	// 初期 : 白  null
	// 成功 : 緑     1
	// 失敗 : 赤    -1
	if (result > 0)
	{
		topScreenConsole.bg = GREEN_COLOR;
	}
	else if (result == 0)
	{
		printf("\x1b[9;1HNot Result\n");
	}
	else
	{
		topScreenConsole.bg = RED_COLOR;
	}

	const char *msg[] = {
		"QTM Enabled",
		"QTM Disabled"};

	printf("\x1b[12;1H%s\n", msg[qtmDisabled]);

	// --------------------------------------------------

	// 描画

	while (aptMainLoop())
	{

		//
		gfxFlushBuffers();
		// 画面描画が完了するまで待機
		gspWaitForVBlank();

		// ボタンが押されるまで待機
		hidScanInput();
	}

	// --------------------------------------------------

	// 終了

	svcSleepThread(100000);
	//
	gfxExit();

	return 0;
}
