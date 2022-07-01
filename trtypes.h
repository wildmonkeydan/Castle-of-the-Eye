#pragma once
#include <stdint.h>

//Basic data types

typedef struct    // 3 bytes
{
    uint8_t Red;        // Red component (0 -- darkest, 255 -- brightest)
    uint8_t Green;      // Green component (0 -- darkest, 255 -- brightest)
    uint8_t Blue;       // Blue component (0 -- darkest, 255 -- brightest)
}tr_colour;

typedef struct    // 6 bytes
{
    int16_t x;
    int16_t y;
    int16_t z;
}tr_vertex;

typedef struct     // 12 bytes
{
    uint16_t Vertices[4];
    uint16_t Texture;
}tr_face4;

typedef struct    // 12 bytes
{
    uint16_t Vertices[4];
    uint16_t Texture;
    uint16_t Effects;
}tr4_mesh_face4;

typedef struct     // 8 bytes
{
    uint16_t Vertices[3];
    uint16_t Texture;
}tr_face3;

typedef struct     // 10 bytes
{
    uint16_t Vertices[3];
    uint16_t Texture;
    uint16_t Effects;    // TR4-5 ONLY: alpha blending and environment mapping strength
}tr4_mesh_face3;




//Room types

typedef struct    // 16 bytes
{
    int32_t x;             // X-offset of room (world coordinates)
    int32_t z;             // Z-offset of room (world coordinates)
    int32_t yBottom;
    int32_t yTop;
}tr_room_info;

typedef struct   // 32 bytes
{
    uint16_t  AdjoiningRoom; // Which room this portal leads to
    tr_vertex Normal;
    tr_vertex Vertices[4];
}tr_room_portal;

typedef struct  // 8 bytes
{
    uint16_t FDindex;    // Index into FloorData[]
    uint16_t BoxIndex;   // Index into Boxes[] (-1 if none)
    uint8_t  RoomBelow;  // 255 is none
    int8_t   Floor;      // Absolute height of floor
    uint8_t  RoomAbove;  // 255 if none
    int8_t   Ceiling;    // Absolute height of ceiling
}tr_room_sector;

typedef struct   // 12 bytes
{
    tr_vertex Vertex;
    int16_t Lighting;   // Value is ignored!
    uint16_t Attributes; // A set of flags for special rendering effects
    uint16_t Colour;     // 15-bit colour
}tr3_room_vertex;

typedef struct     // (variable length)
{
    int16_t NumVertices;                   // Number of vertices in the following list
    tr3_room_vertex* Vertices; // List of vertices (relative coordinates)

    int16_t NumRectangles;                 // Number of textured rectangles
    tr_face4* Rectangles;    // List of textured rectangles

    int16_t NumTriangles;                  // Number of textured triangles
    tr_face3* Triangles;      // List of textured triangles
}tr_room_data;

typedef struct   // 20 bytes
{
    uint32_t x, y, z;    // Absolute position in world coordinates
    uint16_t Rotation;
    uint16_t Colour;     // 15-bit colour
    uint16_t Unused;     // Not used!
    uint16_t MeshID;     // Which StaticMesh item to draw
}tr3_room_staticmesh;

typedef struct   // (variable length)
{
    tr_room_info Info;           // Where the room exists, in world coordinates

    tr_room_data RoomData;       // The room mesh

    uint16_t NumPortals;                 // Number of visibility portals to other rooms
    tr_room_portal* Portals;  // List of visibility portals

    uint16_t NumZsectors;                                  // ``Width'' of sector list
    uint16_t NumXsectors;                                  // ``Height'' of sector list
    tr_room_sector* SectorList;  // List of sectors in this room

    int32_t RoomColour;    // In ARGB!

    uint16_t NumStaticMeshes;                            // Number of static meshes
    tr3_room_staticmesh* StaticMeshes;   // List of static meshes

    int16_t AlternateRoom;
    int16_t Flags;

    uint8_t WaterScheme;
    uint8_t ReverbInfo;

    uint8_t AlternateGroup;  // Replaces Filler from TR3
}tr4_room;





