#define __EXTLIB_C__
#include "ExtLib.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

#ifdef __COMFLAG__
    #ifdef _WIN32
	#undef _WIN32
    #endif
#endif

// ExtendedLibrary

// Print
PrintfSuppressLevel gPrintfSuppress = 0;
char* sPrintfPrefix = "ExtLib";
u8 sPrintfType = 1;
u8 gPrintfProgressing;
u32 overFlowTestA;
u8* sSegment[255];
char sCurrentPath[1024];
char* sPrintfPreType[][4] = {
	{
		NULL,
		NULL,
		NULL,
		NULL,
	},
	{
		">",
		">",
		">",
		">"
	}
};
DirParam sDirParam;

// Segment
void SetSegment(const u8 id, void* segment) {
	sSegment[id] = segment;
}

void* SegmentedToVirtual(const u8 id, void32 ptr) {
	if (sSegment[id] == NULL)
		printf_error("Segment 0x%X == NULL", id);
	
	return &sSegment[id][ptr];
}

void32 VirtualToSegmented(const u8 id, void* ptr) {
	return (uPtr)ptr - (uPtr)sSegment[id];
}

// Dir
void Dir_SetParam(DirParam w) {
	sDirParam |= w;
}

void Dir_UnsetParam(DirParam w) {
	sDirParam &= ~(w);
}

void Dir_Set(char* path, ...) {
	va_list args;
	
	memset(sCurrentPath, 0, 1024);
	va_start(args, path);
	vsnprintf(sCurrentPath, ArrayCount(sCurrentPath), path, args);
	va_end(args);
}

void Dir_Enter(char* fmt, ...) {
	va_list args;
	char buffer[256 * 4];
	
	va_start(args, fmt);
	vsnprintf(buffer, ArrayCount(buffer), fmt, args);
	va_end(args);
	
	if (!(sDirParam & DIR__MAKE_ON_ENTER)) {
		if (!Dir_Stat(buffer)) {
			printf_warning("%08X", sDirParam);
			printf_error("Could not enter folder [%s%s]", sCurrentPath, buffer);
		}
	}
	
	String_Merge(sCurrentPath, buffer);
	
	if (sDirParam & DIR__MAKE_ON_ENTER) {
		Dir_MakeCurrent();
	}
}

void Dir_Leave(void) {
	sCurrentPath[strlen(sCurrentPath) - 1] = '\0';
	String_Copy(sCurrentPath, String_GetPath(sCurrentPath));
}

void Dir_Make(char* dir, ...) {
	char argBuf[256 * 4];
	va_list args;
	
	va_start(args, dir);
	vsnprintf(argBuf, ArrayCount(argBuf), dir, args);
	va_end(args);
	
	MakeDir(tprintf("%s%s", sCurrentPath, argBuf));
}

void Dir_MakeCurrent(void) {
	struct stat st = { 0 };
	
	if (stat(sCurrentPath, &st) == -1) {
		#ifdef _WIN32
			if (mkdir(sCurrentPath)) {
				printf_error_align("mkdir", "%s", sCurrentPath);
			}
		#else
			if (mkdir(sCurrentPath, 0700)) {
				printf_error_align("mkdir", "%s", sCurrentPath);
			}
		#endif
	}
}

char* Dir_Current(void) {
	return sCurrentPath;
}

char* Dir_File(char* fmt, ...) {
	static char buffer[16][256 * 4];
	static u32 bufID;
	char argBuf[256 * 4];
	va_list args;
	
	bufID++;
	bufID = bufID % 16;
	
	va_start(args, fmt);
	vsnprintf(argBuf, ArrayCount(argBuf), fmt, args);
	va_end(args);
	
	String_Copy(buffer[bufID], tprintf("%s%s", sCurrentPath, argBuf));
	
	return buffer[bufID];
}

s32 Dir_Stat(char* dir) {
	struct stat st = { 0 };
	
	if (stat(tprintf("%s%s", sCurrentPath, dir), &st) == -1)
		return 0;
	
	return 1;
}

static bool __isDir(char* path) {
	struct stat st = { 0 };
	
	stat(path, &st);
	
	if (S_ISDIR(st.st_mode)) {
		return true;
	}
	
	return false;
}

