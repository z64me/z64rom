#include "z64rom.h"

RomFile Rom_GetRomFile(Rom* rom, u32 vromA, u32 vromB) {
	DmaEntry* dmaTable = rom->table.dma;
	s32 i;
	RomFile romFile;
	u32 useAddress = false;
	
	for (i = 0; i < rom->table.num.dma; i++) {
		if (dmaTable[i].vromStart == vromA &&
			dmaTable[i].vromEnd == vromB) {
			break;
		}
		if (i + 1 == rom->table.num.dma) {
			printf_debugExt_align("DmaEntry", "Could not find");
			printf_debug("%08X - %08X", ReadBE(vromA), ReadBE(vromB));
			useAddress = true;
		}
	}
	
	if (useAddress) {
		romFile.romStart = ReadBE(vromA);
		romFile.romEnd = ReadBE(vromB);
		romFile.data = SegmentedToVirtual(0x0, romFile.romStart);
		romFile.size = CLAMP_MIN((s32)romFile.romEnd - (s32)romFile.romStart, 0);
		
		return romFile;
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

RomFile Dma_RomFile_Func(ObjectEntry, table.object, Object);
RomFile Dma_RomFile_Func(ActorEntry, table.actor, Actor);
RomFile Dma_RomFile_Func(DmaEntry, table.dma, DmaEntry);
RomFile Dma_RomFile_Func(GameStateEntry, table.state, GameState);
RomFile Dma_RomFile_Func(SceneEntry, table.scene, Scene);

void Dma_SetFlag(u64* flag, s32 id) {
	u32 shift = id % 64;
	u32 ar = (id - shift) / 64;
	
	flag[ar] |= 1 << shift;
}

void Dma_ClearFlag(u64* flag, s32 id) {
	u32 shift = id % 64;
	u32 ar = (id - shift) / 64;
	
	flag[ar] &= ~(1 << shift);
}

s32 Dma_GetFlag(u64* flag, s32 id) {
	u32 shift = id % 64;
	u32 ar = (id - shift) / 64;
	
	return ((flag[ar] & (1 << shift)) != 0);
}

void Dma_WriteEntry(Rom* rom, MemFile* memFile) {
}