#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <libgte.h>
#include <libgpu.h>
#include "input.h"
#include "ui.h"
#include "cd.h"
#include "trtypes.h"
#include "testSuite.h"
#include "menu.h"
#include "display.h"

static bool testInit = true;
static lvlData* level = { 0 };
static int byteCounter = 0;
static MATRIX mtx = { 0 };
static VECTOR tpos = { 0 };			// Translation value for matrix calculations
static SVECTOR trot = { 0 };
static VECTOR pos = { 0 };
static SVECTOR rot = { 0 };
static CAMINPUT input;
static char roomCount = 0;
static unsigned char meshCount = 0;
static char mode = 0;
static char load = 0;

void testS_Run() {
	
	if (testInit == true) {

		if (load == 0) {
			menu_CleanUp();
			TIM_IMAGE tim;
			unsigned long* texone = cdLoad_File("\\TEX0.TIM;1");
			ui_LoadTexture(texone, &tim);
			free(texone);
			unsigned long* textwo = cdLoad_File("\\TEX1.TIM;1");
			ui_LoadTexture(textwo, &tim);
			free(textwo);
			unsigned long* texthree = cdLoad_File("\\TEX2.TIM;1");
			ui_LoadTexture(texthree, &tim);
			free(texthree);
		}

		if (load == 1) {
			unsigned long* geoData = cdLoad_File("\\CASTLE.GEO;1");
			testS_LoadGeo((unsigned char*)geoData, &level);
			free(geoData);
		}

		if (load == 2) {
			unsigned long* meshData = cdLoad_File("\\CASTLE.MSH;1");
			testS_LoadMeshes((unsigned char*)meshData, &level);
			free(meshData);
		}

		if (load == 3) {
			unsigned long* lvlData = cdLoad_File("\\CASTLE.LVL;1");
			testS_LoadMisc((unsigned char*)lvlData, &level);
			free(lvlData);
			printf("e");
			testInit = false;
		}

		load++;

		TILEDATA tile;
		tile.color[0] = 255;
		tile.color[1] = 255;
		tile.color[2] = 255;
		tile.size.h = 12;
		tile.size.w = (load * 50);
		tile.size.x = 60;
		tile.size.y = 200;

		disp_DrawTile(tile);
		
	}
	else {

		input = input_TestSuiteCam(&trot, input);

		mode += input.mode;

		if (mode > 1) {
			mode = 0;
		}


		switch (mode) {
		case 0:
			roomCount += input.count;

			if (roomCount > 20) {
				roomCount = 20;
			}
			if (roomCount < 0) {
				roomCount = 0;
			}
			break;
		case 1:
			meshCount += input.count;

			if (meshCount > 1) {
				meshCount = 1;
			}
			if (meshCount < 0) {
				meshCount = 0;
			}

		}


		// Set rotation to the matrix
		RotMatrix(&trot, &mtx);

		// Divide out the fractions of camera coordinates and invert
		// the sign, so camera coordinates will line up to world
		// (or geometry) coordinates
		tpos.vx = -input.cam_pos.vx >> 12;
		tpos.vy = -input.cam_pos.vy >> 12;
		tpos.vz = -input.cam_pos.vz >> 12;

		// Divide out fractions of camera rotation
		trot.vx = input.cam_rot.vx >> 12;
		trot.vy = input.cam_rot.vy >> 12;
		trot.vz = input.cam_rot.vz >> 12;

		// Apply rotation of matrix to translation value to achieve a
		// first person perspective
		ApplyMatrixLV(&mtx, &tpos, &tpos);

		// Set translation matrix
		TransMatrix(&mtx, &tpos);
		SetRotMatrix(&mtx);
		SetTransMatrix(&mtx);

		switch (mode) {
		case 0:
			gte_DrawRoom(&mtx, &pos, &rot, level->Rooms[roomCount].RoomData);
			break;
		case 1:
			gte_DrawMesh(&mtx, &pos, &rot, level->Meshes[meshCount]);
		}





		FntPrint("X=%d Y=%d Z=%d\n",
			input.cam_pos.vx >> 12,
			input.cam_pos.vy >> 12,
			input.cam_pos.vz >> 12);
		FntPrint("RX=%d RY=%d\n",
			input.cam_rot.vx >> 12,
			input.cam_rot.vy >> 12);
		switch (mode) {
		case 0:
			FntPrint("Room No. : %d", roomCount);
			break;
		case 1:
			FntPrint("Mesh No. : %d", meshCount);
			break;
		}

		FntFlush(-1);
	}
}

