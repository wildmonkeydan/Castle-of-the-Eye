#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <libgte.h>
#include <libetc.h>
#include <libgpu.h>
#include <libpad.h>
#include <libapi.h>
#include <libspu.h>
#include <libcd.h>
#include <inline_c.h>
#include <malloc.h>
#include "gameflow.h"
#include "const.h"
#include "display.h"
#include "input.h"

SpuCommonAttr spuSettings;
static unsigned char ramAddr[0x40000];

void init();

int main() {
	init();
	while (1) {
		doGame();
		disp_Display();
	}
}

void init() {

	disp_initDisplay();

	// Initialize the GTE
	InitGeom();

	// Set GTE offset (recommended method  of centering)
	gte_SetGeomOffset(CENTERX, CENTERY);

	// Set screen depth (basically FOV control, W/2 works best)
	gte_SetGeomScreen(CENTERX);

	CdInit();
	InitHeap((u_long*)ramAddr, sizeof(ramAddr));
	SpuInit();

	// Set master & CD volume to max
	spuSettings.mask = (SPU_COMMON_MVOLL | SPU_COMMON_MVOLR | SPU_COMMON_CDVOLL | SPU_COMMON_CDVOLR | SPU_COMMON_CDMIX);
	// Master volume should be in range 0x0000 - 0x3fff
	spuSettings.mvol.left = 0x3fff;
	spuSettings.mvol.right = 0x3fff;
	// Cd volume should be in range 0x0000 - 0x7fff
	spuSettings.cd.volume.left = 0x3fff;
	spuSettings.cd.volume.right = 0x3fff;
	// Enable CD input ON
	spuSettings.cd.mix = SPU_ON;
	// Apply settings
	SpuSetCommonAttr(&spuSettings);
	// Set transfer mode 
	SpuSetTransferMode(SPU_TRANSFER_BY_DMA);

	input_Init();

	// Load font and open a text stream
	FntLoad(960, 0);
	FntOpen(0, 8, 320, 216, 0, 100);

}