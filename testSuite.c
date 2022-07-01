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

void testS_Run() {
	if (testInit == true) {
		menu_CleanUp();

		unsigned long* geoData = cdLoad_File("\\CASTLE.LVL;1");
		testS_LoadGeo((unsigned char*)geoData, &level);
		free(geoData);

		unsigned long* meshData = cdLoad_File("\\CASTLE.MSH;1");
		testS_LoadMeshes((unsigned char*)meshData, &level);
		free(meshData);

		printf("e");
		testInit = false;
	}

	input = input_TestSuiteCam(&trot,input);

	roomCount += input.count;

	if (roomCount > 20) {
		roomCount = 20;
	}
	if (roomCount < 0) {
		roomCount = 0;
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
	gte_DrawRoom(&mtx, &pos, &rot, level->Rooms[roomCount].RoomData);



	FntPrint("X=%d Y=%d Z=%d\n",
		input.cam_pos.vx >> 12,
		input.cam_pos.vy >> 12,
		input.cam_pos.vz >> 12);
	FntPrint("RX=%d RY=%d\n",
		input.cam_rot.vx >> 12,
		input.cam_rot.vy >> 12);
	FntPrint("Room No. : %d", roomCount);
	FntFlush(-1);
}

void testS_LoadGeo(u_char* data, lvlData** level) {
	raw_int raw32;
	raw_short raw16;

	byteCounter = 0;

	byteCounter += 8;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[byteCounter]; byteCounter++;
	}
	(*level)->NumRooms = raw16.s;

	printf("NumRooms: %d", (*level)->NumRooms);

	(*level)->Rooms = malloc(sizeof(tr4_room) * (*level)->NumRooms);

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

	room.RoomData.Vertices = malloc(sizeof(tr3_room_vertex) * room.RoomData.NumVertices);

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

	room.RoomData.Rectangles = malloc(sizeof(tr_face4) * room.RoomData.NumRectangles);

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

	room.RoomData.Triangles = malloc(sizeof(tr_face3) * room.RoomData.NumTriangles);

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

	room.Portals = malloc(sizeof(tr_room_portal) * room.NumPortals);

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

	room.StaticMeshes = malloc(sizeof(tr3_room_staticmesh) * room.NumStaticMeshes);

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

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[byteCounter]; byteCounter++;
	}
	(*level)->NumMeshData = raw16.s;

	(*level)->Meshes = malloc(sizeof(tr4_mesh) * (*level)->NumMeshData);
	
	for (int cout = 0; cout < (*level)->NumRooms; cout++) {
		printf(" %d", byteCounter);
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
	raw16.s = mesh.Centre.x;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	raw16.s = mesh.Centre.y;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	raw16.s = mesh.Centre.z;

	for (int i = 0; i < 4; i++) {
		raw32.c[i] = data[counter]; counter;
	}
	raw32.i = mesh.CollRadius;

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	raw16.s = mesh.NumVertices;

	mesh.Vertices = malloc(sizeof(tr_vertex) * mesh.NumVertices);

	for (int a = 0; a < mesh.NumVertices; a++) {
		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		raw16.s = mesh.Vertices[a].x;

		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		raw16.s = mesh.Vertices[a].y;

		for (int i = 0; i < 2; i++) {
			raw16.c[i] = data[counter]; counter++;
		}
		raw16.s = mesh.Vertices[a].z;
	}

	for (int i = 0; i < 2; i++) {
		raw16.c[i] = data[counter]; counter++;
	}
	raw16.s = mesh.NumTexturedRectangles;

	mesh.NumTexturedRectangles = malloc(sizeof(tr_vertex) * mesh.NumTexturedRectangles);

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

	return mesh;
}