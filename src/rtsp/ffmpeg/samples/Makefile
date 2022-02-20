
CC = gcc
OUT_FILE_NAME = ffmpeg.a
FFMPEG_DIR = ../../ffmpeg

CFLAGS=  -g -Wall -c 

TARGETS=$(OUT_FILE_NAME) $(TARGET)

INC = -I$(FFMPEG_DIR) -I../ffmpeg
OBJ_DIR=$(FFMPEG_DIR)
OUT_DIR=./

# Enumerating of every *.cpp as *.o and using that as dependency.	
# filter list of .c files in a directory.
# FILES =dump_l.c \
#	kter.c \
#
# $(OUT_FILE_NAME): $(patsubst %.c,$(OBJ_DIR)/%.o,$(wildcard $(FILES))) 


.DEFAULT_GOAL=all
.PHONY: all
all: $(TARGETS)


# Enumerating of every *.cpp as *.o and using that as dependency
$(OUT_FILE_NAME): $(patsubst %.c,%.o,$(wildcard $(FFMPEG_DIR)/*[!template].c))
	ar -r -o $(OUT_DIR)/$@ $^


#Compiling every *.cpp to *.o
%.o: %.c 
	$(CC) -c $(INC) $(CFLAGS) -o $@  $<
	
dirmake:
	@mkdir -p $(OUT_DIR)
	
clean:
	rm -f $(OBJ_DIR)/*.o $(OUT_DIR)/$(OUT_FILE_NAME) Makefile.bak
	rm -f  $(OBJS)

rebuild: clean build

CXX     = g++
TARGET = decode_video # define the name of the executable
SOURCES    = $(wildcard *.c $(FFMPEG_DIR)/*[!template].c)

CXXFLAGS   := -g -Wno-inconsistent-missing-override -Wno-uninitialized -Wno-attributes -Wno-fpermissive -Wno-deprecated-declarations -I$(FFMPEG_DIR)/ -no-pie 

CPPFLAGS   := -I$(FFMPEG_DIR)/ 
LDFLAGS   := -L$(FFMPEG_DIR) 
LDLIBS    := -lpthread -lm -ldl -lfdk-aac 

SLIBS     := $(OUT_FILE_NAME) 


# define list of objects
OBJSC=$(SOURCES:.cpp=.o)
OBJS=$(OBJSC:.cpp=.o)

# the target is obtained linking all .o files
all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CXXFLAGS) $(OBJS) $(SLIBS) $(LDFLAGS) $(LDLIBS) -o $(TARGET)

purge: clean
	rm -f $(TARGET)
	#rm -f $(OUT_FILE_NAME)
