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

// Load a misc level data file
void testS_LoadMisc(uint8_t* data, lvlData** level);

// Load an animation from a file
tr4_animation testS_LoadAnimation(uint8_t* data, int counter);

// Load a mesh tree from a file
tr_meshtree_node testS_LoadMeshtree(uint8_t* data, int counter);

// Load a model from a file
tr_model testS_LoadModel(uint8_t* data, int counter);

// Load a staticmesh from a file
tr_staticmesh testS_LoadStatic(uint8_t* data, int counter);

// Load a Box from a file
tr2_box testS_LoadBox(uint8_t* data, int counter);

// Load an Object Texture
tr4_object_texture testS_LoadObjTex(uint8_t* data, int counter);

// Load an Entity 
tr4_entity testS_LoadEntity(uint8_t* data, int counter);