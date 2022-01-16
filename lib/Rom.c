#include "z64rom.h"

u32 gDmaDataTable = 0x012F70;
u32 gObjectTable = 0xB9E6C8;
u32 gActorOverlayTable = 0xB8D440;
u32 gGameStateOverlayTable = 0xB969D0;
u32 gSceneTable = 0xBA0BB0;
u32 gKaleidoTable = 0xBA4340;

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
u32 KALEIDO_ID_MAX = 2;

char sBankFiles[1800][256];
u32 sBankNum;
SampleInfo sUnsortedSampleTbl[1800];
SampleInfo* sSortedSampleTbl[1800];
u32 sDumpID;
u32 sSortID;

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

static void Rom_Config_Actor(MemFile* config, ActorEntry* actorOvl, char* name, char* out) {
	MemFile_Clear(config);
	Config_WriteTitle_Str(name);
	Config_WriteVar_Hex("vram_addr", ReadBE(actorOvl->vramStart));
	Config_WriteVar_Hex("init_vars", ReadBE(actorOvl->initInfo));
	Config_WriteVar_Int("alloc_type", ReadBE(actorOvl->allocType));
	MemFile_SaveFile_String(config, out);
}

static void Rom_Config_GameState(MemFile* config, GameStateEntry* stateOvl, char* name, char* out) {
	MemFile_Clear(config);
	Config_WriteTitle_Str(name);
	Config_WriteVar_Hex("vram_addr", ReadBE(stateOvl->vramStart));
	Config_WriteVar_Hex("init_func", ReadBE(stateOvl->init));
	Config_WriteVar_Hex("dest_func", ReadBE(stateOvl->destroy));
	MemFile_SaveFile_String(config, out);
}

static void Rom_Config_Player(MemFile* config, KaleidoEntry* player, char* name, char* out) {
	u16* dataHi;
	u16* dataLo;
	u32 init;
	u32 dest;
	u32 updt;
	u32 draw;
	
	dataHi = SegmentedToVirtual(0x0, 0x00B288F8);
	dataLo = SegmentedToVirtual(0x0, 0x00B28900);
	init = ReadBE(dataHi[1]) << 16 | ReadBE(dataLo[1]);
	
	dataHi = SegmentedToVirtual(0x0, 0x00B28908);
	dataLo = SegmentedToVirtual(0x0, 0x00B28914);
	dest = ReadBE(dataHi[1]) << 16 | ReadBE(dataLo[1]);
	
	dataHi = SegmentedToVirtual(0x0, 0x00B2891C);
	dataLo = SegmentedToVirtual(0x0, 0x00B28928);
	updt = ReadBE(dataHi[1]) << 16 | ReadBE(dataLo[1]);
	
	dataHi = SegmentedToVirtual(0x0, 0x00B28930);
	dataLo = SegmentedToVirtual(0x0, 0x00B2893C);
	draw = ReadBE(dataHi[1]) << 16 | ReadBE(dataLo[1]);
	
	MemFile_Clear(config);
	Config_WriteTitle_Str(name);
	Config_WriteVar_Hex("vram_addr", ReadBE(player->vramStart));
	Config_WriteVar_Hex("init", init);
	Config_WriteVar_Hex("dest", dest);
	Config_WriteVar_Hex("updt", updt);
	Config_WriteVar_Hex("draw", draw);
	MemFile_SaveFile_String(config, out);
}

static void Rom_Config_Scene(MemFile* config, SceneEntry* sceneEntry, char* name, char* out) {
	MemFile_Clear(config);
	Config_WriteTitle_Str(name);
	Config_WriteVar_Int("unk_a", ReadBE(sceneEntry->unk_10));
	Config_WriteVar_Int("unk_b", ReadBE(sceneEntry->unk_12));
	Config_WriteVar_Int("shader", ReadBE(sceneEntry->config));
	MemFile_SaveFile_String(config, out);
}

