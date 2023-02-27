
#########  AVR Project Makefile Template   #########
######                                        ######
######    Copyright (C) 2003-2005,Pat Deegan, ######
######            Psychogenic Inc             ######
######          All Rights Reserved           ######
######                                        ######
###### You are free to use this code as part  ######
###### of your own applications provided      ######
###### you keep this copyright notice intact  ######
###### and acknowledge its authorship with    ######
###### the words:                             ######
######                                        ######
###### "Contains software by Pat Deegan of    ######
###### Psychogenic Inc (www.psychogenic.com)" ######
######                                        ######
###### If you use it as part of a web site    ######
###### please include a link to our site,     ######
###### http://electrons.psychogenic.com  or   ######
###### http://www.psychogenic.com             ######
######                                        ######
####################################################


##### This Makefile will make compiling Atmel AVR 
##### micro controller projects simple with Linux 
##### or other Unix workstations and the AVR-GCC 
##### tools.
#####
##### It supports C, C++ and Assembly source files.
#####
##### Customize the values as indicated below and :
##### make
##### make disasm 
##### make stats 
##### make hex
##### make writeflash
##### make gdbinit
##### or make clean
#####
##### See the http://electrons.psychogenic.com/ 
##### website for detailed instructions


####################################################
#####                                          #####
#####              Configuration               #####
#####                                          #####
##### Customize the values in this section for #####
##### your project. MCU, PROJECTNAME and       #####
##### PRJSRC must be setup for all projects,   #####
##### the remaining variables are only         #####
##### relevant to those needing additional     #####
##### include dirs or libraries and those      #####
##### who wish to use the avrdude programmer   #####
#####                                          #####
##### See http://electrons.psychogenic.com/    #####
##### for further details.                     #####
#####                                          #####
####################################################


#####         Target Specific Details          #####
#####     Customize these for your project     #####

# Name of target controller 
# (e.g. 'at90s8515', see the available avr-gcc mmcu 
# options for possible values)
MCU=atmega32u4

# id to use with programmer
# default: PROGRAMMER_MCU=$(MCU)
# In case the programer used, e.g avrdude, doesn't
# accept the same MCU name as avr-gcc (for example
# for ATmega8s, avr-gcc expects 'atmega8' and 
# avrdude requires 'm8')
PROGRAMMER_MCU=atmega32u4

# Name of our project
# (use a single word, e.g. 'myproject')
PROJECTNAME=libLEEA

# Source files
# List C/C++/Assembly source files:
# (list all files to compile, e.g. 'a.c b.cpp as.S'):
# Use .cc, .cpp or .C suffix for C++ files, use .S 
# (NOT .s !!!) for assembly source code files.

SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)
SRCPP=$(wildcard *.cpp)
OBJPP=$(SRCPP:.cpp=.o)


# additional includes (e.g. -I/path/to/mydir)
INC=-I../include/

# libraries to link in (e.g. -lmylib)
LIBS=../lib

# Optimization level, 
# use s (size opt), 1, 2, 3 or 0 (off)
OPTLEVEL=s


#####      AVR Dude 'writeflash' options       #####
#####  If you are using the avrdude program
#####  (http://www.bsdhome.com/avrdude/) to write
#####  to the MCU, you can set the following config
#####  options and use 'make writeflash' to program
#####  the device.


# programmer id--check the avrdude for complete list
# of available opts.  These should include stk500,
# avr910, avrisp, bsd, pony and more.  Set this to
# one of the valid "-c PROGRAMMER-ID" values 
# described in the avrdude info page.
# 
AVRDUDE_PROGRAMMERID=avr109 -b57600 -D

# port--serial or parallel port to which your 
# hardware programmer is attached
#
AVRDUDE_PORT=/dev/ttyACM0


####################################################
#####                Config Done               #####
#####                                          #####
##### You shouldn't need to edit anything      #####
##### below to use the makefile but may wish   #####
##### to override a few of the flags           #####
##### nonetheless                              #####
#####                                          #####
####################################################


##### Flags ####

