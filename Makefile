.PHONY: clean, flash, all
# Compiler settings - Can be customized.
# CROSS_COMPILE =/home/vienchau/toolchain/arm-linux-uclibcgnueabi-20190305/arm-linux-uclibcgnueabi-20190305/bin/arm-linux-
CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
AS = $(CROSS_COMPILE)as

LIBRARY_PATH =/home/vienchau/build/local/lib
INCLUDE_PATH =/home/vienchau/build/local/include 
STD = gnu99
CFLAGS =-Wall \
	-I./inc \
	-I$(INCLUDE_PATH) \
	-g -Wno-incompatible-pointer-types \
	-fPIC \
	 -DX64 \
	-fsanitize=address \
	-fsanitize=leak \
	-g

LDFLAGS =-L$(LIBRARY_PATH) \
	# -lasan 

VPATH+=./src/

EXEDIR=./bin
OBJDIR = ./obj

# LIB_GEN  = $(LIBDIR)/libscaninfo.so

OBJ += $(OBJDIR)/main.o \
	$(OBJDIR)/extract.o \
	$(OBJDIR)/utils.o \

EXEC = extract
RM = rm

########################################################################
####################### Targets beginning here #########################
########################################################################

# all: $(EXEDIR)/$(EXEC) $(LIB_GEN)

# Builds the app
$(EXEDIR)/$(EXEC): $(OBJ)
	@echo =============EXE PROCESS=============
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


# Building rule for .o files and its .c/.cpp in combination with all .h
$(OBJDIR)/%.o: %.c
	@echo =============OBJ PROCESS=============	
	$(CC) $(CFLAGS) -o $@ -c $^

################### Cleaning rules for Unix-based OS ###################
# Cleans complete project

clean:
	$(RM) $(EXEDIR)/$(EXEC) $(OBJ)

flash: clean all
	@./$(EXEDIR)/$(EXEC)