void testS_LoadGeo(u_char* data, lvlData** level) {
	raw_int raw32;
	raw_short raw16;

	byteCounter = 0;

	byteCounter += 8; // Bypass Padding

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[byteCounter]; byteCounter++;
	}
	(*level)->NumRooms = raw16.s;

	printf("NumRooms: %d", (*level)->NumRooms);

	if (!(*level)->NumRooms == 0) {
		(*level)->Rooms = malloc(sizeof(tr4_room) * (*level)->NumRooms);
	}

	for (int cout = 0; cout < (*level)->NumRooms; cout++) {
		printf(" %d", byteCounter);
		tr4_room room = testS_LoadRoom(data, byteCounter);
		(*level)->Rooms[cout] = room;
		printf("\n\nROOMLOAD: Loaded room with %d portals, %d sectors and %d statics", (*level)->Rooms[cout].NumPortals, ((*level)->Rooms[cout].NumXsectors * (*level)->Rooms[cout].NumZsectors), (*level)->Rooms[cout].NumStaticMeshes);
	}
}

tr4_room testS_LoadRoom(uint8_t* data, int counter) {
	tr4_room room = { 0 };
	raw_int raw32 = { 0 };
	raw_short raw16 = { 0 };

	// Load room info

	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[counter]; counter++;
	}
	room.Info.x = (int32_t)raw32.i;

	printf(" %d %d %d %d", raw32.c[0], raw32.c[1], raw32.c[2], raw32.c[3]);

	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[counter]; counter++;
	}
	room.Info.z = raw32.i;

	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[counter]; counter++;
	}
	room.Info.yBottom = raw32.i;

	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[counter]; counter++;
	}
	room.Info.yTop = raw32.i;

	printf("\nROOMLOAD: Loaded info  %d %d  %d %d", room.Info.x, room.Info.z, room.Info.yBottom, room.Info.yTop);


	counter += 4; // Skip uneccessary size declaration


	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	room.RoomData.NumVertices = raw16.s;

	if (!room.RoomData.NumVertices == 0) {
		room.RoomData.Vertices = malloc(sizeof(tr3_room_vertex) * room.RoomData.NumVertices);
	}

	for (int a = 0; a < room.RoomData.NumVertices; a++) { // Load room verts
		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		room.RoomData.Vertices[a].Vertex.x = raw16.s;
		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		room.RoomData.Vertices[a].Vertex.y = raw16.s;
		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		room.RoomData.Vertices[a].Vertex.z = raw16.s;
		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		room.RoomData.Vertices[a].Lighting = raw16.s;
		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		room.RoomData.Vertices[a].Attributes = raw16.us;
		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		room.RoomData.Vertices[a].Colour = raw16.s;
	}

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	room.RoomData.NumRectangles = raw16.s;

	if (!room.RoomData.NumRectangles == 0) {
		room.RoomData.Rectangles = malloc(sizeof(tr_face4) * room.RoomData.NumRectangles);
	}

	for (int a = 0; a < room.RoomData.NumRectangles; a++) { // Load room quads
		for (int b = 0; b < 4; b++) {
			for (int i = 0; i < 2; i++) {
				raw16.c[i] = data[counter]; counter++;
			}
			room.RoomData.Rectangles[a].Vertices[b] = raw16.us;
		}
		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		room.RoomData.Rectangles[a].Texture = raw16.us;
	}



	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	room.RoomData.NumTriangles = raw16.s;

	if (!room.RoomData.NumTriangles == 0) {
		room.RoomData.Triangles = malloc(sizeof(tr_face3) * room.RoomData.NumTriangles);
	}

	for (int a = 0; a < room.RoomData.NumTriangles; a++) { // Load room tris
		for (int b = 0; b < 3; b++) {
			for (int i = 0; i < 2; i++) {
				raw16.c[i] = data[counter]; counter++;
			}
			room.RoomData.Triangles[a].Vertices[b] = raw16.us;
		}
		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		room.RoomData.Triangles[a].Texture = raw16.us;
	}

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	short numSprites = raw16.s;
	printf("  %d", numSprites);

	counter += numSprites * 4;

	printf("\nROOMLOAD: Loaded roomData"); printf(" %d", room.RoomData.NumVertices); printf(" %d", room.RoomData.NumRectangles); printf(" %d", room.RoomData.NumTriangles);


	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	room.NumPortals = raw16.s;

	printf(" %d %d", room.NumPortals, raw16.s);

	if (!room.NumPortals == 0) {
		room.Portals = malloc(sizeof(tr_room_portal) * room.NumPortals);
	}

	for (int a = 0; a < room.NumPortals; a++) { // Loads portals

		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}

		room.Portals[a].AdjoiningRoom = raw16.us;

		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		room.Portals[a].Normal.x = raw16.s;

		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		room.Portals[a].Normal.y = raw16.s;

		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		room.Portals[a].Normal.z = raw16.s;

		for (int b = 0; b < 4; b++) {
			for (int i = 0; i < 2; i++) {
				raw16.c[i] = data[counter]; counter++;
			}
			room.Portals[a].Vertices[b].x = raw16.s;

			for (int i = 0; i < 2; i++) {
				raw16.c[i] = data[counter]; counter++;
			}
			room.Portals[a].Vertices[b].y = raw16.s;

			for (int i = 0; i < 2; i++) {
				raw16.c[i] = data[counter]; counter++;
			}
			room.Portals[a].Vertices[b].z = raw16.s;
		}
	}

	printf("\nROOMLOAD: Loaded portals");

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	room.NumXsectors = raw16.s;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	room.NumZsectors = raw16.s;

	room.SectorList = malloc(sizeof(tr_room_sector) * (room.NumXsectors * room.NumZsectors));

	for (int a = 0; a < (room.NumXsectors * room.NumZsectors); a++) { // Loads Sectorlist
		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		room.SectorList[a].FDindex = raw16.s;

		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		room.SectorList[a].BoxIndex = raw16.s;

		room.SectorList[a].RoomBelow = data[counter]; counter++;

		room.SectorList[a].Floor = (int8_t)data[counter]; counter++;

		room.SectorList[a].RoomAbove = data[counter]; counter++;

		room.SectorList[a].Ceiling = (int8_t)data[counter]; counter++;
	}

	printf("\nROOMLOAD: Loaded sectorlist  %d %d", room.SectorList[0].FDindex, room.SectorList[0].BoxIndex);

	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[counter]; counter++;
	}
	room.RoomColour = raw32.i;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	uint16_t numLights = raw16.s;

	counter += numLights * 46; // Skip light reading

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	room.NumStaticMeshes = raw16.s;

	printf(" %d", room.NumStaticMeshes);

	if (!room.NumStaticMeshes == 0) {
		room.StaticMeshes = malloc(sizeof(tr3_room_staticmesh) * room.NumStaticMeshes);
	}

	for (int a = 0; a < room.NumStaticMeshes; a++) { // Loads staticmeshes
		for (int i = 0; i < 4; i++) {
			raw32.c[i] = data[counter]; counter++;
		}
		room.StaticMeshes[a].x = raw32.i;

		for (int i = 0; i < 4; i++) {
			raw32.c[i] = data[counter]; counter++;
		}
		room.StaticMeshes[a].y = raw32.i;

		for (int i = 0; i < 4; i++) {
			raw32.c[i] = data[counter]; counter++;
		}
		room.StaticMeshes[a].z = raw32.i;

		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		room.StaticMeshes[a].Rotation = raw16.s;

		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		room.StaticMeshes[a].Colour = raw16.s;

		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		room.StaticMeshes[a].Unused = raw16.s;

		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		room.StaticMeshes[a].MeshID = raw16.s;
	}

	printf("\nROOMLOAD: Loaded staticmeshes");

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	room.AlternateRoom = raw16.s;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	room.Flags = raw16.s;

	room.WaterScheme = data[counter]; counter++;
	room.ReverbInfo = data[counter]; counter++;
	room.AlternateGroup = data[counter]; counter++;

	byteCounter = counter;
	return room;
}

