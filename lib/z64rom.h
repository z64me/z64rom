#ifndef __Z64ROM_H__
#define __Z64ROM_H__

#include "ExtLib.h"
#include "Rom.h"
#include "Dma.h"
#include "Audio.h"

typedef struct {
	u32 hi;
	u32 lo;
} HiLo;

typedef enum {
	NoRom = 0,
	Zelda_OoT_Debug,
	Zelda_OoT_1_0,
	Zelda_MM_U,
} AttPacked RomType;

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
		struct {
			HiLo init;
			HiLo dest;
			HiLo updt;
			HiLo draw;
		} player;
	} table;
	struct {
		u32 seqRom;
		u32 fontRom;
		u32 smplRom;
	} segment;
} RomOffset;

typedef struct {
	u32 start : 28;
	// 4
	u32 size  : 24;
	u32 free  : 1;
	// 7
} Dma;

typedef struct Rom {
	RomType   type;
	MemFile   file;
	RomOffset offset;
	struct {
		DmaEntry*       dma;
		ActorEntry*     actor;
		SceneEntry*     scene;
		ObjectEntry*    object;
		KaleidoEntry*   kaleido;
		GameStateEntry* state;
		struct {
			u16 dma;
			u16 obj;
			u16 actor;
			u16 state;
			u16 scene;
			u16 kaleido;
		} num;
	} table;
	Dma dma[2000];
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

extern const char* gObjectName[];
extern const char* gActorName[];
extern const char* gStateName[];
extern const char* gSceneName[];
extern const char* gBankName[];
extern const char* gSequenceName[];
extern const N64AudioInfo gSampleInfo[];

extern s32 gExtractAudio;
extern s32 gLog;
extern s32 gGenericNames;

void fix_crc(unsigned char* rom);

#endif