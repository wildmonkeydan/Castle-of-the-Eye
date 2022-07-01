#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <libgte.h>
#include <libgpu.h>
#include "ui.h"
#include "display.h"
#include "padEx.h"


void ui_LoadTexture(u_long* tim, TIM_IMAGE* tparam) {
	OpenTIM(tim);
	ReadTIM(tparam);

	// Upload pixel data to framebuffer
	LoadImage(tparam->prect, tparam->paddr);
	DrawSync(0);

	// Upload CLUT to framebuffer
	if (tparam->mode & 0x8) {
		LoadImage(tparam->crect, tparam->caddr);
		DrawSync(0);
	}
}

SPRTDATA ui_LoadSpriteData(TIM_IMAGE* tparam) {
	SPRTDATA data = { 0 };
	data.prect = *tparam->prect;
	data.crect = *tparam->crect;
	data.mode = tparam->mode;
	return data;
}

void ui_DrawSprite(SPRTDATA data, uint8_t color[3]) {
	disp_DrawSprite(data, color);
}

void ui_SetTPage(SPRTDATA data) {
	disp_SetDrawTPage(data);
}

void ui_DrawButton(int button, int x, int y) {
	SPRTDATA sprt;
	sprt.x = x;
	sprt.y = y;

	switch (button) {
	case PAD_CROSS:
		sprt.u = 128;
		sprt.v = 0;
		break;
	case PAD_SQUARE:
		sprt.u = 136;
		sprt.v = 0;
		break;
	case PAD_CIRCLE:
		sprt.u = 128;
		sprt.v = 8;
		break;
	case PAD_TRIANGLE:
		sprt.u = 136;
		sprt.v = 8;
		break;
	case PAD_LEFT:
		sprt.u = 128;
		sprt.v = 16;
		break;
	case PAD_RIGHT:
		sprt.u = 136;
		sprt.v = 16;
		break;
	case PAD_UP:
		sprt.u = 128;
		sprt.v = 24;
		break;
	case PAD_DOWN:
		sprt.u = 136;
		sprt.v = 24;
		break;
	}

	disp_DrawSprite8(sprt);

	SPRTDATA tpage;
	tpage.x = 960;
	tpage.y = 0;
	tpage.mode = 0;

	disp_SetDrawTPage(tpage);
}