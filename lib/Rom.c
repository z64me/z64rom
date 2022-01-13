#include "z64rom.h"

u32 gDmaDataTable = 0x012F70;
u32 gObjectTable = 0xB9E6C8;
u32 gActorOverlayTable = 0xB8D440;
u32 gGameStateOverlayTable = 0xB969D0;
u32 gSceneTable = 0xBA0BB0;

// ?
u32 gSequenceFontTable = 0xBCC4E0;

/* SegmentRomStart is the offset to each AudioEntry
 * entry->romAddr += romSegAddr;
 */
u32 gSequenceTable = 0xBCC6A0;
u32 _Sequence_SegmentRomStart = 0x44df0;

u32 gSoundFontTable = 0xBCC270;
u32 _SoundFont_SegmentRomStart = 0x19030;

u32 gSampleBankTable = 0xBCCD90;
u32 _SampleBank_SegmentRomStart = 0x94870;

u32 OBJECT_ID_MAX = 402;
u32 SCENE_ID_MAX = 110;
u32 ACTOR_ID_MAX = 471;
u32 GAMESTATE_ID_MAX = 6;
u32 DMA_ID_MAX = 1548;

Sample sDumpTBL[1800];
u32 sDumpID;
Sample* sSortTBL[1800];
u32 sSortID;

static void Rom_Config_Actor(MemFile* config, ActorEntry* actorOvl, char* name, char* out) {
	char buffer[256 * 4];
	
	sprintf(buffer, "%s%s", String_GetPath(out), "config.cfg");
	MemFile_Clear(config);
	Config_WriteTitle_Str(name);
	Config_WriteVar_Hex(vram_addr, ReadBE(actorOvl->vramStart));
	Config_WriteVar_Hex(init_vars, ReadBE(actorOvl->initInfo));
	Config_WriteVar_Int(alloc_type, ReadBE(actorOvl->allocType));
	MemFile_SaveFile(config, buffer);
}

static void Rom_Config_GameState(MemFile* config, GameStateEntry* stateOvl, char* name, char* out) {
	char buffer[256 * 4];
	
	sprintf(buffer, "%s%s", String_GetPath(out), "config.cfg");
	MemFile_Clear(config);
	Config_WriteTitle_Str(name);
	Config_WriteVar_Hex(vram_addr, ReadBE(stateOvl->vramStart));
	Config_WriteVar_Hex(init_func, ReadBE(stateOvl->init));
	Config_WriteVar_Hex(dest_func, ReadBE(stateOvl->destroy));
	MemFile_SaveFile(config, buffer);
}

static void Rom_Config_Scene(MemFile* config, SceneEntry* sceneEntry, char* name, char* out) {
	char buffer[256 * 4];
	
	sprintf(buffer, "%s%s", String_GetPath(out), "config.cfg");
	MemFile_Clear(config);
	Config_WriteTitle_Str(name);
	Config_WriteVar_Int(unk_a, ReadBE(sceneEntry->unk_10));
	Config_WriteVar_Int(unk_b, ReadBE(sceneEntry->unk_12));
	Config_WriteVar_Int(shader, ReadBE(sceneEntry->config));
	MemFile_SaveFile(config, buffer);
}