void Dir_ItemList(ItemList* itemList, bool isPath) {
	DIR* dir = opendir(sCurrentPath);
	u32 bufSize = 0;
	struct dirent* entry;
	
	*itemList = (ItemList) { 0 };
	
	if (dir == NULL)
		printf_error_align("Could not opendir()", "%s", sCurrentPath);
	
	while ((entry = readdir(dir)) != NULL) {
		if (isPath) {
			if (__isDir(Dir_File(entry->d_name))) {
				if (entry->d_name[0] == '.')
					continue;
				itemList->num++;
				bufSize += strlen(entry->d_name) + 2;
			}
		} else {
			if (!__isDir(Dir_File(entry->d_name))) {
				itemList->num++;
				bufSize += strlen(entry->d_name) + 2;
			}
		}
	}
	
	if (itemList->num) {
		u32 i = 0;
		dir = opendir(sCurrentPath);
		itemList->buffer = Lib_Malloc(0, bufSize);
		itemList->item = Lib_Malloc(0, sizeof(char*) * itemList->num);
		
		while ((entry = readdir(dir)) != NULL) {
			if (isPath) {
				if (__isDir(Dir_File(entry->d_name))) {
					if (entry->d_name[0] == '.')
						continue;
					String_Copy(&itemList->buffer[itemList->writePoint], entry->d_name);
					itemList->item[i++] = &itemList->buffer[itemList->writePoint];
					itemList->writePoint += strlen(itemList->item[i - 1]) + 1;
				}
			} else {
				if (!__isDir(Dir_File(entry->d_name))) {
					String_Copy(&itemList->buffer[itemList->writePoint], entry->d_name);
					itemList->item[i++] = &itemList->buffer[itemList->writePoint];
					itemList->writePoint += strlen(itemList->item[i - 1]) + 1;
				}
			}
		}
	}
}

void MakeDir(char* dir, ...) {
	struct stat st = { 0 };
	char buffer[256 * 4];
	
	va_list args;
	
	va_start(args, dir);
	vsnprintf(buffer, ArrayCount(buffer), dir, args);
	
	if (stat(buffer, &st) == -1) {
		#ifdef _WIN32
			if (mkdir(buffer)) {
				printf_error_align("mkdir", "%s", buffer);
			}
		#else
			if (mkdir(buffer, 0700)) {
				printf_error_align("mkdir", "%s", buffer);
			}
		#endif
	}
	va_end(args);
}

char* CurWorkDir(void) {
	static char buf[256 * 4];
	
	if (getcwd(buf, sizeof(buf)) == NULL) {
		printf_error("Could not get CurWorkDir");
	}
	
	for (s32 i = 0; i < strlen(buf); i++) {
		if (buf[i] == '\\')
			buf[i] = '/';
	}
	
	String_Merge(buf, "/");
	
	return buf;
}

// ItemList
void ItemList_Free(ItemList* itemList) {
	if (itemList->item != NULL)
		free(itemList->item);
	
	if (itemList->buffer)
		free(itemList->buffer);
	
	*itemList = (ItemList) { 0 };
}

// printf
char* tprintf(char* fmt, ...) {
	static char buffer[16][256 * 4];
	static u32 id;
	
	id = (id + 1) % 16;
	
	va_list args;
	
	va_start(args, fmt);
	vsnprintf(buffer[id], ArrayCount(buffer[id]), fmt, args);
	va_end(args);
	
	return buffer[id];
}

void printf_SetSuppressLevel(PrintfSuppressLevel lvl) {
	gPrintfSuppress = lvl;
}

void printf_SetPrefix(char* fmt) {
	sPrintfPrefix = fmt;
}

void printf_SetPrintfTypes(const char* d, const char* w, const char* e, const char* i) {
	sPrintfType = 0;
	sPrintfPreType[sPrintfType][0] = String_Generate(d);
	sPrintfPreType[sPrintfType][1] = String_Generate(w);
	sPrintfPreType[sPrintfType][2] = String_Generate(e);
	sPrintfPreType[sPrintfType][3] = String_Generate(i);
}

void printf_toolinfo(const char* toolname, const char* fmt, ...) {
	if (gPrintfSuppress >= PSL_NO_INFO)
		return;
	
	u32 strln = strlen(toolname);
	u32 rmv = 0;
	u32 tmp = strln;
	va_list args;
	
	for (s32 i = 0; i < strln; i++) {
		if (rmv) {
			if (toolname[i] != 'm') {
				tmp--;
			} else {
				tmp -= 2;
				rmv = false;
			}
		} else {
			if (toolname[i] == '\e' && toolname[i + 1] == '[') {
				rmv = true;
				strln--;
			}
		}
	}
	
	strln = tmp;
	
	va_start(args, fmt);
	
	printf(PRNT_GRAY "[>]--");
	for (s32 i = 0; i < strln; i++)
		printf("-");
	printf("------[>]\n");
	
	printf(" |   ");
	printf(PRNT_CYAN "%s" PRNT_GRAY, toolname);
	printf("       |\n");
	
	printf("[>]--");
	for (s32 i = 0; i < strln; i++)
		printf("-");
	printf("------[>]\n" PRNT_RSET);
	
	vprintf(
		fmt,
		args
	);
	va_end(args);
}

static void __printf_call(u32 type) {
	char* color[4] = {
		PRNT_PRPL,
		PRNT_YELW,
		PRNT_REDD,
		PRNT_CYAN
	};
	
	printf(
		"%s"
		PRNT_GRAY "["
		"%s%s"
		PRNT_GRAY "]: "
		PRNT_RSET,
		sPrintfPrefix,
		color[type],
		sPrintfPreType[sPrintfType][type]
	);
}