void testS_LoadMeshes(u_char* data, lvlData** level) {
	raw_int raw32;
	raw_short raw16;

	byteCounter = 0;

	byteCounter += 4; // Bypass Padding

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[byteCounter]; byteCounter++;
	}
	(*level)->NumMeshData = raw16.s;

	printf("\n%d", (*level)->NumMeshData);

	if (!(*level)->NumMeshData == 0) {
		(*level)->Meshes = malloc(sizeof(tr4_mesh) * (*level)->NumMeshData);
	}

	for (int cout = 0; cout < (*level)->NumMeshData; cout++) {
		tr4_mesh mesh = testS_LoadMesh(data, byteCounter);
		(*level)->Meshes[cout] = mesh;
	}
}

tr4_mesh testS_LoadMesh(uint8_t* data, int counter) {
	tr4_mesh mesh = { 0 };
	raw_int raw32;
	raw_short raw16;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	mesh.Centre.x = raw16.s;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	mesh.Centre.y = raw16.s;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	mesh.Centre.z = raw16.s;

	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[counter]; counter++;
	}
	mesh.CollRadius = raw32.i;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	mesh.NumVertices = raw16.s;

	if (!mesh.NumVertices == 0) {
		mesh.Vertices = malloc(sizeof(tr_vertex) * mesh.NumVertices);
	}

	for (int a = 0; a < mesh.NumVertices; a++) {
		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		mesh.Vertices[a].x = raw16.s;

		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		mesh.Vertices[a].y = raw16.s;

		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		mesh.Vertices[a].z = raw16.s;
	}

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	mesh.NumTexturedRectangles = raw16.s;

	if (!mesh.NumTexturedRectangles == 0) {
		mesh.TexturedRectangles = malloc(sizeof(tr4_mesh_face4) * mesh.NumTexturedRectangles);
	}

	for (int a = 0; a < mesh.NumTexturedRectangles; a++) {
		for (int b = 0; b < 4; b++) {
			for (int i = 0; i < 2; i++) {
				raw16.c[i] = data[counter]; counter++;
			}
			mesh.TexturedRectangles[a].Vertices[b] = raw16.s;
		}

		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		mesh.TexturedRectangles[a].Texture = raw16.s;

		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		mesh.TexturedRectangles[a].Effects = raw16.s;
	}

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	mesh.NumTexturedTriangles = raw16.s;

	if (!mesh.NumTexturedTriangles == 0) {
		mesh.TexturedTriangles = malloc(sizeof(tr4_mesh_face3) * mesh.NumTexturedTriangles);
	}

	for (int a = 0; a < mesh.NumTexturedTriangles; a++) {
		for (int b = 0; b < 3; b++) {
			for (int i = 0; i < 2; i++) {
				raw16.c[i] = data[counter]; counter++;
			}
			mesh.TexturedTriangles[a].Vertices[b] = raw16.s;
		}

		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		mesh.TexturedTriangles[a].Texture = raw16.s;

		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		mesh.TexturedTriangles[a].Effects = raw16.s;
	}

	printf("\nMESHLOAD: Loaded a mesh with %d vertices, %d tris and %d quads", mesh.NumVertices, mesh.NumTexturedTriangles, mesh.NumTexturedRectangles);

	byteCounter = counter;

	return mesh;
}

