#pragma once
#include "trtypes.h"

typedef union {
    unsigned char c[4];
    int i;
    unsigned int ui;
} raw_int;

typedef union {
    unsigned char c[2];
    short s;
    unsigned short us;
} raw_short;

// Run the test suite
void testS_Run();

// Load a level geometry file
void testS_LoadGeo(u_char* data, lvlData** level);

// Load a room from a file
tr4_room testS_LoadRoom(uint8_t* data, int counter);

// Load a meshdata file
void testS_LoadMeshes(u_char* data, lvlData** level);

// Load a room from a file
tr4_mesh testS_LoadMesh(uint8_t* data, int counter);