void printf_debug(const char* fmt, ...) {
	if (gPrintfSuppress > PSL_DEBUG)
		return;
	
	if (gPrintfProgressing) {
		printf("\n");
		gPrintfProgressing = false;
	}
	
	va_list args;
	
	va_start(args, fmt);
	__printf_call(0);
	vprintf(
		fmt,
		args
	);
	printf(PRNT_RSET "\n");
	va_end(args);
}

void printf_debug_align(const char* info, const char* fmt, ...) {
	if (gPrintfSuppress > PSL_DEBUG)
		return;
	
	if (gPrintfProgressing) {
		printf("\n");
		gPrintfProgressing = false;
	}
	
	va_list args;
	
	va_start(args, fmt);
	__printf_call(0);
	printf(
		"%-16s " PRNT_RSET,
		info
	);
	vprintf(
		fmt,
		args
	);
	printf(PRNT_RSET "\n");
	va_end(args);
}

void printf_warning(const char* fmt, ...) {
	if (gPrintfSuppress >= PSL_NO_WARNING)
		return;
	
	if (gPrintfProgressing) {
		printf("\n");
		gPrintfProgressing = false;
	}
	
	va_list args;
	
	va_start(args, fmt);
	__printf_call(1);
	vprintf(
		fmt,
		args
	);
	printf("\n");
	va_end(args);
}

void printf_warning_align(const char* info, const char* fmt, ...) {
	if (gPrintfSuppress >= PSL_NO_WARNING)
		return;
	
	if (gPrintfProgressing) {
		printf("\n");
		gPrintfProgressing = false;
	}
	
	va_list args;
	
	va_start(args, fmt);
	__printf_call(1);
	printf(
		"%-16s " PRNT_RSET,
		info
	);
	vprintf(
		fmt,
		args
	);
	printf(PRNT_RSET "\n");
	va_end(args);
}

void printf_error(const char* fmt, ...) {
	if (gPrintfSuppress < PSL_NO_ERROR) {
		if (gPrintfProgressing) {
			printf("\n");
			gPrintfProgressing = false;
		}
		
		va_list args;
		
		printf("\n");
		va_start(args, fmt);
		__printf_call(2);
		vprintf(
			fmt,
			args
		);
		printf("\n");
		va_end(args);
	}
	
	#ifdef _WIN32
		printf(PRNT_RSET "Press any key to exit...");
		getchar();
	#endif
	exit(EXIT_FAILURE);
}

void printf_error_align(const char* info, const char* fmt, ...) {
	if (gPrintfSuppress < PSL_NO_ERROR) {
		if (gPrintfProgressing) {
			printf("\n");
			gPrintfProgressing = false;
		}
		
		va_list args;
		
		va_start(args, fmt);
		__printf_call(2);
		printf(
			"%-16s " PRNT_RSET,
			info
		);
		vprintf(
			fmt,
			args
		);
		printf(PRNT_RSET "\n");
		va_end(args);
	}
	
	#ifdef _WIN32
		printf(PRNT_RSET "Press any key to exit...");
		getchar();
	#endif
	exit(EXIT_FAILURE);
}

void printf_info(const char* fmt, ...) {
	if (gPrintfSuppress >= PSL_NO_INFO)
		return;
	
	if (gPrintfProgressing) {
		printf("\n");
		gPrintfProgressing = false;
	}
	va_list args;
	
	va_start(args, fmt);
	__printf_call(3);
	vprintf(
		fmt,
		args
	);
	printf("\n");
	va_end(args);
}

void printf_info_align(const char* info, const char* fmt, ...) {
	if (gPrintfSuppress >= PSL_NO_INFO)
		return;
	
	if (gPrintfProgressing) {
		printf("\n");
		gPrintfProgressing = false;
	}
	va_list args;
	
	va_start(args, fmt);
	__printf_call(3);
	printf(
		"%-16s " PRNT_RSET,
		info
	);
	vprintf(
		fmt,
		args
	);
	printf(PRNT_RSET "\n");
	va_end(args);
}

void printf_progress(const char* info, u32 a, u32 b) {
	if (gPrintfSuppress >= PSL_NO_INFO)
		return;
	
	printf("\r");
	__printf_call(3);
	printf(
		"%-16s " PRNT_RSET,
		info
	);
	printf("[%d / %d]", a, b);
	gPrintfProgressing = true;
	
	if (a == b) {
		gPrintfProgressing = false;
		printf("\n");
	}
}

s32 printf_get_answer(void) {
	char ans = 0;
	
	while (ans != 'n' && ans!= 'N' && ans != 'y' && ans != 'Y') {
		printf("\r" PRNT_GRAY "[" PRNT_DGRY "<" PRNT_GRAY "]: " PRNT_RSET);
		if (scanf("%c", &ans)) (void)0;
	}
	
	if (ans == 'N' || ans == 'n')
		return 0;
	
	return 1;
}