void testS_LoadMisc(uint8_t* data, lvlData** level) {
	raw_int raw32;
	raw_short raw16;

	byteCounter = 0;

	byteCounter += 4; // Bypass Padding

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[byteCounter]; byteCounter++;
	}
	(*level)->NumFloorData = raw16.s;

	if (!(*level)->NumFloorData == 0) {
		(*level)->FloorData = malloc(sizeof(short) * (*level)->NumFloorData);
	}

	printf("\nNum Floordata: %d", (*level)->NumFloorData);

	for (int cout = 0; cout < (*level)->NumFloorData; cout++) {
		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[byteCounter]; byteCounter++;
		}
		(*level)->FloorData[cout] = raw16.us;
	}
	byteCounter += 2;





	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[byteCounter]; byteCounter++;
	}
	(*level)->NumAnimations = raw32.i;

	if (!(*level)->NumAnimations == 0) {
		(*level)->Animations = malloc(sizeof(tr4_animation) * (*level)->NumAnimations);
	}

	printf("\nNum Animations: %d", (*level)->NumAnimations);

	for (int cout = 0; cout < (*level)->NumAnimations; cout++) {
		tr4_animation anim = testS_LoadAnimation(data, byteCounter);
		(*level)->Animations[cout] = anim;
	}





	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[byteCounter]; byteCounter++;
	}
	(*level)->NumStateChanges = raw32.ui;

	if (!(*level)->NumStateChanges == 0) {
		(*level)->StateChanges = malloc(sizeof(tr_state_change) * (*level)->NumStateChanges);
	}

	printf("\nNum State Changes: %d", (*level)->NumStateChanges);

	for (int cout = 0; cout < (*level)->NumStateChanges; cout++) {
		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[byteCounter]; byteCounter++;
		}
		(*level)->StateChanges[cout].StateID = raw16.us;

		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[byteCounter]; byteCounter++;
		}
		(*level)->StateChanges[cout].NumAnimDispatches = raw16.us;

		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[byteCounter]; byteCounter++;
		}
		(*level)->StateChanges[cout].AnimDispatch = raw16.us;
	}





	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[byteCounter]; byteCounter++;
	}
	(*level)->NumAnimDispatches = raw32.ui;

	if (!(*level)->NumAnimDispatches == 0) {
		(*level)->AnimDispatches = malloc(sizeof(tr_anim_dispatch) * (*level)->NumAnimDispatches);
	}

	printf("\nNum Anim Dispatches: %d", (*level)->NumAnimDispatches);

	for (int cout = 0; cout < (*level)->NumAnimDispatches; cout++) {
		short temp[4];
		for (int a = 0; a < 4; a++) {
			for (int i = 0; i < 2; i++) {
				raw16.c[i] = data[byteCounter]; byteCounter++;
			}
			temp[a] = raw16.s;
		}
		(*level)->AnimDispatches[cout].Low = temp[0];
		(*level)->AnimDispatches[cout].High = temp[1];
		(*level)->AnimDispatches[cout].NextAnimation = temp[2];
		(*level)->AnimDispatches[cout].NextFrame = temp[3];
	}







	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[byteCounter]; byteCounter++;
	}
	(*level)->NumAnimCommands = raw32.i;

	if (!(*level)->NumAnimCommands == 0) {
		(*level)->AnimCommands = malloc(sizeof(tr_anim_command) * (*level)->NumAnimCommands);
	}

	printf("\nNum Anim Commands: %d", (*level)->NumAnimCommands);

	for (int cout = 0; cout < (*level)->NumAnimCommands; cout++) {
		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[byteCounter]; byteCounter++;
		}
		(*level)->AnimCommands[cout].Value = raw16.s;
	}





	
	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[byteCounter]; byteCounter++;
	}
	(*level)->NumMeshTrees = raw32.i;

	if (!(*level)->NumMeshTrees == 0) {
		(*level)->MeshTrees = malloc(sizeof(tr_meshtree_node) * (*level)->NumMeshTrees);
	}

	printf("\nNum Meshtrees: %d", (*level)->NumMeshTrees);

	for (int cout = 0; cout < (*level)->NumMeshTrees; cout++) {
		(*level)->MeshTrees[cout] = testS_LoadMeshtree(data, byteCounter);
	}





	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[byteCounter]; byteCounter++;
	}
	(*level)->NumFrames = raw32.i;

	if (!(*level)->NumFrames == 0) {
		(*level)->Frames = malloc(sizeof(short) * (*level)->NumFrames);
	}

	printf("\nNum Frames: %d", (*level)->NumFrames);

	for (int cout = 0; cout < (*level)->NumAnimCommands; cout++) {
		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[byteCounter]; byteCounter++;
		}
		(*level)->Frames[cout] = raw16.us;
	}





	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[byteCounter]; byteCounter++;
	}
	(*level)->NumModels = raw32.i;

	if (!(*level)->NumModels == 0) {
		(*level)->Models = malloc(sizeof(tr_model) * (*level)->NumModels);
	}

	printf("\nNum Models: %d", (*level)->NumModels);

	for (int cout = 0; cout < (*level)->NumModels; cout++) {
		(*level)->Models[cout] = testS_LoadModel(data, byteCounter);
	}




	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[byteCounter]; byteCounter++;
	}
	(*level)->NumStaticMeshes = raw32.i;

	if (!(*level)->NumStaticMeshes == 0) {
		(*level)->StaticMeshes = malloc(sizeof(tr_staticmesh) * (*level)->NumStaticMeshes);
	}

	printf("\nNum Staticmeshes: %d", (*level)->NumStaticMeshes);

	for (int cout = 0; cout < (*level)->NumStaticMeshes; cout++) {
		(*level)->StaticMeshes[cout] = testS_LoadStatic(data, byteCounter);
	}


	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[byteCounter]; byteCounter++;
	}
	(*level)->NumBoxes = raw32.i;

	if (!(*level)->NumBoxes == 0) {
		(*level)->Boxes = malloc(sizeof(tr2_box) * (*level)->NumBoxes);
	}

	printf("\nNum Boxes: %d", (*level)->NumBoxes);

	for (int cout = 0; cout < (*level)->NumBoxes; cout++) {
		(*level)->Boxes[cout] = testS_LoadBox(data, byteCounter);
	}





	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[byteCounter]; byteCounter++;
	}
	(*level)->NumOverlaps = raw32.i;

	if (!(*level)->NumOverlaps == 0) {
		(*level)->Overlaps = malloc(sizeof(short) * (*level)->NumOverlaps);
	}

	printf("\nNum Overlaps: %d", (*level)->NumOverlaps);

	for (int cout = 0; cout < (*level)->NumOverlaps; cout++) {
		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[byteCounter]; byteCounter++;
		}
		(*level)->Overlaps[cout] = raw16.s;
	}





	int NumZones = ((*level)->NumBoxes * 10);

	if (!NumZones == 0) {
		(*level)->Zones = malloc(sizeof(short) * NumZones);
	}

	printf("\nNum Zones: %d", NumZones);

	for (int cout = 0; cout < NumZones; cout++) {
		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[byteCounter]; byteCounter++;
		}
		(*level)->Zones[cout] = raw16.s;
	}





	byteCounter += 4;
	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[byteCounter]; byteCounter++;
	}
	(*level)->NumObjectTextures = raw32.i;

	printf("\nNum Object Textures: %d", (*level)->NumObjectTextures);

	if (!(*level)->NumObjectTextures == 0) {
		(*level)->ObjectTextures = malloc(sizeof(tr4_object_texture) * (*level)->NumObjectTextures);
	}

	for (int cout = 0; cout < (*level)->NumObjectTextures; cout++) {
		(*level)->ObjectTextures[cout] = testS_LoadObjTex(data, byteCounter);
	}





	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[byteCounter]; byteCounter++;
	}
	(*level)->NumEntities = raw32.i;

	if (!(*level)->NumEntities == 0) {
		(*level)->Entities = malloc(sizeof(tr4_entity) * (*level)->NumEntities);
	}

	for (int cout = 0; cout < (*level)->NumEntities; cout++) {
		(*level)->Entities[cout] = testS_LoadEntity(data, byteCounter);
	}
	
}

