#
# http://www.gnu.org/software/make/manual/make.html
#
#FFMPEG_ROOT = ../ffmpeg_min

#CC:=gcc
#INCLUDES:=$(shell pkg-config --cflags sdl)
#CFLAGS=  -g -Wall -c
# CFLAGS:=-Wall -ggdb
# LDFLAGS:=$(shell pkg-config --libs libavformat libavcodec libswresample libswscale libavutil sdl) -lm
# EXE:=tutorial01.out tutorial02.out tutorial03.out tutorial04.out\
# 	tutorial05.out tutorial06.out tutorial07.out

#
# This is here to prevent Make from deleting secondary files.
#

	
#### 1st Approach #####
#
# $< is the first dependency in the dependency list
# $@ is the target name
#
#SRC := src
#OBJ := obj


#SOURCES := $(wildcard ${FFMPEG_ROOT}/libavformat/*[!template].c ${FFMPEG_ROOT}/libavcodec/*[!template].c ${FFMPEG_ROOT}/libavutil/*[!template].c  ${FFMPEG_ROOT}/libswscale/*[!template].c ${FFMPEG_ROOT}/libswresample/*[!template].c) 

#OBJECTS := $(patsubst ${FFMPEG_ROOT}/libavformat/*[!template]%.c ${FFMPEG_ROOT}/libavcodec/*[!template]%.c ${FFMPEG_ROOT}/libavutil/*[!template]%.c  ${FFMPEG_ROOT}/libswscale/*[!template]%.c ${FFMPEG_ROOT}/libswresample/*[!template]%.c, ${FFMPEG_ROOT}/libavformat/*[!template]%.o ${FFMPEG_ROOT}/libavcodec/*[!template]%.o ${FFMPEG_ROOT}/libavutil/*[!template]%.o  ${FFMPEG_ROOT}/libswscale/*[!template]%.o ${FFMPEG_ROOT}/libswresample/*[!template]%.o, $(SOURCES))

#all: $(OBJECTS)
#    $(CC) $^ -o $@

#%.o: %.c
#    $(CC) -I${FFMPEG_ROOT}/ -c $< -o $@
#### 1st Approach #####

#### 2nd Approach #####
#VPATH = ${FFMPEG_ROOT}/libavformat:${FFMPEG_ROOT}/libavcodec:${FFMPEG_ROOT}/libavutil:${FFMPEG_ROOT}/libswscale:${FFMPEG_ROOT}/libswresample
#%.o: %.c

#SOURCES := $(wildcard *[!template].c)
#OBJECTS := $(patsubst %.c, %.o, $(SOURCES))

#all: $(OBJECTS)
#    $(CC) --I${FFMPEG_ROOT}/ -o  ffmpegPlayer ffmpegPlayer.c


#### 2nd Approach #####

#### original #####
#all: 
#	gcc -no-pie -g   /usr/local/lib/libx264.a  -I${FFMPEG_ROOT}/ -lpthread -lz -lm -ldl -lfdk-aac `sdl2-config --cflags --libs -Wdeprecated-declarations` -o  ffmpegPlayer ffmpegPlayer.c

# dirs:
# 	mkdir -p obj
# 	mkdir -p bin

# tags: *.c
# 	ctags *.c

# bin/%.out: obj/%.o
# 	$(CC) $(CFLAGS) $< $(LDFLAGS) -o $@

# obj/%.o : %.c
# 	$(CC) $(CFLAGS) $< $(INCLUDES) -c -o $@

#clean:
#	rm -f ffmpegPlayer

#### 3rd Approach #####
# Set project directory one level above of Makefile directory. $(CURDIR) is a GNU make variable containing the path to the current working directory
PROJDIR := $(realpath $(CURDIR)/../../ffmpeg)
SOURCEDIR := $(PROJDIR)
BUILDDIR := $(PROJDIR)/Build
CFLAGS := -g -Wno-uninitialized -Wno-attributes -Wno-deprecated-declarations -Wno-switch -Wno-switch-enum -Wno-pointer-sign -fPIC 
LDFLAGS := -lpthread -lz -lm -ldl -lfdk-aac 