void printf_WinFix() {
	#ifdef _WIN32
		system("\0");
	#endif
}

// Lib
void* Lib_MemMem(const void* haystack, size_t haystackSize, const void* needle, size_t needleSize) {
	if (haystack == NULL || needle == NULL)
		return NULL;
	register char* cur, * last;
	const char* cl = (const char*)haystack;
	const char* cs = (const char*)needle;
	
	/* we need something to compare */
	if (haystackSize == 0 || needleSize == 0)
		return NULL;
	
	/* "s" must be smaller or equal to "l" */
	if (haystackSize < needleSize)
		return NULL;
	
	/* special case where s_len == 1 */
	if (needleSize == 1)
		return memchr(haystack, (int)*cs, haystackSize);
	
	/* the last position where its possible to find "s" in "l" */
	last = (char*)cl + haystackSize - needleSize;
	
	for (cur = (char*)cl; cur <= last; cur++) {
		if (*cur == *cs && memcmp(cur, cs, needleSize) == 0)
			return cur;
	}
	
	return NULL;
}

void* Lib_MemMemCase(void* haystack, size_t haystackSize, void* needle, size_t needleSize) {
	if (haystack == NULL || needle == NULL)
		return NULL;
	register char* cur, * last;
	char* cl = (char*)haystack;
	char* cs = (char*)needle;
	
	/* we need something to compare */
	if (haystackSize == 0 || needleSize == 0)
		return NULL;
	
	/* "s" must be smaller or equal to "l" */
	if (haystackSize < needleSize)
		return NULL;
	
	/* special case where s_len == 1 */
	if (needleSize == 1)
		return memchr(haystack, (int)*cs, haystackSize);
	
	/* the last position where its possible to find "s" in "l" */
	last = (char*)cl + haystackSize - needleSize;
	
	for (cur = (char*)cl; cur <= last; cur++) {
		if (tolower(*cur) == tolower(*cs) && String_CaseComp(cur, cs, needleSize) == 0)
			return cur;
	}
	
	return NULL;
}

void Lib_ByteSwap(void* src, s32 size) {
	u32 buffer[16] = { 0 };
	u8* temp = (u8*)buffer;
	u8* srcp = src;
	
	for (s32 i = 0; i < size; i++) {
		temp[size - i - 1] = srcp[i];
	}
	
	for (s32 i = 0; i < size; i++) {
		srcp[i] = temp[i];
	}
}

void* Lib_Malloc(void* data, s32 size) {
	data = malloc(size);
	
	if (data == NULL) {
		printf_error("Could not allocate [0x%X] bytes.", size);
	}
	
	return data;
}

void* Lib_Calloc(void* data, s32 size) {
	data = Lib_Malloc(data, size);
	if (data != NULL) {
		memset(data, 0, size);
	}
	
	return data;
}

void* Lib_Realloc(void* data, s32 size) {
	data = realloc(data, size);
	
	if (data == NULL) {
		printf_error("Could not reallocate to [0x%X] bytes.", size);
	}
	
	return data;
}

void* Lib_Free(void* data) {
	if (data)
		free(data);
	
	return NULL;
}

s32 Lib_Touch(char* file) {
	MemFile mem = MemFile_Initialize();
	
	if (MemFile_LoadFile(&mem, file)) {
		return 1;
	}
	
	if (MemFile_SaveFile(&mem, file)) {
		MemFile_Free(&mem);
		
		return 1;
	}
	
	MemFile_Free(&mem);
	
	return 0;
}

s32 Lib_ParseArguments(char* argv[], char* arg, u32* parArg) {
	s32 i = 1;
	
	if (parArg != NULL)
		*parArg = 0;
	
	while (argv[i] != NULL) {
		if (!String_IsDiff(argv[i], arg)) {
			if (parArg != NULL)
				*parArg =  i + 1;
			
			return i + 1;
		}
		
		i++;
	}
	
	return 0;
}

// File
void* File_Load(void* destSize, char* filepath) {
	s32 size;
	void* dest;
	s32* ss = destSize;
	
	FILE* file = fopen(filepath, "rb");
	
	if (file == NULL) {
		printf_error("Could not open file [%s]", filepath);
	}
	
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	dest = Lib_Malloc(0, size);
	if (dest == NULL) {
		printf_error("Failed to malloc [0x%X] bytes to store data from [%s].", size, filepath);
	}
	rewind(file);
	if (fread(dest, sizeof(char), size, file)) {
	}
	fclose(file);
	
	ss[0] = size;
	
	return dest;
}

void File_Save(char* filepath, void* src, s32 size) {
	FILE* file = fopen(filepath, "w");
	
	printf_debugExt_align("Save", "%s", filepath);
	
	if (file == NULL) {
		printf_error("Failed to fopen file [%s].", filepath);
	}
	
	fwrite(src, sizeof(char), size, file);
	fclose(file);
}

