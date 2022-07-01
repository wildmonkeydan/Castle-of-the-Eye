#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <libgte.h>
#include <libgpu.h>
#include <libetc.h>
#include "display.h"
#include "ui.h"
#include "const.h"
#include "trtypes.h"
#include "clip.h"

// Double buffer structure
typedef struct {
	DISPENV	disp;			// Display environment
	DRAWENV	draw;			// Drawing environment
	u_long 	ot[OT_LEN];		// Ordering table
	char 	p[PACKET_LEN];	// Packet buffer
} DB;

// Double buffer variables
static DB	db[2];
static int	db_active = 0;
static char* db_nextpri;

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
	//db[0].disp.isrgb24 = 1;


	// Define the second set of display/draw environments
	SetDefDispEnv(&db[1].disp, SCREEN_XRES, 0, SCREEN_XRES, SCREEN_YRES);
	SetDefDrawEnv(&db[1].draw, 0, 0, SCREEN_XRES, SCREEN_YRES);

	setRGB0(&db[1].draw, 0, 0, 0);
	db[1].draw.isbg = 1;
	db[1].draw.dtd = 1;
	//db[1].disp.isrgb24 = 1;

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

void disp_SetDrawTPage(SPRTDATA data) {
	DR_TPAGE* tprit = (DR_TPAGE*)db_nextpri;

	setDrawTPage(tprit, 0, 1, getTPage(data.mode & 0x3, 0, data.prect.x, data.prect.y));
	addPrim(&db[db_active].ot, tprit);
	tprit++;

	db_nextpri += sizeof(DR_TPAGE);
}

void disp_DrawSprite(SPRTDATA data, uint8_t color[3]) {
	SPRT* sprt = (SPRT*)db_nextpri;

	setSprt(sprt);
	setWH(sprt, data.w, data.h);                  // Initialize the primitive (very important)
	setXY0(sprt, data.x, data.y);           // Position the sprite at (48,48)
	setUV0(sprt,                    // Set UV coordinates
		data.u,
		data.v);
	setClut(sprt,                   // Set CLUT coordinates to sprite
		data.crect.x,
		data.crect.y);
	setRGB0(sprt,                   // Set primitive color
		color[0], color[1], color[2]);

	addPrim(&db[db_active].ot, sprt);
	sprt++;
	db_nextpri += sizeof(SPRT);
}

void disp_DrawSprite8(SPRTDATA data) {

}

void disp_DrawBack(u_long* tim, TIM_IMAGE* tparam) {
	OpenTIM(tim);
	ReadTIM(tparam);

	RECT pos;
	pos.h = 240;
	pos.w = 320;
	pos.x = db[db_active].disp.disp.x;
	pos.y = 0;

	// Upload pixel data to framebuffer
	LoadImage(&pos, tparam->paddr);
}

void disp_DrawTile(TILEDATA data) {
	TILE* tile = (TILE*)db_nextpri;

	setTile(tile);
	setWH(tile, data.size.w, data.size.h);
	setXY0(tile, data.size.x, data.size.y);
	setRGB0(tile, data.color[0], data.color[1], data.color[2]);

	addPrim(&db[db_active].ot, tile);
	tile++;
	db_nextpri += sizeof(TILE);
}

void gte_DrawRoom(MATRIX* mtx, VECTOR* pos, SVECTOR* rot, tr_room_data room) {

	// Object and light matrix for object
	MATRIX omtx;

	// Set object rotation and position
	RotMatrix(rot, &omtx);
	TransMatrix(&omtx, pos);

	// Composite coordinate matrix transform, so object will be rotated and
	// positioned relative to camera matrix (mtx), so it'll appear as
	// world-space relative.
	CompMatrixLV(mtx, &omtx, &omtx);

	// Save matrix
	PushMatrix();

	// Set matrices
	SetRotMatrix(&omtx);
	SetTransMatrix(&omtx);

	POLY_F4* poly = (POLY_F4*)db_nextpri;

	for (int i = 0; i < room.NumRectangles; i++) {
		
		long OTz, p, Flag;

		SVECTOR v0 = (SVECTOR){ room.Vertices[room.Rectangles[i].Vertices[0]].Vertex.x, room.Vertices[room.Rectangles[i].Vertices[0]].Vertex.y, room.Vertices[room.Rectangles[i].Vertices[0]].Vertex.z, 0 };
		SVECTOR v1 = (SVECTOR){ room.Vertices[room.Rectangles[i].Vertices[1]].Vertex.x, room.Vertices[room.Rectangles[i].Vertices[1]].Vertex.y, room.Vertices[room.Rectangles[i].Vertices[1]].Vertex.z, 0 };
		SVECTOR v2 = (SVECTOR){ room.Vertices[room.Rectangles[i].Vertices[3]].Vertex.x, room.Vertices[room.Rectangles[i].Vertices[3]].Vertex.y, room.Vertices[room.Rectangles[i].Vertices[3]].Vertex.z, 0 };
		SVECTOR v3 = (SVECTOR){ room.Vertices[room.Rectangles[i].Vertices[2]].Vertex.x, room.Vertices[room.Rectangles[i].Vertices[2]].Vertex.y, room.Vertices[room.Rectangles[i].Vertices[2]].Vertex.z, 0 };

		long nclip = RotAverageNclip4(&v0, &v1, &v2, &v3, (long*)&poly->x0, (long*)&poly->x1, (long*)&poly->x2, (long*)&poly->x3, &p, &OTz, &Flag);

		if (nclip > 0 && OTz > 0 && quad_clip(&screen_clip, (DVECTOR*)&poly->x0, (DVECTOR*)&poly->x1, (DVECTOR*)&poly->x2, (DVECTOR*)&poly->x3) == 0) {

			setPolyF4(poly);
			setRGB0(poly, 205, 64, 50);
			

			addPrim(&db[db_active].ot, poly);
			poly++;
			db_nextpri += sizeof(POLY_F4);
		}
		
	}

	POLY_F3* tri = (POLY_F3*)db_nextpri;

	for (int i = 0; i < room.NumTriangles; i++) {

		long OTz, p, Flag;

		SVECTOR v0 = (SVECTOR){ room.Vertices[room.Triangles[i].Vertices[0]].Vertex.x, room.Vertices[room.Triangles[i].Vertices[0]].Vertex.y, room.Vertices[room.Triangles[i].Vertices[0]].Vertex.z, 0 };
		SVECTOR v1 = (SVECTOR){ room.Vertices[room.Triangles[i].Vertices[1]].Vertex.x, room.Vertices[room.Triangles[i].Vertices[1]].Vertex.y, room.Vertices[room.Triangles[i].Vertices[1]].Vertex.z, 0 };
		SVECTOR v2 = (SVECTOR){ room.Vertices[room.Triangles[i].Vertices[2]].Vertex.x, room.Vertices[room.Triangles[i].Vertices[2]].Vertex.y, room.Vertices[room.Triangles[i].Vertices[2]].Vertex.z, 0 };

		long nclip = RotAverageNclip3(&v0, &v1, &v2, (long*)&tri->x0, (long*)&tri->x1, (long*)&tri->x2, &p, &OTz, &Flag);

		if (nclip > 0 && OTz > 0 && tri_clip(&screen_clip, (DVECTOR*)&tri->x0, (DVECTOR*)&tri->x1, (DVECTOR*)&tri->x2) == 0) {

			setPolyF3(tri);
			setRGB0(tri, 128, 255, 128);

			addPrim(&db[db_active].ot, tri);
			tri++;
			db_nextpri += sizeof(POLY_F3);
		}
		
	}
	PopMatrix();
}