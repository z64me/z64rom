#include "z64rom.h"

static RomFile Rom_GetRomFile(Rom* rom, u32 vromA, u32 vromB) {
	DmaEntry* dmaTable = rom->dmaTable;
	s32 i;
	RomFile romFile;
	
	for (i = 0; i < DMA_ID_MAX; i++) {
		if (dmaTable[i].vromStart == vromA &&
			dmaTable[i].vromEnd == vromB) {
			break;
		}
	}
	
	romFile.romStart = ReadBE(dmaTable[i].romStart);
	if (dmaTable[i].romEnd != 0)
		romFile.romEnd = ReadBE(dmaTable[i].romEnd);
	else
		romFile.romEnd = ReadBE(dmaTable[i].vromEnd);
	romFile.data = SegmentedToVirtual(0x0, romFile.romStart);
	romFile.size = CLAMP_MIN((s32)romFile.romEnd - (s32)romFile.romStart, 0);
	
	return romFile;
}

#define Dma_RomFile_Func(type, PART, name) \
	Dma_RomFile_ ## name(Rom * rom, s32 id) { \
		type* entry = &rom->PART[id]; \
		return Rom_GetRomFile(rom, entry->vromStart, entry->vromEnd); \
	}

RomFile Dma_RomFile_Func(ObjectEntry, objectTable, Object);
RomFile Dma_RomFile_Func(ActorEntry, actorTable, Actor);
RomFile Dma_RomFile_Func(DmaEntry, dmaTable, DmaEntry);
RomFile Dma_RomFile_Func(GameStateEntry, stateTable, GameState);
RomFile Dma_RomFile_Func(SceneEntry, sceneTable, Scene);
