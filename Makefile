CFLAGS         := -Os -s -flto -Wall -I./
SOURCE_C       := $(shell find lib/* -type f -name '*.c')
SOURCE_O_WIN32 := $(foreach f,$(SOURCE_C:.c=.o),bin/win32/$f)
SOURCE_O_LINUX := $(foreach f,$(SOURCE_C:.c=.o),bin/linux/$f)

PRNT_DGRY := \e[90;2m
PRNT_GRAY := \e[0;90m
PRNT_DRED := \e[91;2m
PRNT_REDD := \e[0;91m
PRNT_GREN := \e[0;92m
PRNT_YELW := \e[0;93m
PRNT_BLUE := \e[0;94m
PRNT_PRPL := \e[0;95m
PRNT_CYAN := \e[0;96m
PRNT_RSET := \e[m

HEADER := lib/z64rom.h lib/z64audio.h lib/ExtLib.h

# Make build directories
$(shell mkdir -p bin/ $(foreach dir, \
	$(dir $(SOURCE_O_WIN32)) \
	$(dir $(SOURCE_O_LINUX)), $(dir)))

.PHONY: clean default win32 linux

default: linux
copyz64audio: tools/z64audio.exe tools/z64audio
all: copyz64audio linux-release win32-release
linux: copyz64audio $(SOURCE_O_LINUX) z64rom
win32: copyz64audio $(SOURCE_O_WIN32) bin/icon.o z64rom.exe

tools/z64audio.exe: ../z64audio/z64audio.exe
	@echo 'Update $<'
	@cp $< $@

tools/z64audio: ../z64audio/z64audio
	@echo 'Update $<'
	@cp $< $@

linux-release: linux
	@rm -f z64rom-linux.7z
	@mkdir -p bin/release-linux/
	@cp z64rom bin/release-linux/z64rom
	@cp -r tools/ bin/release-linux/
	@rm -f bin/release-linux/tools/z64audio.exe
	@upx -9 --lzma bin/release-linux/z64rom
	@7z a z64rom-linux.7z ./bin/release-linux/*

win32-release: win32
	@rm -f z64rom-win32.7z
	@mkdir -p bin/release-win32/
	@cp z64rom.exe bin/release-win32/z64rom.exe
	@cp -r tools/ bin/release-win32/
	@rm -f bin/release-win32/tools/z64audio
	@upx -9 --lzma bin/release-win32/z64rom.exe
	@7z a z64rom-win32.7z ./bin/release-win32/*
	

clear:
	@rm -f -R rom/*

clean:
	@echo "$(PRNT_RSET)rm $(PRNT_RSET)[$(PRNT_CYAN)$(shell find bin/* -type f)$(PRNT_RSET)]"
	@rm -f $(shell find bin/* -type f)
	@echo "$(PRNT_RSET)rm $(PRNT_RSET)[$(PRNT_CYAN)$(shell find z64ro* -type f -not -name '*.c*')$(PRNT_RSET)]"
	@rm -f $(shell find z64ro* -type f -not -name '*.c')
	@rm -f -R bin/*

# LINUX
bin/linux/%.o: %.c %.h $(HEADER)
	@echo "$(PRNT_RSET)$(PRNT_RSET)[$(PRNT_CYAN)$(notdir $@)$(PRNT_RSET)]"
	@gcc -c -o $@ $< $(CFLAGS)
	
bin/linux/%.o: %.c $(HEADER)
	@echo "$(PRNT_RSET)$(PRNT_RSET)[$(PRNT_CYAN)$(notdir $@)$(PRNT_RSET)]"
	@gcc -c -o $@ $< $(CFLAGS) -Wno-missing-braces

z64rom: z64rom.c $(SOURCE_O_LINUX)
	@echo "$(PRNT_RSET)$(PRNT_RSET)[$(PRNT_CYAN)$(notdir $@)$(PRNT_RSET)] [$(PRNT_CYAN)$(notdir $^)$(PRNT_RSET)]"
	@gcc -o $@ $^ $(CFLAGS) -lm

# WINDOWS32
bin/win32/%.o: %.c %.h $(HEADER)
	@echo "$(PRNT_RSET)$(PRNT_RSET)[$(PRNT_CYAN)$(notdir $@)$(PRNT_RSET)]"
	@i686-w64-mingw32.static-gcc -c -o $@ $< $(CFLAGS) -D_WIN32
	
bin/win32/%.o: %.c $(HEADER)
	@echo "$(PRNT_RSET)$(PRNT_RSET)[$(PRNT_CYAN)$(notdir $@)$(PRNT_RSET)]"
	@i686-w64-mingw32.static-gcc -c -o $@ $< $(CFLAGS) -D_WIN32 -Wno-missing-braces

bin/icon.o: lib/icon.rc lib/icon.ico
	@i686-w64-mingw32.static-windres -o $@ $<

z64rom.exe: z64rom.c bin/icon.o $(SOURCE_O_WIN32)
	@echo "$(PRNT_RSET)$(PRNT_RSET)[$(PRNT_CYAN)$(notdir $@)$(PRNT_RSET)] [$(PRNT_CYAN)$(notdir $^)$(PRNT_RSET)]"
	@i686-w64-mingw32.static-gcc -o $@ $^ $(CFLAGS) -lm -D_WIN32