# Name of the final executable
TARGET = decode_video

# Decide whether the commands will be shwon or not
VERBOSE = TRUE

# Create the list of directories
DIRS = samples ./  x86 
#libavformat libavcodec libavutil libswscale libswresample

SOURCEDIRS = $(foreach dir, $(DIRS), $(addprefix $(SOURCEDIR)/, $(dir)))
TARGETDIRS = $(foreach dir, $(DIRS), $(addprefix $(BUILDDIR)/, $(dir)))

# Generate the GCC includes parameters by adding -I before each source folder
INCLUDES = $(foreach dir, $(SOURCEDIRS), $(addprefix -I, $(dir)))

# Add this list to VPATH, the place make will look for the source files
VPATH = $(SOURCEDIRS)

# Create a list of *.c sources in DIRS
SOURCES = $(foreach dir,$(SOURCEDIRS),$(wildcard $(dir)/*[!template].c))
#SOURCES = $(foreach dir,$(SOURCEDIRS),$(wildcard $(dir)/*.c))

# Define objects for all sources
OBJS := $(subst $(SOURCEDIR),$(BUILDDIR),$(SOURCES:.c=.o))

# Define dependencies files for all objects
DEPS = $(OBJS:.o=.d)

# Name the compiler
CC = gcc

# OS specific part
ifeq ($(OS),Windows_NT)
	RM = del /F /Q 
	RMDIR = -RMDIR /S /Q
	MKDIR = -mkdir
	ERRIGNORE = 2>NUL || true
	SEP=\\
else
	RM = rm -rf 
	RMDIR = rm -rf 
	MKDIR = mkdir -p
	ERRIGNORE = 2>/dev/null
	SEP=/
endif

# Remove space after separator
PSEP = $(strip $(SEP))

# Hide or not the calls depending of VERBOSE
ifeq ($(VERBOSE),TRUE)
	HIDE =  
else
	HIDE = @
endif

# Define the function that will generate each rule
define generateRules
$(1)/%.o: %.c
	@echo Building $$@
	$(HIDE)$(CC) -c $(CFLAGS) $$(INCLUDES) -o $$(subst /,$$(PSEP),$$@) $$(subst /,$$(PSEP),$$<) -MMD 
endef

.PHONY: all clean directories 

all: directories $(TARGET)

$(TARGET): $(OBJS)
	$(HIDE)echo Linking $@
	$(HIDE)$(CC)  -o $(TARGET) $(OBJS) $(LDFLAGS)
	@printf "\n[$(GREEN)OK$(WHITE)] Binary : $(TARGET)\n"
	@echo "-------------------\n"
#	gcc -no-pie -g   /usr/local/lib/libx264.a  -I${FFMPEG_ROOT}/ -lpthread -lz -lm -ldl -lfdk-aac `sdl2-config --cflags --libs -Wdeprecated-declarations` -o  ffmpegPlayer ffmpegPlayer.c

#	$(HIDE)$(CC) $(OBJS) -o $(TARGET) -no-pie -g /usr/local/lib/libx264.a $(INCLUDES) -lpthread -lz -lm -ldl -lfdk-aac `sdl2-config --cflags --libs -Wdeprecated-declarations` $(TARGET).c 
#$(HIDE)$(CC) $(TARGET).c $(SOURCEDIR)/avpacket.c $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Include dependencies
-include $(DEPS)

# Generate rules
$(foreach targetdir, $(TARGETDIRS), $(eval $(call generateRules, $(targetdir))))

directories: 
	$(HIDE)$(MKDIR) $(subst /,$(PSEP),$(TARGETDIRS)) $(ERRIGNORE)

# Remove all objects, dependencies and executable files generated during the build
clean:
	$(HIDE)$(RMDIR) $(subst /,$(PSEP),$(TARGETDIRS)) $(ERRIGNORE)
	$(HIDE)$(RM) $(TARGET) $(ERRIGNORE)
	@echo Cleaning done ! 

