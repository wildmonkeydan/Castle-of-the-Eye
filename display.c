#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <libgpu.h>
#include <libetc.h>
#include "display.h"
#include "const.h"

void disp_initDisplay() {
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
}

void disp_Display() {
	// Wait for GPU to finish drawing and vertical retrace
	DrawSync(0);
	VSync(0);

	// Swap buffers
	db_active ^= 1;
	db_nextpri = db[db_active].p;

	// Clear the OT of the next frame
	ClearOTagR(db[db_active].ot, OT_LEN);

	// Apply display/drawing environments
	PutDrawEnv(&db[db_active].draw);
	PutDispEnv(&db[db_active].disp);

	// Enable display
	SetDispMask(1);

	// Start drawing the OT of the last buffer
	DrawOTag(db[1 - db_active].ot + (OT_LEN - 1));
}