void* File_Load_ReqExt(void* size, char* filepath, const char* ext) {
	if (Lib_MemMem(filepath, strlen(filepath), ext, strlen(ext))) {
		return File_Load(size, filepath);
	}
	printf_error("[%s] does not match extension [%s]", filepath, ext);
	
	return 0;
}

void File_Save_ReqExt(char* filepath, void* src, s32 size, const char* ext) {
	if (Lib_MemMem(filepath, strlen(filepath), ext, strlen(ext))) {
		File_Save(filepath, src, size);
		
		return;
	}
	
	printf_error("[%s] does not match extension [%s]", src, ext);
}

// MemFile
MemFile MemFile_Initialize() {
	return (MemFile) { 0 };
}

void MemFile_Malloc(MemFile* memFile, u32 size) {
	memset(memFile, 0, sizeof(struct MemFile));
	memFile->data = malloc(size);
	memset(memFile->data, 0, size);
	
	if (memFile->data == NULL) {
		printf_warning("Failed to malloc [0x%X] bytes.", size);
	}
	
	memFile->memSize = size;
}

void MemFile_Realloc(MemFile* memFile, u32 size) {
	if (memFile->memSize > size)
		return;
	printf_debugExt_align("memSize", "%08X", memFile->memSize);
	printf_debug_align("reqSize", "%08X", size);
	// Make sure to have enough space
	if (size < memFile->memSize + 0x10000) {
		size += 0x10000;
	}
	
	memFile->data = realloc(memFile->data, size);
	memFile->memSize = size;
	printf_debug_align("newSize", "%08X", size);
}

void MemFile_Rewind(MemFile* memFile) {
	memFile->seekPoint = 0;
}

s32 MemFile_Write(MemFile* dest, void* src, u32 size) {
	if (dest->seekPoint + size > dest->memSize) {
		printf_warning("DataSize exceeded MemSize while writing to MemFile.\n\tDataSize: [0x%X]\n\tMemSize: [0x%X]", dest->dataSize, dest->memSize);
		
		return 1;
	}
	if (dest->seekPoint + size > dest->dataSize) {
		dest->dataSize = dest->seekPoint + size;
	}
	memcpy(&dest->cast.u8[dest->seekPoint], src, size);
	dest->seekPoint += size;
	
	return 0;
}

s32 MemFile_Printf(MemFile* dest, const char* fmt, ...) {
	char buffer[1024 * 8];
	va_list args;
	
	va_start(args, fmt);
	vsnprintf(
		buffer,
		ArrayCount(buffer),
		fmt,
		args
	);
	va_end(args);
	
	return MemFile_Write(dest, buffer, strlen(buffer));
}

s32 MemFile_Read(MemFile* src, void* dest, u32 size) {
	if (src->seekPoint + size > src->dataSize) {
		printf_debugExt("Extended dataSize");
		
		return 1;
	}
	
	memcpy(dest, &src->cast.u8[src->seekPoint], size);
	src->seekPoint += size;
	
	return 0;
}

s32 MemFile_LoadFile(MemFile* memFile, char* filepath) {
	u32 tempSize;
	FILE* file = fopen(filepath, "rb");
	struct stat sta;
	
	if (file == NULL) {
		printf_warning("Failed to fopen file [%s].", filepath);
		
		return 1;
	}
	
	printf_debugExt_align("File", "%s", filepath);
	
	fseek(file, 0, SEEK_END);
	tempSize = ftell(file);
	
	if (memFile->data == NULL) {
		MemFile_Malloc(memFile, tempSize);
		memFile->memSize = memFile->dataSize = tempSize;
		if (memFile->data == NULL) {
			printf_warning("Failed to malloc MemFile.\n\tAttempted size is [0x%X] bytes to store data from [%s].", tempSize, filepath);
			
			return 1;
		}
	} else {
		if (memFile->memSize < tempSize)
			MemFile_Realloc(memFile, tempSize * 2);
		memFile->dataSize = tempSize;
	}
	
	rewind(file);
	if (fread(memFile->data, 1, memFile->dataSize, file)) {
	}
	fclose(file);
	stat(filepath, &sta);
	memFile->info.age = sta.st_mtime;
	if (memFile->info.name)
		free(memFile->info.name);
	memFile->info.name = String_Generate(filepath);
	
	printf_debug_align("Ptr", "%08X", memFile->data);
	printf_debug_align("Size", "%08X", memFile->dataSize);
	
	return 0;
}

