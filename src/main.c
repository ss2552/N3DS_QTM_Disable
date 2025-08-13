#include <3ds/types.h>
#include <3ds/console.h>
#include <3ds/gfx.h>
#include <3ds/svc.h>
#include <3ds/services/hid.h>
#include <3ds/services/apt.h>
#include <3ds/services/ptmsysm.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void rpDoQTMPatchAndToggle(void);

// https://rgbcolorpicker.com/0-1
#define BLACK_COLOR 0b0000
#define GREEN_COLOR 0b0010
#define MAGENTA_COLOR 0b0101
#define WHITE_COLOR 0b1111

// ? b g r
// 0b0101 紫色
// 0b1010 緑

// 結果のリザルト
int result = 0;
// 有効又は無効か
int qtmDisabled = 0;
//
PrintConsole topScreenConsole;

// https://github.com/devkitPro/3ds-examples/tree/master/qtm

int main(void)
{
	//
	bool isN3DS = true;
	//
	bool skip = false;

	// --------------------------------------------------

	// デフォルトでグラフィック初期化
	gfxInitDefault();
	// コンソールの初期化
	consoleInit(GFX_TOP, &topScreenConsole);

	// 背景を白色に設定 文字色を黒に設定
	topScreenConsole.bg = WHITE_COLOR;
	topScreenConsole.fg = BLACK_COLOR;

	// 画面を空白埋め
	consoleClear();

	// --------------------------------------------------

	// new 3dsか確認
	// PTM:CheckNew3DS https://www.3dbrew.org/wiki/PTM:CheckNew3DS
	// Result res = PTMSYSM_CheckNew3DS(&isN3DS);
	// printf("\x1b[10;10H: new 3ds check: %lu", res);
	// {
	// 	printf("\x1b[3;10Hn3ds ka wakaranai");
	// 	skip = true;
	// 	result = -1;
	// }
	// else
	if (isN3DS != true)
	{
		printf("\x1b[8;10HN3ds nomi");
		skip = true;
		result = -1;
	}

	// QTMを有効または無効化
	if (!skip)
		rpDoQTMPatchAndToggle();

	// 結果
	// --------------------------------------------------
	// 初期 : 白  null
	// 成功 : 緑     1
	// 失敗 : 赤    -1
	if (result == 0)
	{
		printf("\x1b[6;10Hkekka nashi");
	}
	else if (result > 0)
	{
		topScreenConsole.bg = GREEN_COLOR;
	}
	else
	{
		topScreenConsole.bg = MAGENTA_COLOR;
	}

	if (skip)
		goto skip_point;

	const char *msg[] = {
		"mukou ka",
		"yuukou ka"};

	printf("\x1b[8;10HQTM wo %s", msg[qtmDisabled]);

skip_point:
	// --------------------------------------------------

	// 描画

	while (aptMainLoop())
	{

		// ボタンが押されるまで待機
		hidScanInput();

		if (hidKeysDown())
			break;

		//
		gfxFlushBuffers();
		// 画面描画が完了するまで待機
		gspWaitForVBlank();
	}

	// --------------------------------------------------

	// 終了

	// svcSleepThread(100000);
	//
	gfxExit();

	return 0;
}
