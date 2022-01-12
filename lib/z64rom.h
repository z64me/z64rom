#ifndef __Z64ROM_H__
#define __Z64ROM_H__

#include "ExtLib.h"
#include "Rom.h"
#include "Dma.h"

typedef struct Rom {
	MemFile   file;
	DmaEntry* dmaTable;
	ActorOverlay* actorTable;
	VRomFile* objectTable;
	GameStateOverlay* stateTable;
} Rom;

typedef enum {
	ROM_NONE         = 0,
	ROM_DMA_TABLE    = 0x012F70,
	ROM_OBJECT_TABLE = 0xB9E6C8,
	ROM_ACTOR_TABLE  = 0xB8D440,
	ROM_STATE_TABLE  = 0xB969D0,
} RomAddress;

typedef enum {
	TABLESIZE_NONE   = 0,
	TABLESIZE_OBJECT = 402,
	TABLESIZE_SCENE  = 110,
	TABLESIZE_ACTOR  = 471,
	TABLESIZE_STATE  = 6,
	TABLESIZE_DMA    = 1548,
} TableSize;

extern char* gObjectName[];
extern char* gActorName[];
extern char* gStateName[];

#endif