tr4_animation testS_LoadAnimation(uint8_t* data, int counter) {
	raw_int raw32;
	raw_short raw16;
	tr4_animation anim;

	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[counter]; counter++;
	}
	anim.FrameOffset = raw32.ui;

	anim.FrameRate = data[counter]; counter++;
	anim.FrameSize = data[counter]; counter++;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	anim.State_ID = raw16.s;



	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[counter]; counter++;
	}
	anim.Speed = raw32.i;

	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[counter]; counter++;
	}
	anim.Accel = raw32.i;

	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[counter]; counter++;
	}
	anim.SpeedLateral = raw32.i;

	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[counter]; counter++;
	}
	anim.AccelLateral = raw32.i;


	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	anim.FrameStart = raw16.s;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	anim.FrameEnd = raw16.s;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	anim.NextAnimation = raw16.s;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	anim.NextFrame = raw16.s;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	anim.NumStateChanges = raw16.s;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	anim.StateChangeOffset = raw16.s;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	anim.NumAnimCommands = raw16.s;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	anim.AnimCommand = raw16.s;

	byteCounter = counter;

	return anim;
}

tr_meshtree_node testS_LoadMeshtree(uint8_t* data, int counter) {
	raw_int raw32;
	raw_short raw16;
	tr_meshtree_node node;

	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[counter]; counter++;
	}
	node.Flags = raw32.ui;

	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[counter]; counter++;
	}
	node.Offset_X = raw32.i;

	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[counter]; counter++;
	}
	node.Offset_Y = raw32.i;

	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[counter]; counter++;
	}
	node.Offset_Z = raw32.i;

	byteCounter = counter;

	return node;
}

