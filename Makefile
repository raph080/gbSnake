# GBDK_LOCATION should be set when running make command. example:
# > make GBDK_LOCATION=/usr/Downloads/gbdk-2020
# if not set, the makefile will try to find the path if lcc exists in the env
GBDK_LOCATION = $(shell dirname $(shell dirname $(shell which lcc)))

LCC = $(GBDK_LOCATION)/bin/lcc
GCC = gcc
PNG2ASSET = $(GBDK_LOCATION)/bin/png2asset

# You can set flags for LCC here
# For example, you can uncomment the line below to turn on debug output
# LCCFLAGS += -debug

LCCFLAGS += -Wa-l -Wl-m -Wl-j

CC = $(LCC) $(LCCFLAGS)

# You can set the name of the .gb ROM file here
PROJECTNAME = snake

# project directories
BINDIR      = bin
SRCDIR      = src
BUILDDIR    = build
RESBUILDDIR = build/resources
RESDIR      = resources
VENDDIR     = vendors
MKDIRS      = $(BUILDDIR) $(BINDIR) $(RESBUILDDIR)

# gbt_player directories
GBTPDIR     = $(VENDDIR)/gbt_player
GBTPSRCDIR  = $(GBTPDIR)/src
GBTPINCDIR  = $(GBTPDIR)/include
GBTPASMDIR  = $(GBTPDIR)/assembly

# Add directory where image gets converted into (obj/)
# So they can be included with "#include <res/somefile.h>"
LCCFLAGS += -I$(BUILDDIR)
# add include/gbt_player.h as include
LCCFLAGS += -I$(GBTPINCDIR)

