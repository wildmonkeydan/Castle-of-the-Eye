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
#include <libpress.h>
#include <inline_c.h>
#include <malloc.h>
#include "input.h"
#include "strplay.c"
#include "gameflow.h"
#include "const.h"
#include "display.h"


SpuCommonAttr spuSettings;
static unsigned char ramAddr[1400000];
STRFILE StrFile[] = {
	// File name	Resolution		Frame count
	"\\COPY.STR;1", 320, 240, 893
};

void init();

int main() {
	init();
	/*while (1) {
		if (PlayStr(320, 240, 0, 0, &StrFile[0]) == 0)	// If player presses Start
			break;	// Exit the loop
	}*/
	while (1) {
		gf_doGame();
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
	EnterCriticalSection();
	InitHeap((u_long*)ramAddr, sizeof(ramAddr));
	ExitCriticalSection();
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
	FntOpen(80, 150, 320, 216, 0, 200);

}

static void strCheckRGB24() {

	/* From http://psx.arthus.net/sdk/Psy-Q/DOCS/, p.713
	 * When playing a movie in 24-bit mode, there is a potential hardware conflict between the CD subsystem
	 * and the MDEC image decompression system which can result in corrupted data. To avoid this,
	 * StCdInterrupt() may defer transferring a sector and instead set a flag variable called StCdInterFlag to
	 * indicate that a CD sector is ready to be transferred. Once the MDEC is finished transferring data, your
	 * application should check StCdIntrFlag and call StCdInterrupt() directly if it is set.
	 */
#if TRUECOL
	extern u_long StCdIntrFlag;
	// If flag was set
	if (StCdIntrFlag) {
		// Trigger data transfer
		StCdInterrupt();
		// Reset flag
		StCdIntrFlag = 0;
	}
#endif
}