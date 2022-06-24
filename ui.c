#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <libgpu.h>
#include "display.h"
#include "ui.h"

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

}