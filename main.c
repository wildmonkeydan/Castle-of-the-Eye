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


// Double buffer structure
typedef struct {
	DISPENV	disp;			// Display environment
	DRAWENV	draw;			// Drawing environment
	u_long 	ot[OT_LEN];		// Ordering table
	char 	p[PACKET_LEN];	// Packet buffer
} DB;

// Double buffer variables
DB		db[2];
int		db_active = 0;
char* db_nextpri;
RECT	screen_clip;

// Pad data buffer
char pad_buff[2][34];
SpuCommonAttr spuSettings;
static unsigned char ramAddr[0x40000];

void init();

int main() {
	init();
	while (1) {
		doGame();
	}
}

void init() {

	// Reset the GPU, also installs a VSync event handler
	ResetGraph(0);

	SetVideoMode(REGION);



	// Set display and draw environment areas
	// (display and draw areas must be separate, otherwise hello flicker)
	SetDefDispEnv(&db[0].disp, 0, 0, SCREEN_XRES, SCREEN_YRES);
	SetDefDrawEnv(&db[0].draw, SCREEN_XRES, 0, SCREEN_XRES, SCREEN_YRES);

	// Enable draw area clear and dither processing
	setRGB0(&db[0].draw, 0, 0, 0);
	db[0].draw.isbg = 1;
	db[0].draw.dtd = 1;


	// Define the second set of display/draw environments
	SetDefDispEnv(&db[1].disp, SCREEN_XRES, 0, SCREEN_XRES, SCREEN_YRES);
	SetDefDrawEnv(&db[1].draw, 0, 0, SCREEN_XRES, SCREEN_YRES);

	setRGB0(&db[1].draw, 0, 0, 0);
	db[1].draw.isbg = 1;
	db[1].draw.dtd = 1;

	SetDispMask(1);

	// Apply the drawing environment of the first double buffer
	PutDispEnv(&db[0].disp);
	PutDrawEnv(&db[0].draw);

	// Clear both ordering tables to make sure they are clean at the start
	ClearOTagR(db[0].ot, OT_LEN);
	ClearOTagR(db[1].ot, OT_LEN);

	// Set primitive pointer address
	db_nextpri = db[0].p;

	// Set clip region
	setRECT(&screen_clip, 0, 0, SCREEN_XRES, SCREEN_YRES);


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


	// Init BIOS pad driver and set pad buffers (buffers are updated
	// automatically on every V-Blank)
	InitPAD(&pad_buff[0][0], 34, &pad_buff[1][0], 34);

	// Start pad
	StartPAD();

	// Don't make pad driver acknowledge V-Blank IRQ (recommended)
	ChangeClearPAD(0);

	// Load font and open a text stream
	FntLoad(960, 0);
	FntOpen(0, 8, 320, 216, 0, 100);

}