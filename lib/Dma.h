#ifndef __Z64_DMA_H__
#define __Z64_DMA_H__

#include "ExtLib.h"

typedef struct {
	/* 0x00 */ void32 loadedRamAddr;
	/* 0x04 */ u32    vromStart; // if applicable
	/* 0x08 */ u32    vromEnd; // if applicable
	/* 0x0C */ void32 vramStart; // if applicable
	/* 0x10 */ void32 vramEnd; // if applicable
	/* 0x14 */ void32 unk_14;
	/* 0x18 */ void32 init; // initializes and executes the given context
	/* 0x1C */ void32 destroy; // deconstructs the context, and sets the next context to load
	/* 0x20 */ void32 unk_20;
	/* 0x24 */ void32 unk_24;
	/* 0x28 */ s32    unk_28;
	/* 0x2C */ u32    instanceSize;
} GameStateOverlay; // size = 0x30

typedef struct {
	s16   id;
	u8    category;
	u32   flags;
	s16   objectId;
	u32   instanceSize;
	void* init;
	void* destroy;
	void* update;
	void* draw;
} ActorInit;

typedef struct {
	u32    vromStart;
	u32    vromEnd;
	void32 vramStart;
	void32 vramEnd;
	void32 loadedRamAddr;
	void32 initInfo;
	void32 name;
	u16    allocType;
	s8 numLoaded;
} ActorOverlay;

typedef struct {
	u32 vromStart;
	u32 vromEnd;
	u32 romStart;
	u32 romEnd;
} DmaEntry;

typedef struct {
	u32 vromStart;
	u32 vromEnd;
} VRomFile;

typedef struct {
	union {
		void* data;
		PointerCast cast;
	};
	u32 romStart;
	u32 romEnd;
	u32 size;
} RomFile;

struct Rom;

#define Dma_RomFile_Proto(type, PART, name) \
	Dma_RomFile_Get ## name(struct Rom* rom, s32 id)

RomFile Dma_RomFile_Proto(VRomFile, objectTable, Object);
RomFile Dma_RomFile_Proto(ActorOverlay, actorTable, Actor);
RomFile Dma_RomFile_Proto(DmaEntry, dmaTable, DmaEntry);
RomFile Dma_RomFile_Proto(GameStateOverlay, stateTable, State);

#endif