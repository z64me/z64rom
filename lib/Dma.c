#include "z64rom.h"

static RomFile Rom_GetRomFile(Rom* rom, u32 vromA, u32 vromB) {
	DmaEntry* dmaTable = rom->dmaTable;
	s32 i;
	RomFile romFile;
	
	for (i = 0; i < TABLESIZE_DMA; i++) {
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
	romFile.data = Lib_SegToPtr(romFile.romStart);
	romFile.size = CLAMP_MIN((s32)romFile.romEnd - (s32)romFile.romStart, 0);
	
	return romFile;
}

#define Dma_RomFile_GetX(type, PART, name) \
	Dma_RomFile_Get ## name(Rom * rom, s32 id) { \
		type* entry = &rom->PART[id]; \
		return Rom_GetRomFile(rom, entry->vromStart, entry->vromEnd); \
	}

RomFile Dma_RomFile_GetX(VRomFile, objectTable, Object);
RomFile Dma_RomFile_GetX(ActorOverlay, actorTable, Actor);
RomFile Dma_RomFile_GetX(DmaEntry, dmaTable, DmaEntry);
RomFile Dma_RomFile_GetX(GameStateOverlay, stateTable, State);