#define __Config_Sample(wow, sampletype) \
	Config_WriteVar_Hex(# wow "_sample", ReadBE(sample->sampleAddr) + _SampleBank_SegmentRomStart + off); \
	Config_WriteVar_Flo(# wow "_tuning", *f); \
	sUnsortedSampleTbl[sDumpID].tuning = *f; \
	sUnsortedSampleTbl[sDumpID].data = sample->data; \
	sUnsortedSampleTbl[sDumpID].sampleAddr = ReadBE(sample->sampleAddr) + _SampleBank_SegmentRomStart + off; \
	sUnsortedSampleTbl[sDumpID].loop = VirtualToSegmented(0x0, SegmentedToVirtual(0x1, ReadBE(sample->loop))); \
	sUnsortedSampleTbl[sDumpID++].book = VirtualToSegmented(0x0, SegmentedToVirtual(0x1, ReadBE(sample->book)));

#define __Config_Sample_NULL(wow) \
	Config_WriteVar_Str(# wow "_sample_id", "NULL"); \
	Config_WriteVar_Str(# wow "_tuning", "NULL");

static void Rom_Config_Instrument(MemFile* config, Instrument* instrument, char* name, char* out, u32 off) {
	Adsr* envelope = SegmentedToVirtual(0x1, ReadBE(instrument->envelope));
	Sample* sample;
	u32 val;
	f32* f = (f32*)&val;
	
	MemFile_Clear(config);
	Config_WriteTitle_Str("Instrument");
	Config_WriteVar_Int("loaded", instrument->loaded);
	Config_WriteVar_Int("split_lo", instrument->splitLo);
	Config_WriteVar_Int("split_hi", instrument->splitHi);
	
	Config_SPrintf("\n");
	Config_WriteTitle_Str("Envelope");
	Config_WriteVar_Int("attack_rate", ReadBE(envelope[0].rate));
	Config_WriteVar_Int("attack_level", ReadBE(envelope[0].level));
	Config_WriteVar_Int("hold_rate", ReadBE(envelope[1].rate));
	Config_WriteVar_Int("hold_level", ReadBE(envelope[1].level));
	Config_WriteVar_Int("decay_rate", ReadBE(envelope[2].rate));
	Config_WriteVar_Int("decay_level", ReadBE(envelope[2].level));
	Config_WriteVar_Int("release", instrument->release);
	
	Config_SPrintf("\n");
	Config_WriteTitle_Str("Low Sample");
	if (instrument->lo.sample != 0) {
		sample = SegmentedToVirtual(0x1, ReadBE(instrument->lo.sample));
		val = ReadBE(instrument->lo.swap32);
		__Config_Sample(low, ins);
	} else {
		__Config_Sample_NULL(low);
	}
	
	Config_SPrintf("\n");
	Config_WriteTitle_Str("Primary Sample");
	if (instrument->prim.sample != 0) {
		sample = SegmentedToVirtual(0x1, ReadBE(instrument->prim.sample));
		val = ReadBE(instrument->prim.swap32);
		sUnsortedSampleTbl[sDumpID].isPrim = true;
		sUnsortedSampleTbl[sDumpID].splitLo = instrument->splitLo;
		sUnsortedSampleTbl[sDumpID].splitHi = instrument->splitHi;
		__Config_Sample(prim, ins);
	} else {
		__Config_Sample_NULL(prim);
	}
	
	Config_SPrintf("\n");
	Config_WriteTitle_Str("High Sample");
	if (instrument->hi.sample != 0) {
		sample = SegmentedToVirtual(0x1, ReadBE(instrument->hi.sample));
		val = ReadBE(instrument->hi.swap32);
		__Config_Sample(hi, ins);
	} else {
		__Config_Sample_NULL(hi);
	}
	
	MemFile_SaveFile_String(config, out);
}

static void Rom_Config_Sfx(MemFile* config, Sound* sfx, char* name, char* out, u32 off) {
	u32 val;
	f32* f = (f32*)&val;
	
	MemFile_Clear(config);
	Config_WriteTitle_Str("Sfx");
	if (sfx->sample != 0) {
		Sample* sample = SegmentedToVirtual(0x1, ReadBE(sfx->sample));
		val = ReadBE(sfx->swap32);
		__Config_Sample(prim, sfx);
	} else {
		__Config_Sample_NULL(prim);
	}
	MemFile_SaveFile_String(config, out);
}

static void Rom_Config_Drum(MemFile* config, Drum* drum, char* name, char* out, u32 off) {
	Adsr* envelope = SegmentedToVirtual(0x1, ReadBE(drum->envelope));
	u32 val;
	f32* f = (f32*)&val;
	
	MemFile_Clear(config);
	Config_WriteTitle_Str("Drum");
	Config_WriteVar_Int("loaded", drum->loaded);
	Config_WriteVar_Int("pan", drum->pan);
	
	Config_SPrintf("\n");
	Config_WriteTitle_Str("Envelope");
	Config_WriteVar_Int("attack_rate", ReadBE(envelope[0].rate));
	Config_WriteVar_Int("attack_level", ReadBE(envelope[0].level));
	Config_WriteVar_Int("hold_rate", ReadBE(envelope[1].rate));
	Config_WriteVar_Int("hold_level", ReadBE(envelope[1].level));
	Config_WriteVar_Int("decay_rate", ReadBE(envelope[2].rate));
	Config_WriteVar_Int("decay_level", ReadBE(envelope[2].level));
	Config_WriteVar_Int("release", drum->release);
	
	Config_SPrintf("\n");
	Config_WriteTitle_Str("Sample");
	if (drum->sound.sample != 0) {
		Sample* sample = SegmentedToVirtual(0x1, ReadBE(drum->sound.sample));
		val = ReadBE(drum->sound.swap32);
		__Config_Sample(prim, drm);
	} else {
		__Config_Sample_NULL(prim);
	}
	
	MemFile_SaveFile_String(config, out);
}

static void Rom_Config_Sample(MemFile* config, Sample* sample, char* name, char* out) {
	AdpcmLoop* loop = SegmentedToVirtual(0x0, sample->loop);
	
	MemFile_Clear(config);
	Config_WriteTitle_Str(name);
	Config_WriteVar_Int("codec", ReadBE(sample->data) >> (32 - 4));
	Config_WriteVar_Int("medium", (ReadBE(sample->data) >> (32 - 6)) & 2);
	Config_WriteVar_Int("bitA", (ReadBE(sample->data) >> (32 - 7)) & 1);
	Config_WriteVar_Int("bitB", (ReadBE(sample->data) >> (32 - 8)) & 1);
	
	Config_SPrintf("\n");
	Config_WriteTitle_Str("Loop");
	Config_WriteVar_Int("loop_start", ReadBE(loop->start));
	Config_WriteVar_Int("loop_end", ReadBE(loop->end));
	Config_WriteVar_Int("loop_count", ReadBE(loop->count));
	Config_WriteVar_Int("tail_end", ReadBE(loop->origSpls));
	
	MemFile_SaveFile_String(config, out);
}

static void Rom_Dump_SoundFont(Rom* rom, MemFile* dataFile, MemFile* config) {
	AudioEntryHead* head = SegmentedToVirtual(0, gSoundFontTable);
	AudioEntryHead* sampHead = SegmentedToVirtual(0, gSampleBankTable);
	SoundFontEntry* entry;
	u32 num = ReadBE(head->numEntries);
	SoundFont* bank;
	Instrument* instrument;
	Sound* sfx;
	Drum* drum;
	u32 off = 0;
	
	printf_debugExt_align("Entry Num", "%d", num);
	
	Dir_Enter("soundfont/");
	for (s32 i = 0; i < num; i++) {
		printf_progress("SoundFont", i + 1, num);
		
		entry = &head->entries[i];
		
		#ifndef NDEBUG
			printf_debug_align("numInst", "%d", ReadBE(entry->numInst));
			printf_debug_align("numSfx", "%d", ReadBE(entry->numSfx));
			printf_debug_align("numDrum", "%d", ReadBE(entry->numDrum));
			printf_debug_align("bank", "%08X", ReadBE(entry->romAddr) + _SoundFont_SegmentRomStart);
		#endif
		
		bank = SegmentedToVirtual(0x0, ReadBE(entry->romAddr) + _SoundFont_SegmentRomStart);
		off = ReadBE(sampHead->entries[entry->audioTable1].romAddr);
		SetSegment(0x1, bank);
		
		Dir_Enter("0x%02X-%s/", i, gBankName[i]);
		
		if (entry->numInst) {
			Dir_Enter("instruments/");
			
			#ifndef NDEBUG
				printf_debug_align("dump", "instruments");
			#endif
			
			for (s32 j = 0; j < entry->numInst; j++) {
				char* output = Dir_File("%d-Inst.cfg", j);
				if (bank->instruments[j] == 0)
					continue;
				instrument = SegmentedToVirtual(0x1, ReadBE(bank->instruments[j]));
				Rom_Config_Instrument(config, instrument, "instrument", output, off);
				String_Copy(sBankFiles[sBankNum++], output);
			}
			
			Dir_Leave();
		}
		
		if (entry->numSfx) {
			Dir_Enter("sfx/");
			
			#ifndef NDEBUG
				printf_debug_align("dump", "sfx");
			#endif
			
			for (s32 j = 0; j < ReadBE(entry->numSfx); j++) {
				char* output = Dir_File("%d-Sfx.cfg", j);
				sfx = SegmentedToVirtual(0x1, ReadBE(bank->sfx));
				if (sfx[j].sample == 0)
					continue;
				Rom_Config_Sfx(config, &sfx[j], "Sound Effect", output, off);
				String_Copy(sBankFiles[sBankNum++], output);
			}
			
			Dir_Leave();
		}
		
		if (entry->numDrum) {
			Dir_Enter("drums/");
			
			#ifndef NDEBUG
				printf_debug_align("dump", "drums");
			#endif
			
			for (s32 j = 0; j < entry->numDrum; j++) {
				char* output = Dir_File("%d-Drum.cfg", j);
				u32* wow = SegmentedToVirtual(0x1, ReadBE(bank->drums));
				
				if (wow[j] == 0)
					continue;
				drum = SegmentedToVirtual(0x1, ReadBE(wow[j]));
				if (drum->sound.sample == 0)
					continue;
				Rom_Config_Drum(config, drum, "Drum", output, off);
				String_Copy(sBankFiles[sBankNum++], output);
			}
			
			Dir_Leave();
		}
		
		Dir_Leave();
	}
	Dir_Leave();
	SetSegment(0x1, NULL);
}

static void Rom_Dump_Sequences(Rom* rom, MemFile* dataFile, MemFile* config) {
	AudioEntryHead* head = SegmentedToVirtual(0x0, gSequenceTable);
	u16* seqFontTable;
	SoundFontEntry* entry;
	RomFile romFile;
	u32 num = ReadBE(head->numEntries);
	
	Dir_Enter("sequences/");
	SetSegment(0x1, SegmentedToVirtual(0x0, gSequenceFontTable));
	
	MemFile_Clear(config);
	for (s32 i = 0; i < num; i++) {
		seqFontTable = SegmentedToVirtual(0x0, gSequenceFontTable);
		entry = &head->entries[i];
		romFile.data = SegmentedToVirtual(0x0, ReadBE(entry->romAddr) + _Sequence_SegmentRomStart);
		romFile.size = ReadBE(entry->size);
		
		if (romFile.size == 0)
			continue;
		
		printf_progress("Sequence", i + 1, num);
		Rom_Extract(dataFile, romFile, Dir_File("0x%02X-%s.seq", i, gSequenceName[i]));
		
		seqFontTable = SegmentedToVirtual(0x1, ReadBE(seqFontTable[i]));
		
		Config_WriteTitle_Str(gSequenceName[i]);
		Config_WriteVar_Hex("sequence_id", i);
		Config_WriteVar_Hex("bank_id", ReadBE(seqFontTable[0]) & 0xFF);
		Config_SPrintf("\n");
	}
	MemFile_SaveFile_String(config, Dir_File("config.cfg"));
	
	SetSegment(0x1, NULL);
	
	Dir_Leave();
}

static void Rom_Dump_Samples_PatchWavFiles(MemFile* dataFile, MemFile* config) {
	#define NOTE(note, octave) (note + (12 * (octave)))
	u8* instInfo;
	u32* smplInfo;
	const struct {
		u8 basenote;
		s8 finetune;
		const N64AudioInfo* info;
	} info[] = {
		{ NOTE(0, 3), 0, &gSampleInfo[367] }, // LowPerc
		{ NOTE(1, 3), 0, &gSampleInfo[368] }, // Snare
		{ NOTE(3, 3), 0, &gSampleInfo[369] }, // SoftSnare
		{ NOTE(1, 4), 0, &gSampleInfo[370] }, // Cymbal
		{ NOTE(0, 6), 0, &gSampleInfo[371] }, // Timpani
		{ NOTE(5, 3), 0, &gSampleInfo[405] }, // Gong
		{ NOTE(0, 6), 0, &gSampleInfo[406] }, // WindChimes
		
		{ NOTE(0, 5), 0, &gSampleInfo[413] }, // CongaOpen
		{ NOTE(0, 5), 0, &gSampleInfo[414] }, // CongaSoft
		{ NOTE(8, 4), 0, &gSampleInfo[415] }, // CongaMute
		
		{ NOTE(5, 3), 0, &gSampleInfo[393] }, // LuteA
		{ NOTE(7, 3), 0, &gSampleInfo[394] }, // LuteB
		
		{ NOTE(11, 3), 0, &gSampleInfo[378] }, // Tambourine
		{ NOTE(0, 4), 0, &gSampleInfo[120] }, // Tambourine
		{ NOTE(2, 4), 0, &gSampleInfo[121] }, // Tambourine
		
		{ NOTE(0, 3), 0, &gSampleInfo[432] }, // Cajon
		{ NOTE(2, 3), 0, &gSampleInfo[433] }, // Cajon
	};
	
	Dir_Enter("samples/"); {
		for (s32 i = 0; i < ArrayCount(info); i++) {
			printf_progress("Update Sample", i + 1, ArrayCount(info));
			char* file = Dir_File("%s/Sample.wav", info[i].info->name);
			
			MemFile_Clear(dataFile);
			MemFile_LoadFile(dataFile, file);
			instInfo = Lib_MemMem(dataFile->data, dataFile->dataSize, "inst", 4);
			smplInfo = Lib_MemMem(dataFile->data, dataFile->dataSize, "smpl", 4);
			
			/* basenote */ instInfo[8] = info[i].basenote;
			/* finetune */ instInfo[9] = info[i].finetune;
			/* basenote */ smplInfo[5] = info[i].basenote;
			/* finetune */ smplInfo[6] = info[i].finetune;
			MemFile_SaveFile(dataFile, file);
		}
	} Dir_Leave();
	#undef NOTE
}

static void Rom_Dump_Samples(Rom* rom, MemFile* dataFile, MemFile* config) {
	SampleInfo* smallest = sUnsortedSampleTbl;
	SampleInfo* largest = sUnsortedSampleTbl;
	RomFile rf;
	SampleInfo** tbl;
	AdpcmLoop* loop;
	AdpcmBook* book;
	char buff[16];
	char* name;
	u32 sampRate;
	char sysbuf[256 * 2];
	
	printf_debug("sort I");
	
	for (s32 i = 0; i < sDumpID; i++) {
		if (smallest->sampleAddr > sUnsortedSampleTbl[i].sampleAddr) {
			smallest = &sUnsortedSampleTbl[i];
		}
		if (largest->sampleAddr < sUnsortedSampleTbl[i].sampleAddr) {
			largest = &sUnsortedSampleTbl[i];
		}
	}
	sSortedSampleTbl[sSortID++] = smallest;
	printf_debug("sort II");
	
	while (1) {
		smallest = largest;
		
		for (s32 i = 0; i < sDumpID; i++) {
			if (sUnsortedSampleTbl[i].sampleAddr < smallest->sampleAddr) {
				if (sUnsortedSampleTbl[i].sampleAddr > sSortedSampleTbl[sSortID - 1]->sampleAddr) {
					smallest = &sUnsortedSampleTbl[i];
				}
			}
		}
		sSortedSampleTbl[sSortID++] = smallest;
		if (smallest->sampleAddr == largest->sampleAddr)
			break;
	}
	printf_debug("sort OK");
	
	tbl = sSortedSampleTbl;
	
	Dir_Enter("samples/"); {
		for (s32 i = 0; i < sSortID; i++) {
			printf_progress("Sample", i + 1, sSortID);
			name = gSampleInfo[i].dublicate == NULL ? gSampleInfo[i].name : gSampleInfo[i].dublicate->name;
			sampRate = gSampleInfo[i].dublicate == NULL ? gSampleInfo[i].sampleRate : gSampleInfo[i].dublicate->sampleRate;
			
			if (name == NULL)
				printf_error("Sample ID [%D] is missing name", i);
			
			if (sampRate == 0)
				printf_error("Sample [%s] is missing samplerate", name);
			
			Dir_Enter("%s/", name); {
				book = SegmentedToVirtual(0x0, tbl[i]->book);
				loop = SegmentedToVirtual(0x0, tbl[i]->loop);
				
				rf.size = ReadBE(tbl[i]->data) & 0x00FFFFFF;
				rf.data = SegmentedToVirtual(0x0, tbl[i]->sampleAddr);
				Rom_Extract(dataFile, rf, Dir_File("vadpcm.bin"));
				
				rf.size = sizeof(s16) * 8 * ReadBE(book->order) * ReadBE(book->npredictors) + 8;
				rf.data = book;
				Rom_Extract(dataFile, rf, Dir_File("book.bin"));
				
				Rom_Config_Sample(config, (Sample*)tbl[i], name, Dir_File("config.cfg"));
				
				if (loop->count) {
					rf.size = 0x20;
					rf.data = loop;
					Rom_Extract(dataFile, rf, Dir_File("loopbook.bin"));
				}
				
				#ifndef _WIN32
					String_Copy(sysbuf, "./tools/z64audio --i ");
				#else
					String_Copy(sysbuf, "tools\\z64audio.exe --i ");
				#endif
				String_Merge(sysbuf, Dir_File("vadpcm.bin"));
				String_Merge(sysbuf, " --o ");
				String_Merge(sysbuf, Dir_File("Sample.wav"));
				if (gPrintfSuppress == PSL_DEBUG)
					String_Merge(sysbuf, " --D");
				else
					String_Merge(sysbuf, " --S");
				String_Merge(sysbuf, " --srate ");
				String_Merge(sysbuf, TempPrintf("%d", sampRate));
				String_Merge(sysbuf, " --tuning ");
				String_Merge(sysbuf, TempPrintf("%f", tbl[i]->tuning));
				
				if (tbl[i]->isPrim && (tbl[i]->splitHi != 127 || tbl[i]->splitLo != 0)) {
					String_Merge(sysbuf, " --split-hi ");
					String_Merge(sysbuf, TempPrintf("%d", tbl[i]->splitHi + 21));
					if (tbl[i]->splitLo) {
						String_Merge(sysbuf, " --split-lo ");
						String_Merge(sysbuf, TempPrintf("%d", tbl[i]->splitLo + 21));
					}
				}
				
				if (system(sysbuf))
					printf_error(sysbuf);
				
				MemFile_Clear(dataFile);
				s8* instInfo;
				
				if (MemFile_LoadFile(dataFile, Dir_File("Sample.wav"))) {
					printf_warning_align("Sample not found", "%s", Dir_File("Sample.wav"));
					getchar();
				}
				
				instInfo = Lib_MemMem(dataFile->data, dataFile->dataSize, "inst", 4);
				
				if (instInfo) {
					Config_SPrintf("\n # Instrument Info\n");
					Config_WriteVar_Int("basenote", instInfo[8]);
					Config_WriteVar_Int("finetune", instInfo[9]);
					MemFile_SaveFile(config, Dir_File("config.cfg"));
				} else {
					if (dataFile->dataSize == 0)
						printf_warning_align("Audio", "Empty File [%s]", Dir_File("Sample.wav"));
				}
			} Dir_Leave();
		}
	} Dir_Leave();
	
	for (s32 j = 0; j < sBankNum; j++) {
		char* replacedName = NULL;
		printf_progress("Update SoundFont", j + 1, sBankNum);
		
		MemFile_Clear(config);
		MemFile_LoadFile_String(config, sBankFiles[j]);
		for (s32 i = 0; i < sSortID; i++) {
			name = gSampleInfo[i].dublicate == NULL ? gSampleInfo[i].name : gSampleInfo[i].dublicate->name;
			sprintf(buff, "0x%X", sSortedSampleTbl[i]->sampleAddr);
			if (String_Replace(config->data, buff, name)) {
				replacedName = name;
			}
		}
		
		config->dataSize = strlen(config->data);
		MemFile_SaveFile_String(config, sBankFiles[j]);
		
		// Rename SFX To their samples
		if (String_MemMem(sBankFiles[j], "-Sfx")) {
			char* tempName = TempPrintf("%s%d-%s.cfg", String_GetPath(sBankFiles[j]), String_GetInt(String_GetBasename(sBankFiles[j])), replacedName);
			printf_debug_align(
				"Rename",
				"[%s] -> [%s]",
				sBankFiles[j],
				tempName
			);
			renamer_remove(sBankFiles[j], tempName);
		}
		
		// Rename Inst to their primary sample
		if ((String_MemMem(sBankFiles[j], "-Inst") || String_MemMem(sBankFiles[j], "-Drum")) && (String_MemMem(config->data, "Inst_") || String_MemMem(config->data, "Perc_"))) {
			char instName[256] = { 0 };
			char* tempName;
			
			String_Copy(instName, Config_GetString(config, "prim_sample"));
			String_Remove(instName, strlen("Inst_"));
			String_Replace(instName, "_Prim", "");
			String_Replace(instName, "Soft", "");
			String_Replace(instName, "Hard", "");
			String_Replace(instName, "Mute", "");
			String_Replace(instName, "Open", "");
			String_Replace(instName, "_Hi", "Var");
			
			if (instName[0] == 0)
				printf_error("String maniplation failed for instrument");
			
			tempName = TempPrintf("%s%d-%s.cfg", String_GetPath(sBankFiles[j]), String_GetInt(String_GetBasename(sBankFiles[j])), instName);
			
			printf_debug_align(
				"Rename",
				"[%s] -> [%s]",
				sBankFiles[j],
				tempName
			);
			renamer_remove(sBankFiles[j], tempName);
		}
	}
	
	Rom_Dump_Samples_PatchWavFiles(dataFile, config);
}

void Rom_Dump(Rom* rom) {
	MemFile dataFile;
	MemFile config;
	RomFile rf;
	
	MemFile_Malloc(&dataFile, 0x460000); // Slightly larger than audiotable
	MemFile_Malloc(&config, 0x25000);
	
	printf_info_align("Dump Rom", PRNT_YELW "%s", rom->file.info.name);
	Dir_Enter("rom/");
	#if 1
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
				Dir_Enter("GameState_%s/", gStateName[i]); {
					if (Rom_Extract(&dataFile, rf, Dir_File("state.zovl")))
						Rom_Config_GameState(&config, &rom->stateTable[i], gStateName[i], Dir_File("config.cfg"));
				} Dir_Leave();
			}
			
			printf_info("Player");
			Dir_Enter("Player/"); {
				rf.size = ReadBE(rom->kaleidoTable[1].vromEnd) - ReadBE(rom->kaleidoTable[1].vromStart);
				rf.data = SegmentedToVirtual(0x0, ReadBE(rom->kaleidoTable[1].vromStart));
				
				Rom_Extract(&dataFile, rf, Dir_File("EnPlayer.zovl"));
				Rom_Config_Player(&config, &rom->kaleidoTable[1], "Player", Dir_File("config.cfg"));
			} Dir_Leave();
		} Dir_Leave();
		
		Dir_Enter("scene/"); {
			for (s32 i = 0; i < SCENE_ID_MAX; i++) {
				rf = Dma_RomFile_Scene(rom, i);
				
				if (rf.size == 0)
					continue;
				
				printf_progress("Scene", i + 1, SCENE_ID_MAX);
				Dir_Enter("0x%02X-%s/", i, gSceneName[i]); {
					if (Rom_Extract(&dataFile, rf, Dir_File("scene.zscene"))) {
						u32* seg;
						u32 roomNum;
						u32 roomListSeg;
						u32* vromSeg;
						
						Rom_Config_Scene(&config, &rom->sceneTable[i], gSceneName[i], Dir_File("config.cfg"));
						SetSegment(0x2, rf.data);
						seg = dataFile.data;
						
						while (1) {
							if ((seg[0] & 0xFF) == 0x04) {
								break;
							}
							if ((seg[0] & 0xFF) == 0x14) {
								printf_error_align("Scene", "Failed finding room list");
							}
							seg++;
						}
						
						roomNum = (seg[0] & 0xFF00) >> 8;
						roomListSeg = ReadBE(seg[1]) & 0xFFFFFF;
						
						for (s32 j = 0; j < roomNum; j++) {
							char* out = Dir_File("room_%d.zroom", j);
							
							vromSeg = SegmentedToVirtual(0x2, roomListSeg + 8 * j);
							printf_debugExt_align("Room Extract", "Scene Segment %X", VirtualToSegmented(0x2, vromSeg));
							Rom_Extract(
								&dataFile,
								Rom_GetRomFile(rom, vromSeg[0], vromSeg[1]),
								out
							);
						}
					}
				} Dir_Leave();
			}
		} Dir_Leave();
		SetSegment(0x2, NULL);
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

static void Rom_Build_SoundFont(Rom* rom, MemFile* dataFile, MemFile* config) {
	ItemList bankList;
	
	Dir_ItemList(&bankList, true);
	ItemList_Free(&bankList);
}

void Rom_Build(Rom* rom) {
	MemFile dataFile;
	MemFile config;
	ItemList itemList;
	
	MemFile_Malloc(&dataFile, 0x460000);
	MemFile_Malloc(&config, 0x25000);
	
	Dir_Enter("sound/"); {
		Dir_Enter("soundfont"); {
			Dir_ItemList(&itemList, true);
			
			for (s32 i = 0; i < itemList.num; i++) {
				Dir_Enter(itemList.item[i]); {
					Rom_Build_SoundFont(rom, &dataFile, &config);
				} Dir_Leave();
			}
			
			ItemList_Free(&itemList);
		} Dir_Leave();
	} Dir_Leave();
}

Rom* Rom_New(char* romName) {
	Rom* rom = Lib_Calloc(0, sizeof(struct Rom));
	
	Assert(rom != NULL);
	
	if (MemFile_LoadFile(&rom->file, romName)) {
		printf_error_align("Load Rom", "%s", romName);
	}
	
	if (rom->file.dataSize < MbToBin(64)) {
		printf_warning_align("Rom Size", "[%.2fMb / %.2fMb]", BinToMb(rom->file.dataSize), 64.0f);
		printf_warning("Bad things " PRNT_REDD "might " PRNT_RSET "happen...");
	}
	
	SetSegment(0x0, rom->file.data);
	rom->dmaTable = SegmentedToVirtual(0x0, gDmaDataTable);
	rom->objectTable = SegmentedToVirtual(0x0, gObjectTable);
	rom->actorTable = SegmentedToVirtual(0x0, gActorOverlayTable);
	rom->stateTable = SegmentedToVirtual(0x0, gGameStateOverlayTable);
	rom->sceneTable = SegmentedToVirtual(0x0, gSceneTable);
	rom->kaleidoTable = SegmentedToVirtual(0x0, gKaleidoTable);
	
	#if 0
		{
			u16* getVal;
			getVal = SegmentedToVirtual(0x0, 0xB5A4AC);
			_Sequence_SegmentRomStart = ReadBE(getVal[1]) << 16 | ReadBE(getVal[3]);
			getVal = SegmentedToVirtual(0x0, 0xB5A4C0);
			_SoundFont_SegmentRomStart = ReadBE(getVal[1]) << 16 | ReadBE(getVal[3]);
			if ( (ReadBE(getVal[1]) & 0xFF00) == 0x2400)
				_SoundFont_SegmentRomStart -= 0x10000;
			getVal = SegmentedToVirtual(0x0, 0xB5A4D4);
			_SampleBank_SegmentRomStart = ReadBE(getVal[1]) << 16 | ReadBE(getVal[3]);
		}
	#endif
	
	return rom;
}

Rom* Rom_Free(Rom* rom) {
	MemFile_Free(&rom->file);
	memset(rom, 0, sizeof(struct Rom));
	
	return NULL;
}