# HEXFORMAT -- format for .hex file output
HEXFORMAT=ihex

# compiler
CFLAGS=-I. $(INC) -g -mmcu=$(MCU) -O$(OPTLEVEL) \
	-ffunction-sections -fdata-sections             \
	-Wall -MMD -DUSB_VID=0x2341 -DUSB_PID=0x8036 -DARDUINO=101 \
	-Wa,-ahlms=$(firstword                  \
	$(filter %.lst, $(<:.c=.lst)))

# c++ specific flags
CPPFLAGS=-g -Os -Wall -mmcu=$(MCU)	\
	-fno-exceptions -ffunction-sections -fdata-sections \
	-MMD -DUSB_VID=0x2341 -DUSB_PID=0x8036 -DARDUINO=101 \
	-Wa,-ahlms=$(firstword         \
	$(filter %.lst, $(<:.cpp=.lst)))\


# assembler
ASMFLAGS =-I. $(INC) -mmcu=$(MCU)        \
	-x assembler-with-cpp            \
	-Wa,-gstabs,-ahlms=$(firstword   \
		$(<:.S=.lst) $(<.s=.lst))


# linker
LDFLAGS=-Wl,-Map,$(TRG).map -mmcu=$(MCU) \
	-lm $(LIBS)

##### executables ####
AR=avr-ar
CC=avr-gcc
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
SIZE=avr-size
AVRDUDE=avrdude
REMOVE=rm -f

##### automatic target names ####
TRG=$(PROJECTNAME).out
DUMPTRG=$(PROJECTNAME).s

HEXROMTRG=$(PROJECTNAME).hex 
HEXTRG=$(HEXROMTRG) $(PROJECTNAME).ee.hex
GDBINITFILE=gdbinit-$(PROJECTNAME)

# Define all object files.

# Start by splitting source files by type
#  C++
CPPFILES=$(filter %.cpp, $(PRJSRC))
CCFILES=$(filter %.cc, $(PRJSRC))
BIGCFILES=$(filter %.C, $(PRJSRC))
#  C
CFILES=$(filter %.c, $(PRJSRC))
#  Assembly
ASMFILES=$(filter %.S, $(PRJSRC))


# List all object files we need to create
OBJDEPS=$(CFILES:.c=.o)    \
	$(CPPFILES:.cpp=.o)\
	$(BIGCFILES:.C=.o) \
	$(CCFILES:.cc=.o)  \
	$(ASMFILES:.S=.o)

# Define all lst files.
LST=$(filter %.lst, $(OBJDEPS:.o=.lst))

# All the possible generated assembly 
# files (.s files)
GENASMFILES=$(filter %.s, $(OBJDEPS:.o=.s)) 


.SUFFIXES : .c .cc .cpp .C .o .out .s .S \
	.hex .ee.hex .h .hh .hpp


.PHONY:  clean 

# Make targets:
# all, disasm, stats, hex, writeflash/install, clean
all: $(PROJECTNAME).a

$(PROJECTNAME).a: $(OBJ) $(OBJPP)
	$(AR) rcs  $(LIBS)/$(PROJECTNAME).a $?


#### Generating assembly ####
# asm from C
%.s: %.c
	$(CC) -S $(CFLAGS) $< -o $@

# asm from (hand coded) asm
%.s: %.S
	$(CC) -S $(ASMFLAGS) $< > $@


# asm from C++
.cpp.s .cc.s .C.s :
	$(CC) -S $(CFLAGS) $(CPPFLAGS) $< -o $@



#### Generating object files ####
# object from C
.c.o: 
	$(CC) $(CFLAGS)  -c $< -o $@


# object from C++ (.cc, .cpp, .C files)
.cc.o .cpp.o .C.o :
	$(CC) $(CFLAGS)  $(CPPFLAGS) -c $< -o $@

# object from asm
.S.o :
	$(CC) $(ASMFLAGS) -c $< -o $@

#### Cleanup ####
clean:
	$(REMOVE) $(OBJ) $(OBJPP) *.d *.lst ../lib/*.a

#####                    EOF                   #####

