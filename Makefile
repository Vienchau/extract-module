.PHONY: clean, flash, all
# CROSS_COMPILE =/home/vienchau/toolchain/arm-linux-uclibcgnueabi-20190305/arm-linux-uclibcgnueabi-20190305/bin/arm-linux-
CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
AS = $(CROSS_COMPILE)as

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

VPATH+=./src/

EXEDIR=./bin
OBJDIR = ./obj

OBJ += $(OBJDIR)/main.o \
	$(OBJDIR)/extract.o \
	$(OBJDIR)/utils.o \

DEP = $(OBJ:%.o=%.d)

EXEC = extract
RM = rm

########################################################################
####################### Targets beginning here #########################
########################################################################
# Builds the app
$(EXEDIR)/$(EXEC): $(OBJ)
	@echo =============EXE PROCESS=============
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $^ -o $@  $(LDFLAGS)

-include $(DEP)

# Building rule for .o files and its .c/.cpp in combination with all .h
$(OBJDIR)/%.o: %.c 
	@echo =============OBJ PROCESS=============	
	$(CC)  $(CFLAGS) -MMD -c $< -o  $@  

################### Cleaning rules for Unix-based OS ###################
# Cleans complete project

clean:
	$(RM) $(EXEDIR)/$(EXEC) $(OBJ) $(DEP)
 
flash: clean all
	@./$(EXEDIR)/$(EXEC)