//Mesh types

typedef struct  // (variable length)
{
    tr_vertex Centre;
    int32_t CollRadius;

    int16_t NumVertices;           // Number of vertices in this mesh
    tr_vertex* Vertices; // List of vertices (relative coordinates)

    int16_t NumTexturedRectangles; // number of textured rectangles in this mesh
    tr4_mesh_face4* TexturedRectangles; // list of textured rectangles

    int16_t NumTexturedTriangles;  // number of textured triangles in this mesh
    tr4_mesh_face3* TexturedTriangles; // list of textured triangles
}tr4_mesh;

typedef struct  // 12 bytes
{
    int16_t MinX, MaxX, MinY, MaxY, MinZ, MaxZ;
}tr_bounding_box;

typedef struct    // 32 bytes
{
    uint32_t        ID;   // Static Mesh Identifier
    uint16_t        Mesh; // Mesh (offset into MeshPointers[])
    tr_bounding_box VisibilityBox;
    tr_bounding_box CollisionBox;
    uint16_t        Flags;
}tr_staticmesh;

typedef struct   // 18 bytes
{
    uint32_t ID;           // Type Identifier (matched in Entities[])
    uint16_t NumMeshes;    // Number of meshes in this object
    uint16_t StartingMesh; // Stating mesh (offset into MeshPointers[])
    uint32_t MeshTree;     // Offset into MeshTree[]
    uint32_t FrameOffset;  // Byte offset into Frames[] (divide by 2 for Frames[i])
    uint16_t Animation;    // Offset into Animations[]
}tr_model;

typedef struct  // 24 bytes
{
    int16_t TypeID;
    int16_t Room;
    int32_t x;
    int32_t y;
    int32_t z;
    int16_t Angle;
    int16_t Intensity1;
    int16_t OCB;        // Replaces Intensity2, see further for explanations.
    uint16_t Flags;
}tr4_entity;




//Animation Types

typedef struct  // 4 bytes
{
    uint32_t Flags;
    int32_t Offset_X;
    int32_t Offset_Y;
    int32_t Offset_Z;
}tr_meshtree_node;

typedef struct  // 40 bytes
{
    uint32_t  FrameOffset;
    uint8_t  FrameRate;
    uint8_t  FrameSize;

    uint16_t  State_ID;

    uint32_t  Speed;
    uint32_t  Accel;
    uint32_t  SpeedLateral; // New field
    uint32_t  AccelLateral; // New field

    uint16_t  FrameStart;
    uint16_t  FrameEnd;
    uint16_t  NextAnimation;
    uint16_t  NextFrame;

    uint16_t  NumStateChanges;
    uint16_t  StateChangeOffset;

    uint16_t  NumAnimCommands;
    uint16_t  AnimCommand;
}tr4_animation;

typedef struct  // 6 bytes
{
    uint16_t StateID;
    uint16_t NumAnimDispatches; // number of ranges (seems to always be 1..5)
    uint16_t AnimDispatch;      // Offset into AnimDispatches[]
}tr_state_change;

typedef struct     // 8 bytes
{
    int16_t Low;           // Lowest frame that uses this range
    int16_t High;          // Highest frame that uses this range
    int16_t NextAnimation; // Animation to dispatch to
    int16_t NextFrame;     // Frame offset to dispatch to
}tr_anim_dispatch;

typedef struct     // Variable size
{
    tr_bounding_box box; // Bounding box
    int16_t OffsetX, OffsetY, OffsetZ; // Starting offset for this model
    int16_t NumValues;
    uint16_t AngleSets[];   // Variable size
}tr_anim_frame;

typedef struct  // 2 bytes
{
    int16_t Value;
}tr_anim_command;



//NPC types

typedef struct    // 8 bytes
{
    uint8_t Zmin;          // Horizontal dimensions in sectors
    uint8_t Zmax;
    uint8_t Xmin;
    uint8_t Xmax;
    int16_t TrueFloor;     // Height value in global units
    int16_t OverlapIndex;  // Bits 0-13 is the index into Overlaps[]
}tr2_box;