#define __Config_Sample(wow) \
	__Config_WriteVar_Hex(# wow "_sample_id", ReadBE(sample->sampleAddr) + _SampleBank_SegmentRomStart + off); \
	__Config_WriteVar_Flo(# wow "_tuning", *f); \
	sDumpTBL[sDumpID].data = sample->data; \
	sDumpTBL[sDumpID].sampleAddr = ReadBE(sample->sampleAddr) + _SampleBank_SegmentRomStart + off; \
	sDumpTBL[sDumpID].loop = VirtualToSegmented(0x0, SegmentedToVirtual(0x1, ReadBE(sample->loop))); \
	sDumpTBL[sDumpID++].book = VirtualToSegmented(0x0, SegmentedToVirtual(0x1, ReadBE(sample->book)))

#define __Config_Sample_NULL(wow) \
	__Config_WriteVar_Str(# wow "_sample_id", NULL); \
	__Config_WriteVar_Str(# wow "_tuning", NULL); \
	__Config_WriteVar_Hex(# wow "_book", NULL); \
	__Config_WriteVar_Hex(# wow "_loop", NULL)

static void Rom_Config_Instrument(MemFile* config, Instrument* instrument, char* name, char* out, u32 off) {
	Adsr* envelope = SegmentedToVirtual(0x1, ReadBE(instrument->envelope));
	Sample* sample;
	u32 val;
	f32* f = (f32*)&val;
	
	MemFile_Clear(config);
	Config_WriteTitle_Str("Instrument");
	Config_WriteVar_Int(loaded, instrument->loaded);
	Config_WriteVar_Int(split_lo, instrument->splitLo);
	Config_WriteVar_Int(split_hi, instrument->splitHi);
	
	Config_SPrintf("\n");
	Config_WriteTitle_Str("Envelope");
	Config_WriteVar_Int(attack_rate, ReadBE(envelope[0].rate));
	Config_WriteVar_Int(attack_level, ReadBE(envelope[0].level));
	Config_WriteVar_Int(hold_rate, ReadBE(envelope[1].rate));
	Config_WriteVar_Int(hold_level, ReadBE(envelope[1].level));
	Config_WriteVar_Int(decay_rate, ReadBE(envelope[2].rate));
	Config_WriteVar_Int(decay_level, ReadBE(envelope[2].level));
	Config_WriteVar_Int(release, instrument->release);
	
	Config_SPrintf("\n");
	Config_WriteTitle_Str("Low Sample");
	if (instrument->lo.sample != 0) {
		sample = SegmentedToVirtual(0x1, ReadBE(instrument->lo.sample));
		val = ReadBE(instrument->lo.swap32);
		__Config_Sample(low);
	} else {
		__Config_Sample_NULL(low);
	}
	
	Config_SPrintf("\n");
	Config_WriteTitle_Str("Primary Sample");
	if (instrument->prim.sample != 0) {
		sample = SegmentedToVirtual(0x1, ReadBE(instrument->prim.sample));
		val = ReadBE(instrument->prim.swap32);
		__Config_Sample(prim);
	} else {
		__Config_Sample_NULL(prim);
	}
	
	Config_SPrintf("\n");
	Config_WriteTitle_Str("High Sample");
	if (instrument->hi.sample != 0) {
		sample = SegmentedToVirtual(0x1, ReadBE(instrument->hi.sample));
		val = ReadBE(instrument->hi.swap32);
		__Config_Sample(hi);
	} else {
		__Config_Sample_NULL(hi);
	}
	
	MemFile_SaveFile(config, out);
}

static void Rom_Config_Sfx(MemFile* config, Sound* sfx, char* name, char* out, u32 off) {
	u32 val;
	f32* f = (f32*)&val;
	
	MemFile_Clear(config);
	Config_WriteTitle_Str("Sfx");
	if (sfx->sample != 0) {
		Sample* sample = SegmentedToVirtual(0x1, ReadBE(sfx->sample));
		val = ReadBE(sfx->swap32);
		__Config_Sample(prim);
	} else {
		__Config_Sample_NULL(prim);
	}
	MemFile_SaveFile(config, out);
}

static void Rom_Config_Drum(MemFile* config, Drum* drum, char* name, char* out, u32 off) {
	Adsr* envelope = SegmentedToVirtual(0x1, ReadBE(drum->envelope));
	u32 val;
	f32* f = (f32*)&val;
	
	MemFile_Clear(config);
	Config_WriteTitle_Str("Drum");
	Config_WriteVar_Int(loaded, drum->loaded);
	Config_WriteVar_Int(pan, drum->pan);
	
	Config_SPrintf("\n");
	Config_WriteTitle_Str("Envelope");
	Config_WriteVar_Int(attack_rate, ReadBE(envelope[0].rate));
	Config_WriteVar_Int(attack_level, ReadBE(envelope[0].level));
	Config_WriteVar_Int(hold_rate, ReadBE(envelope[1].rate));
	Config_WriteVar_Int(hold_level, ReadBE(envelope[1].level));
	Config_WriteVar_Int(decay_rate, ReadBE(envelope[2].rate));
	Config_WriteVar_Int(decay_level, ReadBE(envelope[2].level));
	Config_WriteVar_Int(release, drum->release);
	
	Config_SPrintf("\n");
	Config_WriteTitle_Str("Sample");
	if (drum->sound.sample != 0) {
		Sample* sample = SegmentedToVirtual(0x1, ReadBE(drum->sound.sample));
		val = ReadBE(drum->sound.swap32);
		__Config_Sample(prim);
	} else {
		__Config_Sample_NULL(prim);
	}
	
	MemFile_SaveFile(config, out);
}

static void Rom_Config_Sample(MemFile* config, Sample* sample, char* name, char* out) {
	AdpcmBook* book = SegmentedToVirtual(0x0, sample->book);
	AdpcmLoop* loop = SegmentedToVirtual(0x0, sample->loop);
	u32 val;
	f32* f = (f32*)&val;
	
	MemFile_Clear(config);
	Config_WriteTitle_Str(name);
	Config_WriteVar_Int(codec, ReadBE(sample->data) >> (32 - 4));
	Config_WriteVar_Int(medium, (ReadBE(sample->data) >> (32 - 6)) & 2);
	Config_WriteVar_Int(bitA, (ReadBE(sample->data) >> (32 - 7)) & 1);
	Config_WriteVar_Int(bitB, (ReadBE(sample->data) >> (32 - 8)) & 1);
	
	Config_SPrintf("\n");
	Config_WriteTitle_Str("Loop");
	Config_WriteVar_Int(start, ReadBE(loop->start));
	Config_WriteVar_Int(end, ReadBE(loop->end));
	Config_WriteVar_Int(count, ReadBE(loop->count));
	Config_WriteVar_Int(tail_end, ReadBE(loop->origSpls));
	
	MemFile_SaveFile(config, out);
}

static s32 Rom_Extract(MemFile* mem, RomFile rom, char* name) {
	if (rom.size == 0)
		return 0;
	MemFile_Clear(mem);
	mem->dataSize = rom.size;
	MemFile_Realloc(mem, rom.size);
	MemFile_Write(mem, rom.data, rom.size);
	MemFile_SaveFile(mem, name);
	
	return 1;
}

void Rom_Dump_SoundFont(Rom* rom, MemFile* dataFile, MemFile* config) {
	AudioEntryHead* head = SegmentedToVirtual(0, gSoundFontTable);
	AudioEntryHead* sampHead = SegmentedToVirtual(0, gSampleBankTable);
	SoundFontEntry* entry;
	u32 num = ReadBE(head->numEntries);
	SoundFont* bank;
	Instrument* instrument;
	Sound* sfx;
	Drum* drum;
	u32 off = 0;
	
	Dir_Enter("soundfont/");
	for (s32 i = 0; i < num; i++) {
		printf_progress("SoundFont", i + 1, num);
		
		entry = &head->entries[i];
		bank = SegmentedToVirtual(0x0, ReadBE(entry->romAddr) + _SoundFont_SegmentRomStart);
		off = ReadBE(sampHead->entries[entry->audioTable1].romAddr);
		SetSegment(0x1, bank);
		
		Dir_Enter("0x%02X-%s/", i, gBankName[i]);
		
		if (entry->numInst) {
			Dir_Enter("instruments/");
			
			for (s32 j = 0; j < entry->numInst; j++) {
				if (bank->instruments[j] == 0)
					continue;
				instrument = SegmentedToVirtual(0x1, ReadBE(bank->instruments[j]));
				Rom_Config_Instrument(config, instrument, "instrument", Dir_File("%d-Inst.cfg", j), off);
			}
			
			Dir_Leave();
		}
		
		if (entry->numSfx) {
			Dir_Enter("sfx/");
			
			for (s32 j = 0; j < ReadBE(entry->numSfx); j++) {
				sfx = SegmentedToVirtual(0x1, ReadBE(bank->sfx));
				if (sfx[j].sample == 0)
					continue;
				Rom_Config_Sfx(config, &sfx[j], "Sound Effect", Dir_File("%d-Sfx.cfg", j), off);
			}
			
			Dir_Leave();
		}
		
		if (entry->numDrum) {
			Dir_Enter("drums/");
			
			for (s32 j = 0; j < entry->numDrum; j++) {
				u32* wow = SegmentedToVirtual(0x1, ReadBE(bank->drums));
				
				if (wow[j] == 0)
					continue;
				drum = SegmentedToVirtual(0x1, ReadBE(wow[j]));
				if (drum->sound.sample == 0)
					continue;
				Rom_Config_Drum(config, drum, "Drum", Dir_File("%d-Drum.cfg", j), off);
			}
			
			Dir_Leave();
		}
		
		Dir_Leave();
	}
	Dir_Leave();
	SetSegment(0x1, NULL);
}

void Rom_Dump_Sequences(Rom* rom, MemFile* dataFile, MemFile* config) {
	AudioEntryHead* head = SegmentedToVirtual(0, gSequenceTable);
	SoundFontEntry* entry;
	RomFile romFile;
	u32 num = ReadBE(head->numEntries);
	
	Dir_Enter("sequences/");
	
	for (s32 i = 0; i < num; i++) {
		entry = &head->entries[i];
		romFile.data = SegmentedToVirtual(0x0, ReadBE(entry->romAddr) + _Sequence_SegmentRomStart);
		romFile.size = ReadBE(entry->size);
		
		if (romFile.size == 0)
			continue;
		
		printf_progress("Sequence", i + 1, num);
		Rom_Extract(dataFile, romFile, Dir_File("0x%02X-%s.seq", i, gSequenceName[i]));
	}
	
	Dir_Leave();
}

void Rom_Dump_Samples(Rom* rom, MemFile* dataFile, MemFile* config) {
	Sample* smallest = sDumpTBL;
	Sample* largest = sDumpTBL;
	RomFile rf;
	Sample** tbl;
	AdpcmLoop* loop;
	AdpcmBook* book;
	
	for (s32 i = 0; i < sDumpID; i++) {
		if (smallest->sampleAddr > sDumpTBL[i].sampleAddr) {
			smallest = &sDumpTBL[i];
		}
		if (largest->sampleAddr < sDumpTBL[i].sampleAddr) {
			largest = &sDumpTBL[i];
		}
	}
	sSortTBL[sSortID++] = smallest;
	
	while (1) {
		smallest = largest;
		
		for (s32 i = 0; i < sDumpID; i++) {
			if (sDumpTBL[i].sampleAddr < smallest->sampleAddr) {
				if (sDumpTBL[i].sampleAddr > sSortTBL[sSortID - 1]->sampleAddr)
					smallest = &sDumpTBL[i];
			}
		}
		sSortTBL[sSortID++] = smallest;
		if (smallest->sampleAddr == largest->sampleAddr)
			break;
	}
	
	tbl = sSortTBL;
	
	Dir_Enter("samples/"); {
		for (s32 i = 0; i < sSortID; i++) {
			printf_progress("Sample", i + 1, sSortID);
			Dir_Enter("%d-Smpl/", i); {
				book = SegmentedToVirtual(0x0, tbl[i]->book);
				loop = SegmentedToVirtual(0x0, tbl[i]->loop);
				
				rf.size = ReadBE(tbl[i]->data) & 0x00FFFFFF;
				rf.data = SegmentedToVirtual(0x0, tbl[i]->sampleAddr);
				Rom_Extract(dataFile, rf, Dir_File("Sample.bin"));
				
				rf.size = 8 * ReadBE(book->order) * ReadBE(book->npredictors) + 8;
				rf.data = book;
				Rom_Extract(dataFile, rf, Dir_File("Book.bin"));
				
				Rom_Config_Sample(config, tbl[i], "Sample", Dir_File("config.cfg"));
				
				if (loop->count) {
					rf.size = 0x20;
					rf.data = loop;
					Rom_Extract(dataFile, rf, Dir_File("LoopBook.bin"));
				}
			} Dir_Leave();
		}
	} Dir_Leave();
}

Rom* Rom_New(char* romName) {
	Rom* rom = Lib_Calloc(0, sizeof(struct Rom));
	
	Assert(rom != NULL);
	
	if (MemFile_LoadFile(&rom->file, romName)) {
		printf_error_align("Load Rom", "%s", romName);
	}
	
	SetSegment(0x0, rom->file.data);
	rom->dmaTable = SegmentedToVirtual(0x0, gDmaDataTable);
	rom->objectTable = SegmentedToVirtual(0x0, gObjectTable);
	rom->actorTable = SegmentedToVirtual(0x0, gActorOverlayTable);
	rom->stateTable = SegmentedToVirtual(0x0, gGameStateOverlayTable);
	rom->sceneTable = SegmentedToVirtual(0x0, gSceneTable);
	
	printf_debugExt_align("Load Rom", "%s", romName);
	
	return rom;
}

Rom* Rom_Free(Rom* rom) {
	MemFile_Free(&rom->file);
	memset(rom, 0, sizeof(struct Rom));
	
	return NULL;
}

void Rom_Dump(Rom* rom) {
	MemFile dataFile;
	MemFile config;
	RomFile rf;
	char buffer[256 * 4];
	
	MemFile_Malloc(&dataFile, 0x460000); // Slightly larger than audiotable
	MemFile_Malloc(&config, 0x25000);
	
	Dir_Enter("rom/");
	printf_info_align("Dump Rom", PRNT_YELW "%s", rom->file.info.name);
	#if 0
		Dir_Enter("actor/"); {
			for (s32 i = 0; i < ACTOR_ID_MAX; i++) {
				rf = Dma_RomFile_Actor(rom, i);
				
				if (rf.size == 0)
					continue;
				
				printf_progress("Actor", i + 1, ACTOR_ID_MAX);
				Dir_Enter("0x%04X-%s/", i, gActorName[i]); {
					if (Rom_Extract(&dataFile, rf, Dir_File("actor.zovl")))
						Rom_Config_Actor(&config, &rom->actorTable[i], gActorName[i], Dir_File("config.cfg"));
				} Dir_Leave();
			}
		} Dir_Leave();
		
		Dir_Enter("object/"); {
			for (s32 i = 0; i < OBJECT_ID_MAX; i++) {
				rf = Dma_RomFile_Object(rom, i);
				
				if (rf.size == 0)
					continue;
				
				printf_progress("Object", i + 1, OBJECT_ID_MAX);
				Dir_Enter("0x%04X-%s/", i, gObjectName[i]); {
					Rom_Extract(&dataFile, rf, Dir_File("object.zobj"));
				} Dir_Leave();
			}
		} Dir_Leave();
		
		Dir_Enter("system/"); {
			for (s32 i = 0; i < GAMESTATE_ID_MAX; i++) {
				rf = Dma_RomFile_GameState(rom, i);
				
				if (rf.size == 0)
					continue;
				
				printf_progress("System", i + 1, GAMESTATE_ID_MAX);
				Dir_Enter("0x%02X-%s/", i, gStateName[i]); {
					if (Rom_Extract(&dataFile, rf, Dir_File("state.zovl")))
						Rom_Config_GameState(&config, &rom->stateTable[i], gStateName[i], Dir_File("config.cfg"));
				} Dir_Leave();
			}
		} Dir_Leave();
		
		Dir_Enter("scene/"); {
			for (s32 i = 0; i < SCENE_ID_MAX; i++) {
				rf = Dma_RomFile_Scene(rom, i);
				
				if (rf.size == 0)
					continue;
				
				printf_progress("Scene", i + 1, SCENE_ID_MAX);
				Dir_Enter("0x%02X-%s/", i, gSceneName[i]); {
					if (Rom_Extract(&dataFile, rf, Dir_File("scene.zscene")))
						Rom_Config_Scene(&config, &rom->sceneTable[i], gSceneName[i], Dir_File("config.cfg"));
				} Dir_Leave();
			}
		} Dir_Leave();
	#endif
	Dir_Enter("sound/"); {
		Rom_Dump_SoundFont(rom, &dataFile, &config);
		Rom_Dump_Sequences(rom, &dataFile, &config);
		Rom_Dump_Samples(rom, &dataFile, &config);
	} Dir_Leave();
	
	Dir_Leave();
	
	MemFile_Free(&dataFile);
	MemFile_Free(&config);
}