s32 MemFile_LoadFile_String(MemFile* memFile, char* filepath) {
	u32 tempSize;
	FILE* file = fopen(filepath, "r");
	struct stat sta;
	
	if (file == NULL) {
		printf_warning("Failed to fopen file [%s].", filepath);
		
		return 1;
	}
	
	printf_debugExt("File: [%s]", filepath);
	
	fseek(file, 0, SEEK_END);
	tempSize = ftell(file);
	
	if (memFile->data == NULL) {
		MemFile_Malloc(memFile, tempSize);
		memFile->memSize = memFile->dataSize = tempSize;
		if (memFile->data == NULL) {
			printf_warning("Failed to malloc MemFile.\n\tAttempted size is [0x%X] bytes to store data from [%s].", tempSize, filepath);
			
			return 1;
		}
	} else {
		MemFile_Realloc(memFile, tempSize);
		memFile->dataSize = tempSize;
	}
	
	rewind(file);
	if (fread(memFile->data, 1, memFile->dataSize, file)) {
	}
	fclose(file);
	stat(filepath, &sta);
	memFile->info.age = sta.st_mtime;
	if (memFile->info.name)
		free(memFile->info.name);
	memFile->info.name = String_Generate(filepath);
	
	printf_debug("Ptr: %08X", memFile->data);
	printf_debug("Size: %08X", memFile->dataSize);
	
	return 0;
}

s32 MemFile_SaveFile(MemFile* memFile, char* filepath) {
	FILE* file = fopen(filepath, "wb");
	
	printf_debugExt_align("Save", "%s", filepath);
	
	if (file == NULL) {
		printf_warning("Failed to fopen file [%s] for writing.", filepath);
		
		return 1;
	}
	
	fwrite(memFile->data, sizeof(char), memFile->dataSize, file);
	fclose(file);
	
	return 0;
}

s32 MemFile_SaveFile_String(MemFile* memFile, char* filepath) {
	FILE* file = fopen(filepath, "w");
	
	printf_debugExt_align("Save String", "%s", filepath);
	
	if (file == NULL) {
		printf_warning("Failed to fopen file [%s] for writing.", filepath);
		
		return 1;
	}
	
	fwrite(memFile->data, sizeof(char), memFile->dataSize, file);
	fclose(file);
	
	return 0;
}

s32 MemFile_LoadFile_ReqExt(MemFile* memFile, char* filepath, const char* ext) {
	if (Lib_MemMem(filepath, strlen(filepath), ext, strlen(ext))) {
		return MemFile_LoadFile(memFile, filepath);
	}
	printf_warning("[%s] does not match extension [%s]", filepath, ext);
	
	return 1;
}

s32 MemFile_SaveFile_ReqExt(MemFile* memFile, char* filepath, s32 size, const char* ext) {
	if (Lib_MemMem(filepath, strlen(filepath), ext, strlen(ext))) {
		return MemFile_SaveFile(memFile, filepath);
	}
	
	printf_warning("[%s] does not match extension [%s]", filepath, ext);
	
	return 1;
}

void MemFile_Free(MemFile* memFile) {
	if (memFile->data) {
		if (memFile->info.name)
			free(memFile->info.name);
		free(memFile->data);
		
		memset(memFile, 0, sizeof(struct MemFile));
	}
}

void MemFile_Clear(MemFile* memFile) {
	memset(memFile->data, 0, memFile->memSize);
	memFile->dataSize = 0;
	memFile->seekPoint = 0;
}

#define __EXT_STR_MAX 32
// String
u32 String_GetHexInt(char* string) {
	return strtol(string, NULL, 16);
}

s32 String_GetInt(char* string) {
	return strtol(string, NULL, 10);
}

f32 String_GetFloat(char* string) {
	return strtod(string, NULL);
}

s32 String_GetLineCount(char* str) {
	s32 line = 1;
	s32 i = 0;
	
	while (str[i++] != '\0') {
		if (str[i] == '\n' && str[i + 1] != '\0')
			line++;
	}
	
	return line;
}

s32 String_CaseComp(char* a, char* b, u32 compSize) {
	u32 wow = 0;
	
	for (s32 i = 0; i < compSize; i++) {
		wow = tolower(a[i]) - tolower(b[i]);
		if (wow)
			return 1;
	}
	
	return 0;
}

char* String_GetLine(char* str, s32 line) {
	#define __EXT_BUFFER(a) buffer[Wrap(index + a, 0, __EXT_STR_MAX - 1)]
	static char* buffer[__EXT_STR_MAX];
	static s32 index;
	s32 iLine = -1;
	s32 i = 0;
	s32 j = 0;
	char* ret;
	
	while (str[i] != '\0') {
		j = 0;
		if (str[i] != '\n') {
			while (str[i + j] != '\n' && str[i + j] != '\0') {
				j++;
			}
			
			iLine++;
			
			if (iLine == line) {
				break;
			}
			
			i += j;
		} else {
			i++;
		}
	}
	
	__EXT_BUFFER(0) = Lib_Calloc(0, j * 2);
	memcpy(__EXT_BUFFER(0), &str[i], j);
	Lib_Free(__EXT_BUFFER(-(__EXT_STR_MAX / 2)));
	ret = __EXT_BUFFER(0);
	index = Wrap(index + 1, 0, __EXT_STR_MAX - 1);
	
	return ret;
	#undef __EXT_BUFFER
}