tr_model testS_LoadModel(uint8_t* data, int counter) {
	raw_int raw32;
	raw_short raw16;
	tr_model model;

	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[counter]; counter++;
	}
	model.ID = raw32.i;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	model.NumMeshes = raw16.s;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	model.StartingMesh = raw16.s;

	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[counter]; counter++;
	}
	model.MeshTree = raw32.i;

	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[counter]; counter++;
	}
	model.FrameOffset = raw32.i;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	model.Animation = raw16.s;

	byteCounter = counter;

	return model;
}

tr_staticmesh testS_LoadStatic(uint8_t* data, int counter) {
	raw_short raw16;
	raw_int raw32;
	tr_staticmesh mesh;

	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[counter]; counter++;
	}
	mesh.ID = raw32.i;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	mesh.Mesh = raw16.s;

	short boundingBox[6];
	for (int a = 0; a < 6; a++) {
		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		boundingBox[a] = raw16.s;
	}
	mesh.VisibilityBox.MinX = boundingBox[0];
	mesh.VisibilityBox.MaxX = boundingBox[1];
	mesh.VisibilityBox.MinY = boundingBox[2];
	mesh.VisibilityBox.MaxY = boundingBox[3];
	mesh.VisibilityBox.MinZ = boundingBox[4];
	mesh.VisibilityBox.MaxZ = boundingBox[5];

	for (int a = 0; a < 6; a++) {
		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		boundingBox[a] = raw16.s;
	}
	mesh.CollisionBox.MinX = boundingBox[0];
	mesh.CollisionBox.MaxX = boundingBox[1];
	mesh.CollisionBox.MinY = boundingBox[2];
	mesh.CollisionBox.MaxY = boundingBox[3];
	mesh.CollisionBox.MinZ = boundingBox[4];
	mesh.CollisionBox.MaxZ = boundingBox[5];

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	mesh.Flags = raw16.us;

	byteCounter = counter;

	return mesh;
}

