#include <3ds/services/ptmsysm.h>
#include <3ds/console.h>
#include <3ds/gfx.h>
#include <3ds/types.h>
#include <3ds/services/hid.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void rpDoQTMPatchAndToggle(void);

// https://rgbcolorpicker.com/0-1
#define BLACK_COLOR 0
#define GREEN_COLOR 2
#define RED_COLOR 4
#define WHITE_COLOR 15

// 結果のリザルト
int result = 0;
// 有効又は無効か
bool qtmDisabled = false;
//
PrintConsole topScreenConsole;

// https://github.com/devkitPro/3ds-examples/tree/master/qtm

int main(void)
{

	bool skip = false;
	bool is_n3ds = false;

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
	PTMSYSM_CheckNew3DS(&is_n3ds);
	if (is_n3ds != 1)
	{
		printf("\x1b[16;20HN3ds専用¥n");
		skip = true;
	}

	hidScanInput();

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
		printf("\x1b[16;20H結果無し\n");
	}
	else
	{
		topScreenConsole.bg = RED_COLOR;
	}

	const char *msg[] = {
		"QTMを有効化",
		"QTMを無効化"};

	printf("\x1b[16;20H・%s\n", msg[qtmDisabled]);

	// --------------------------------------------------

	// 描画

	//
	gfxFlushBuffers();
	// 画面描画が完了するまで待機
	gspWaitForVBlank();
	// ボタンが押されるまで待機
	hidScanInput();

	// --------------------------------------------------

	// 終了

	//
	gfxExit();

	return 0;
}