char* String_GetWord(char* str, s32 word) {
	#define __EXT_BUFFER(a) buffer[Wrap(index + a, 0, __EXT_STR_MAX - 1)]
	static char* buffer[__EXT_STR_MAX];
	static s32 index;
	s32 iWord = -1;
	s32 i = 0;
	s32 j = 0;
	char* ret;
	
	while (str[i] != '\0') {
		j = 0;
		if (str[i + j] > ' ') {
			while (str[i + j] > ' ') {
				j++;
			}
			
			iWord++;
			
			if (iWord == word) {
				break;
			}
			
			i += j;
		} else {
			i++;
		}
	}
	
	if (__EXT_BUFFER(0))
		__EXT_BUFFER(0) = Lib_Free(__EXT_BUFFER(0));
	__EXT_BUFFER(0) = Lib_Calloc(0, j * 2);
	memcpy(__EXT_BUFFER(0), &str[i], j);
	ret = __EXT_BUFFER(0);
	index = Wrap(index + 1, 0, __EXT_STR_MAX - 1);
	
	return ret;
	#undef __EXT_BUFFER
}

char* String_Line(char* str, s32 line) {
	s32 iLine = -1;
	s32 i = 0;
	s32 j = 0;
	
	while (str[i] != '\0') {
		j = 0;
		if (str[i] != '\n') {
			while (str[i + j] != '\n' && str[i + j] != '\0') {
				j++;
			}
			
			iLine++;
			
			if (iLine == line) {
				break;
			}
			
			i += j;
		} else {
			i++;
		}
	}
	
	return &str[i];
}

char* String_Word(char* str, s32 word) {
	s32 iWord = -1;
	s32 i = 0;
	s32 j = 0;
	
	while (str[i] != '\0') {
		j = 0;
		if (str[i + j] > ' ') {
			while (str[i + j] > ' ') {
				j++;
			}
			
			iWord++;
			
			if (iWord == word) {
				break;
			}
			
			i += j;
		} else {
			i++;
		}
	}
	
	return &str[i];
}

void String_GetLine2(char* dest, char* str, s32 line) {
	s32 iLine = -1;
	s32 i = 0;
	s32 j = 0;
	
	while (str[i] != '\0') {
		j = 0;
		if (str[i] != '\n') {
			while (str[i + j] != '\n' && str[i + j] != '\0') {
				j++;
			}
			
			iLine++;
			
			if (iLine == line) {
				break;
			}
			
			i += j;
		} else {
			i++;
		}
	}
	
	memcpy(dest, &str[i], j);
}

void String_GetWord2(char* dest, char* str, s32 word) {
	s32 iWord = -1;
	s32 i = 0;
	s32 j = 0;
	
	while (str[i] != '\0') {
		j = 0;
		if (str[i] != ' ' && str[i] != '\t') {
			while (str[i + j] != ' ' && str[i + j] != '\t' && str[i + j] != '\0') {
				j++;
			}
			
			iWord++;
			
			if (iWord == word) {
				break;
			}
			
			i += j;
		} else {
			i++;
		}
	}
	
	memcpy(dest, &str[i], j);
}

void String_CaseToLow(char* s, s32 i) {
	for (s32 k = 0; k < i; k++) {
		if (s[k] >= 'A' && s[k] <= 'Z') {
			s[k] = s[k] + 32;
		}
	}
}

void String_CaseToUp(char* s, s32 i) {
	for (s32 k = 0; k < i; k++) {
		if (s[k] >= 'a' && s[k] <= 'z') {
			s[k] = s[k] - 32;
		}
	}
}

void String_GetSlashAndPoint(char* src, s32* slash, s32* point) {
	s32 strSize = strlen(src);
	
	for (s32 i = strSize; i > 0; i--) {
		if (*point == 0 && src[i] == '.') {
			*point = i;
		}
		if (src[i] == '/' || src[i] == '\\') {
			*slash = i;
			break;
		}
	}
}

char* String_GetPath(char* src) {
	static char buffer[32][256 * 4];
	static s32 index;
	s32 point = 0;
	s32 slash = 0;
	
	String_GetSlashAndPoint(src, &slash, &point);
	
	index++;
	index = index % 32;
	
	if (slash == 0)
		slash = -1;
	
	memset(buffer[index], 0, slash + 2);
	memcpy(buffer[index], src, slash + 1);
	
	return buffer[index];
}

char* String_GetBasename(char* src) {
	#define __EXT_BUFFER(a) buffer[Wrap(index + a, 0, __EXT_STR_MAX - 1)]
	static char* buffer[__EXT_STR_MAX];
	static s32 index;
	s32 point = 0;
	s32 slash = 0;
	char* ret;
	
	String_GetSlashAndPoint(src, &slash, &point);
	
	if (slash == 0)
		slash = -1;
	
	__EXT_BUFFER(0) = Lib_Calloc(0, slash + 0x10);
	memset(__EXT_BUFFER(0), 0, point - slash);
	memcpy(__EXT_BUFFER(0), &src[slash + 1], point - slash - 1);
	Lib_Free(__EXT_BUFFER(-(__EXT_STR_MAX / 2)));
	ret = __EXT_BUFFER(0);
	index = Wrap(index + 1, 0, __EXT_STR_MAX - 1);
	
	return ret;
}

