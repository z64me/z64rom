#ifndef __Z64ROM_H__
#define __Z64ROM_H__

#include "ExtLib.h"
#include "Rom.h"
#include "Dma.h"
#include "z64audio.h"

typedef struct {
	struct {
		u32 dmaTable;
		u32 objTable;
		u32 actorTable;
		u32 stateTable;
		u32 sceneTable;
		u32 kaleidoTable;
		
		u32 seqFontTbl;
		u32 seqTable;
		u32 fontTable;
		u32 sampleTable;
	} table;
	struct {
		u32 seqRom;
		u32 fontRom;
		u32 smplRom;
	} segment;
	struct {
		u16 dma;
		u16 obj;
		u16 actor;
		u16 state;
		u16 scene;
		u16 kaleido;
	} tblNum;
} RomOffset;

typedef struct Rom {
	MemFile         file;
	RomOffset       addr;
	DmaEntry*       dmaTable;
	ActorEntry*     actorTable;
	SceneEntry*     sceneTable;
	ObjectEntry*    objectTable;
	KaleidoEntry*   kaleidoTable;
	GameStateEntry* stateTable;
} Rom;

typedef struct N64AudioInfo {
	u32   sampleRate;
	u8    halfPrec;
	const struct N64AudioInfo* dublicate;
	char* name;
} N64AudioInfo;

typedef struct  {
	union {
		struct {
			u32 codec     : 4;
			u32 medium    : 2;
			u32 unk_bit26 : 1;
			u32 unk_bit25 : 1;
			u32 size      : 24;
		} infoBE;
		struct {
			u32 size      : 24;
			u32 unk_bit25 : 1;
			u32 unk_bit26 : 1;
			u32 medium    : 2;
			u32 codec     : 4;
		} infoLE;
		u32 data;
	};
	void32 sampleAddr; // u8*
	void32 loop; // AdpcmLoop*
	void32 book; // AdpcmBook*
	f32    tuning;
	u8 isPrim;
	u8 splitLo;
	u8 splitHi;
} SampleInfo;

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
extern const N64AudioInfo gSampleInfo[];

#endif