typedef struct    // 20 bytes
{
    uint16_t GroundZone1_Normal;
    uint16_t GroundZone2_Normal;
    uint16_t GroundZone3_Normal;
    uint16_t GroundZone4_Normal;
    uint16_t FlyZone_Normal;
    uint16_t GroundZone1_Alternate;
    uint16_t GroundZone2_Alternate;
    uint16_t GroundZone3_Alternate;
    uint16_t GroundZone4_Alternate;
    uint16_t FlyZone_Alternate;
}tr2_zone;

typedef struct  // 4 bytes
{
    uint16_t Xcoordinate;
    uint16_t Ycoordinate;
}tr_object_texture_vert;

typedef struct  // 38 bytes
{
    uint16_t Attribute;
    uint16_t TileAndFlag;
    uint16_t NewFlags;

    tr_object_texture_vert Vertices[4]; // The four corners of the texture

    uint32_t OriginalU;
    uint32_t OriginalV;
    uint32_t Width;     // Actually width-1
    uint32_t Height;    // Actually height-1
}tr4_object_texture;



//lvl type
typedef struct
{
    uint16_t NumRooms; // number of rooms (2 bytes)
    tr4_room* Rooms; // room list (variable length)

    uint32_t NumFloorData; // number of floor data uint16_t's to follow (4 bytes)
    uint16_t* FloorData; // floor data (NumFloorData * 2 bytes)

    uint32_t NumMeshData; // number of uint16_t's of mesh data to follow (=Meshes[]) (4 bytes)
    tr4_mesh* Meshes; // note that NumMeshPointers comes AFTER Meshes[]

    uint32_t NumMeshPointers; // number of mesh pointers to follow (4 bytes)
    uint32_t* MeshPointers; // mesh pointer list (NumMeshPointers * 4 bytes)

    uint32_t NumAnimations; // number of animations to follow (4 bytes)
    tr4_animation* Animations; // animation list (NumAnimations * 40 bytes)

    uint32_t NumStateChanges; // number of state changes to follow (4 bytes)
    tr_state_change* StateChanges; // state-change list (NumStructures * 6 bytes)

    uint32_t NumAnimDispatches; // number of animation dispatches to follow (4 bytes)
    tr_anim_dispatch* AnimDispatches; // animation-dispatch list list (NumAnimDispatches * 8 bytes)

    uint32_t NumAnimCommands; // number of animation commands to follow (4 bytes)
    tr_anim_command* AnimCommands; // animation-command list (NumAnimCommands * 2 bytes)

    uint32_t NumMeshTrees; // number of MeshTrees to follow (4 bytes)
    tr_meshtree_node* MeshTrees; // MeshTree list (NumMeshTrees * 4 bytes)

    uint32_t NumFrames; // number of words of frame data to follow (4 bytes)
    uint16_t* Frames; // frame data (NumFrames * 2 bytes)

    uint32_t NumModels; // number of models to follow (4 bytes)
    tr_model* Models; // model list (NumModels * 18 bytes)

    uint32_t NumStaticMeshes; // number of StaticMesh data records to follow (4 bytes)
    tr_staticmesh* StaticMeshes; // StaticMesh data (NumStaticMesh * 32 bytes)

    uint32_t NumBoxes; // number of box data records to follow (4 bytes)
    tr2_box* Boxes; // box data (NumBoxes * 8 bytes)

    uint32_t NumOverlaps; // number of overlap records to follow (4 bytes)
    uint16_t* Overlaps; // overlap data (NumOverlaps * 2 bytes)

    int16_t* Zones; // zone data (NumBoxes * 20 bytes)

    uint32_t NumObjectTextures; // number of object textures to follow (4 bytes) (after AnimatedTextures in TR3)
    tr4_object_texture* ObjectTextures; // object texture list (NumObjectTextures * 38 bytes)

    uint32_t NumEntities; // number of entities to follow (4 bytes)
    tr4_entity* Entities; // entity list (NumEntities * 24 bytes)
}lvlData;