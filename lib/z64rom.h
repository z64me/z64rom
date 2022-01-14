#ifndef __Z64ROM_H__
#define __Z64ROM_H__

#include "ExtLib.h"
#include "Rom.h"
#include "Dma.h"
#include "z64audio.h"

typedef struct Rom {
	MemFile         file;
	DmaEntry*       dmaTable;
	ActorEntry*     actorTable;
	SceneEntry*     sceneTable;
	ObjectEntry*    objectTable;
	KaleidoEntry*   kaleidoTable;
	GameStateEntry* stateTable;
} Rom;

typedef struct SampleInfo {
	u32   sampleRate;
	u8    halfPrec;
	struct SampleInfo* dublicate;
	char* name;
} SampleInfo;

typedef struct IndexInfo {
	u32  romAddress;
	char file[256 * 2];
} IndexInfo;

extern u32 gDmaDataTable;
extern u32 gObjectTable;
extern u32 gActorOverlayTable;
extern u32 gGameStateOverlayTable;

extern u32 gSoundFontTable;
extern u32 gSequenceFontTable;
extern u32 gSequenceTable;
extern u32 gSampleBankTable;

extern u32 OBJECT_ID_MAX;
extern u32 SCENE_ID_MAX;
extern u32 ACTOR_ID_MAX;
extern u32 GAMESTATE_ID_MAX;
extern u32 DMA_ID_MAX;

extern char* gObjectName[];
extern char* gActorName[];
extern char* gStateName[];
extern char* gSceneName[];
extern char* gBankName[];
extern char* gSequenceName[];
extern SampleInfo gSampleInfo[];

#endif