# gbt_player assembly files
GBTPASN     = $(foreach dir,$(GBTPASMDIR),$(notdir $(wildcard $(dir)/*.s)))
GBTPOBJS    = $(GBTPASN:%.s=$(BUILDDIR)/%.o)

# mod2gbt source file
MOD2GBTSRC  = $(GBTPSRCDIR)/mod2gbt.c
MOD2GBTOBJS	= $(MOD2GBTSRC:.c=.o)
MOD2GBT     = $(BINDIR)/mod2gbt

# template.mod
SNDMODS     = $(foreach dir,$(RESDIR),$(notdir $(wildcard $(dir)/*.mod)))
# build/resources/template.c
SNDSOURCES  = $(SNDMODS:%.mod=$(RESBUILDDIR)/%.c)
# build/template.o
SNDOBJS     = $(SNDSOURCES:$(RESBUILDDIR)/%.c=$(BUILDDIR)/%.o)


BINS	    = $(BINDIR)/$(PROJECTNAME).gb

# For png2asset: converting source pngs -> .c -> .o
BKGPNGS     = $(foreach dir,$(RESDIR),$(notdir $(wildcard $(dir)/*.bkg.png)))
BKGSOURCES  = $(BKGPNGS:%.bkg.png=$(RESBUILDDIR)/%.c)
BKGOBJS     = $(BKGSOURCES:$(RESBUILDDIR)/%.c=$(BUILDDIR)/%.o)

SPRPNGS     = $(foreach dir,$(RESDIR),$(notdir $(wildcard $(dir)/*.sprite.png)))
SPRSOURCES  = $(SPRPNGS:%.sprite.png=$(RESBUILDDIR)/%.c)
SPROBJS     = $(SPRSOURCES:$(RESBUILDDIR)/%.c=$(BUILDDIR)/%.o)
# template.mod
SNDMODS     = $(foreach dir,$(RESDIR),$(notdir $(wildcard $(dir)/*.mod)))
# build/resources/template.c
SNDSOURCES  = $(SNDMODS:%.mod=$(RESBUILDDIR)/%.c)
# build/template.o
SNDOBJS     = $(SNDSOURCES:$(RESBUILDDIR)/%.c=$(BUILDDIR)/%.o)

CSOURCES    = $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.c))) $(foreach dir,$(RESDIR),$(notdir $(wildcard $(dir)/*.c)))
ASMSOURCES  = $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.s)))
SRCOBJS       = $(CSOURCES:%.c=$(BUILDDIR)/%.o) $(ASMSOURCES:%.s=$(BUILDDIR)/%.o)

all: $(BINS)

# generate the compile.bat for window 
# make sure to run make clean before runing make compile.bat
compile.bat: Makefile
	rm -f compile.bat
	@echo "REM Automatically generated from Makefile" > compile.bat
	@make -sn | sed y/\\//\\\\/ | grep -v make >> compile.bat

$(MOD2GBT): $(GBTPSRCDIR)/mod2gbt.o
	$(GCC) -o $@ $<

$(GBTPSRCDIR)/mod2gbt.o: $(GBTPSRCDIR)/mod2gbt.c
	$(GCC) -c -o $@ $< $(FOO)

$(RESBUILDDIR)/%.c:      $(RESDIR)/%.mod
	$(MOD2GBT) $< $(basename $(notdir $@)) 2
	mkdir -p $(dir $@)
	mv output.c $@

# Use png2asset to convert the png into C formatted metasprite data
# -map                    : Use "map style" output, not metasprite
# -bpp 2                  : Use 2bpp output
# -c ...                  : Set C output file
# Convert metasprite .pngs in res/ -> .c files in obj/<platform ext>/src/
$(RESBUILDDIR)/%.c:	$(RESDIR)/%.bkg.png
	$(PNG2ASSET) $< -c $@ -map -bpp 2 -noflip

$(RESBUILDDIR)/%.c:	$(RESDIR)/%.sprite.png
	$(PNG2ASSET) $< -c $@ -map -bpp 2 -noflip -keep_duplicate_tiles -tiles_only

# Compile the pngs that were converted to .c files
# .c files in obj/res/ -> .o files in obj/
$(BUILDDIR)/%.o:	$(RESBUILDDIR)/%.c
	$(LCC) $(CFLAGS) -c -o $@ $<

# Compile .c files in "src/" to .o object files
$(BUILDDIR)/%.o:	$(SRCDIR)/%.c
	$(LCC) $(LCCFLAGS) -c -o $@ $<

# Compile .c files in "res/" to .o object files
$(BUILDDIR)/%.o:	$(RESDIR)/%.c
	$(LCC) $(CFLAGS) -c -o $@ $<

# Compile .s assembly files in "src/" to .o object files
$(BUILDDIR)/%.o:	$(SRCDIR)/%.s
	$(LCC) $(LCCFLAGS) -c -o $@ $<

# If needed, compile .c files in "src/" to .s assembly files
# (not required if .c is compiled directly to .o)
$(BUILDDIR)/%.s:	$(SRCDIR)/%.c
	$(LCC) $(LCCFLAGS) -S -o $@ $<

	# Compile .c files in "resources/" to .o object files
$(BUILDDIR)/%.o:	$(RESBUILDDIR)/%.c
	$(CC) -c -o $@ $<

# Compile .s gbt_player files from "gbt_player/include" to .o object files
$(BUILDDIR)/%.o:	$(GBTPASMDIR)/%.s
	$(CC) -c -o $@ $<


# Link the compiled object files into a .gb ROM file
$(BINS):	$(MOD2GBT) $(SNDOBJS) $(BKGOBJS) $(SPROBJS) $(SRCOBJS) $(GBTPOBJS)
	$(CC) -Wl-yt1 -Wl-yo4 -Wl-ya0 -o $(BINS) $(BKGOBJS) $(SPROBJS) $(SRCOBJS) $(SNDOBJS) $(GBTPOBJS)
	rm -f  $(BINDIR)/*.ihx 

clean:
	rm -rf  $(BUILDDIR) $(BINDIR)


# create necessary directories after Makefile is parsed but before build
# info prevents the command from being pasted into the makefile
$(info $(shell mkdir -p $(MKDIRS)))

