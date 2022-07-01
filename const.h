#pragma once
#include <libgte.h>
#include <libgpu.h>


// Region definition
#define REGION          0
#define CD_SECTOR_SIZE  2048
#define TRUECOL			1

 // OT and Packet Buffer sizes
#define OT_LEN			4094
#define PACKET_LEN		160768

// Screen resolution
#define SCREEN_XRES		320
#define SCREEN_YRES		240

// Screen center position
#define CENTERX			SCREEN_XRES>>1
#define CENTERY			SCREEN_YRES>>1

#define SPU_CD_VOL_L *((volatile uint16_t *) (0x1f801db0)) 
#define SPU_CD_VOL_R *((volatile uint16_t *) (0x1f801db2))

#define BtoS(len) ( ( len + CD_SECTOR_SIZE - 1 ) / CD_SECTOR_SIZE )

static RECT	screen_clip;