char* String_GetFilename(char* src) {
	#define __EXT_BUFFER(a) buffer[Wrap(index + a, 0, __EXT_STR_MAX - 1)]
	static char* buffer[__EXT_STR_MAX];
	static s32 index;
	s32 point = 0;
	s32 slash = 0;
	char* ret;
	
	String_GetSlashAndPoint(src, &slash, &point);
	
	if (slash == 0)
		slash = -1;
	
	__EXT_BUFFER(0) = Lib_Calloc(0, slash + 0x10);
	memset(__EXT_BUFFER(0), 0, strlen(src) - slash);
	memcpy(__EXT_BUFFER(0), &src[slash + 1], strlen(src) - slash - 1);
	Lib_Free(__EXT_BUFFER(-(__EXT_STR_MAX / 2)));
	ret = __EXT_BUFFER(0);
	index = Wrap(index + 1, 0, __EXT_STR_MAX - 1);
	
	return ret;
	
	#undef __EXT_BUFFER
}

void String_Insert(char* point, char* insert) {
	s32 insLen = strlen(insert);
	char* insEnd = point + insLen;
	s32 remLen = strlen(point);
	
	memmove(insEnd, point, remLen + 1);
	insEnd[remLen] = 0;
	memcpy(point, insert, insLen);
}

void String_Remove(char* point, s32 amount) {
	char* get = point + amount;
	s32 len = strlen(get);
	
	memcpy(point, get, strlen(get));
	point[len] = 0;
}

s32 String_Replace(char* src, char* word, char* replacement) {
	s32 diff = 0;
	char* ptr;
	
	ptr = String_MemMem(src, word);
	
	while (ptr != NULL) {
		String_Remove(ptr, strlen(word));
		String_Insert(ptr, replacement);
		ptr = String_MemMem(src, word);
		diff = true;
	}
	
	return diff;
}

void String_SwapExtension(char* dest, char* src, const char* ext) {
	String_Copy(dest, String_GetPath(src));
	String_Merge(dest, String_GetBasename(src));
	String_Merge(dest, ext);
}

char* String_GetSpacedArg(char* argv[], s32 cur) {
	#define STRING_BUFFER(a) buffer[Wrap(index + a, 0, __EXT_STR_MAX - 1)]
	static char* buffer[__EXT_STR_MAX];
	char tempBuf[1024];
	static s32 index;
	s32 i = cur + 1;
	
	if (argv[i] && argv[i][0] != '-' && argv[i][1] != '-') {
		if (STRING_BUFFER(0) != NULL)
			free(STRING_BUFFER(0));
		
		String_Copy(tempBuf, argv[cur]);
		
		while (argv[i] && argv[i][0] != '-' && argv[i][1] != '-') {
			String_Merge(tempBuf, " ");
			String_Merge(tempBuf, argv[i++]);
		}
		
		STRING_BUFFER(0) = String_Generate(tempBuf);
		index++;
		
		return STRING_BUFFER(-1);
	}
	
	return argv[cur];
	
	#undef STRING_BUFFER
	#undef CONDITION
}

// Config
s32 Config_GetBool(MemFile* memFile, char* boolName) {
	char* ptr;
	
	ptr = String_MemMem(memFile->data, boolName);
	if (ptr) {
		char* word = String_GetWord(ptr, 2);
		if (!String_IsDiff(word, "true")) {
			return true;
		}
		if (!String_IsDiff(word, "false")) {
			return false;
		}
	}
	
	printf_warning("[%s] is missing bool [%s]", memFile->info.name, boolName);
	
	return -1;
}

s32 Config_GetOption(MemFile* memFile, char* stringName, char* strList[]) {
	char* ptr;
	char* word;
	s32 i = 0;
	
	ptr = String_MemMem(memFile->data, stringName);
	if (ptr) {
		word = String_GetWord(ptr, 2);
		while (strList[i] != NULL && !String_MemMem(word, strList[i]))
			i++;
		
		if (strList != NULL)
			return i;
	}
	
	printf_warning("[%s] is missing option [%s]", memFile->info.name, stringName);
	
	return -1;
}

s32 Config_GetInt(MemFile* memFile, char* intName) {
	char* ptr;
	
	ptr = String_MemMem(memFile->data, intName);
	if (ptr) {
		return String_GetInt(String_GetWord(ptr, 2));
	}
	
	printf_warning("[%s] is missing integer [%s]", memFile->info.name, intName);
	
	return 404040404;
}

char* Config_GetString(MemFile* memFile, char* stringName) {
	char* ptr;
	
	ptr = String_MemMem(memFile->data, stringName);
	if (ptr) {
		return String_GetWord(ptr, 2);
	}
	
	printf_warning("[%s] is missing string [%s]", memFile->info.name, stringName);
	
	return NULL;
}