tr2_box testS_LoadBox(uint8_t* data, int counter) {
	raw_int raw32;
	raw_short raw16;
	tr2_box box;

	box.Zmin = data[counter]; counter++;
	box.Zmax = data[counter]; counter++;
	box.Xmin = data[counter]; counter++;
	box.Xmax = data[counter]; counter++;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	box.TrueFloor = raw16.s;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	box.OverlapIndex = raw16.s;

	byteCounter = counter;

	return box;
}

tr4_object_texture testS_LoadObjTex(uint8_t* data, int counter) {
	raw_int raw32;
	raw_short raw16;
	tr4_object_texture tex;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	tex.Attribute = raw16.s;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	tex.TileAndFlag = raw16.s;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	tex.NewFlags = raw16.s;

	printf("\nProcess UV");

	for (int a = 0; a < 4; a++) {
		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		tex.Vertices[a].Xcoordinate = raw16.s;

		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		tex.Vertices[a].Ycoordinate = raw16.s;
	}

	counter += 8;

	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[counter]; counter++;
	}
	tex.Width = raw32.ui;

	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[counter]; counter++;
	}
	tex.Height = raw32.ui;

	byteCounter = counter;

	return tex;
}

tr4_entity testS_LoadEntity(uint8_t* data, int counter) {
	raw_int raw32;
	raw_short raw16;
	tr4_entity entity;
	
	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	entity.TypeID = raw16.s;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	entity.Room = raw16.s;

	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[counter]; counter++;
	}
	entity.x = raw32.i;

	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[counter]; counter++;
	}
	entity.y = raw32.i;

	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[counter]; counter++;
	}
	entity.z = raw32.i;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	entity.Angle = raw16.s;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	entity.Intensity1 = raw16.s;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	entity.OCB = raw16.s;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	entity.Flags = raw16.us;

	byteCounter = counter;

	return entity;
}