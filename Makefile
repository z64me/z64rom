CFLAGS         := -Os -s -flto -Wall -I./
SOURCE_C       := $(shell find lib/* -maxdepth 0 -type f -name '*.c')
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
all: linux win32
linux: $(SOURCE_O_LINUX) z64rom
win32: $(SOURCE_O_WIN32) bin/icon.o z64rom.exe

clear:
	@rm -f -R rom/*

clean:
	@echo "$(PRNT_RSET)rm $(PRNT_RSET)[$(PRNT_CYAN)$(shell find bin/* -type f)$(PRNT_RSET)]"
	@rm -f $(shell find bin/* -type f)
	@echo "$(PRNT_RSET)rm $(PRNT_RSET)[$(PRNT_CYAN)$(shell find z64ro* -type f -not -name '*.c*')$(PRNT_RSET)]"
	@rm -f $(shell find z64ro -type f -not -name '*.c')

# LINUX
bin/linux/%.o: %.c %.h $(HEADER)
	@echo "$(PRNT_RSET)$(PRNT_RSET)[$(PRNT_CYAN)$(notdir $@)$(PRNT_RSET)]"
	@gcc -c -o $@ $< $(CFLAGS)
	
bin/linux/%.o: %.c $(HEADER)
	@echo "$(PRNT_RSET)$(PRNT_RSET)[$(PRNT_CYAN)$(notdir $@)$(PRNT_RSET)]"
	@gcc -c -o $@ $< $(CFLAGS)

z64rom: z64rom.c $(SOURCE_O_LINUX)
	@echo "$(PRNT_RSET)$(PRNT_RSET)[$(PRNT_CYAN)$(notdir $@)$(PRNT_RSET)] [$(PRNT_CYAN)$(notdir $^)$(PRNT_RSET)]"
	@gcc -o $@ $^ $(CFLAGS) -lm

# WINDOWS32
bin/win32/%.o: %.c %.h $(HEADER)
	@echo "$(PRNT_RSET)$(PRNT_RSET)[$(PRNT_CYAN)$(notdir $@)$(PRNT_RSET)]"
	@i686-w64-mingw32.static-gcc -c -o $@ $< $(CFLAGS) -D_WIN32
	
bin/win32/%.o: %.c $(HEADER)
	@echo "$(PRNT_RSET)$(PRNT_RSET)[$(PRNT_CYAN)$(notdir $@)$(PRNT_RSET)]"
	@i686-w64-mingw32.static-gcc -c -o $@ $< $(CFLAGS) -D_WIN32

bin/icon.o: lib/icon.rc lib/icon.ico
	@i686-w64-mingw32.static-windres -o $@ $<

z64rom.exe: z64rom.c bin/icon.o $(SOURCE_O_WIN32)
	@echo "$(PRNT_RSET)$(PRNT_RSET)[$(PRNT_CYAN)$(notdir $@)$(PRNT_RSET)] [$(PRNT_CYAN)$(notdir $^)$(PRNT_RSET)]"
	@i686-w64-mingw32.static-gcc -o $@ $^ $(CFLAGS) -lm -D_WIN32