.PHONY: clean, flash, all
# CROSS_COMPILE =/home/vienchau/toolchain/arm-linux-uclibcgnueabi-20190305/arm-linux-uclibcgnueabi-20190305/bin/arm-linux-
CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
AS = $(CROSS_COMPILE)as

LIBRARY_PATH =/home/vienchau/build/local/lib
INCLUDE_PATH = /home/vienchau/build/local/include

STD = gnu99
CFLAGS =-Wall \
	-I./inc \
	-I$(INCLUDE_PATH) \
	-g -Wno-incompatible-pointer-types \
	-fPIC \
	-fsanitize=address \
	-fsanitize=leak \

	
LDFLAGS =-L$(LIBRARY_PATH) \
	-ljansson \
	

VPATH+=./src/

EXEDIR=./bin
OBJDIR = ./obj

OBJ += $(OBJDIR)/main.o \
	$(OBJDIR)/extract.o \
	$(OBJDIR)/utils.o \
	$(OBJDIR)/test_function.o \
	$(OBJDIR)/vec.o \



DEP = $(OBJ:%.o=%.d)

EXEC = extract
RM = rm

########################################################################
####################### Targets beginning here #########################
########################################################################
# Builds the app
$(EXEDIR)/$(EXEC): $(OBJ)
	@echo =============EXE PROCESS=============
	[ -d "./bin" ] && echo "Directory /path/to/dir exists." || mkdir ./bin	
	$(CC) $(CFLAGS) $^ -o $@  $(LDFLAGS)

-include $(DEP)

# Building rule for .o files and its .c/.cpp in combination with all .h
$(OBJDIR)/%.o: %.c 
	@echo =============OBJ PROCESS=============
	[ -d "./obj" ] && echo "Directory /path/to/dir exists." || mkdir ./obj	
	$(CC)  $(CFLAGS) -MMD -c $< -o  $@  

################### Cleaning rules for Unix-based OS ###################
# Cleans complete project

clean:
	$(RM) $(EXEDIR)/$(EXEC) $(OBJ) $(DEP)
 
flash: clean all
	@./$(EXEDIR)/$(EXEC)
