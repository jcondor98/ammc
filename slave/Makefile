# AVR Multi Motor Control -- Paolo Lucchesi
# Makefile for slave controllers firmware
.POSIX:
.SUFFIXES: # Reset all implicit rules
.PRECIOUS: $(OBJDIR)/%.o # Keep objects when generated as intermediate targets

PROGNAME := ammc-slave

# Repository structure
SRCDIR := source
INCDIR := include
OBJDIR := objects
TGTDIR := target
INCDIR_COMMON := ../include

# Search paths
vpath %.cpp $(SRCDIR)
vpath %.s $(SRCDIR)

OBJECTS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(wildcard $(SRCDIR)/*.c))

# AVR Compiler and Programmer setup
# Atmel ATMega2560 @16MHz C++11
CC := avr-g++
AS := avr-g++
CFLAGS := -Wall --std=c++11 -DF_CPU=16000000UL -O3 -funsigned-char\
 -funsigned-bitfields  -fshort-enums\
 -mmcu=atmega2560 -I$(INCDIR) -I$(INCDIR_COMMON) -DAVR -D__AVR_3_BYTE_PC__
ASFLAGS := -x assembler-with-cpp $(CFLAGS)

# Avrdude settings
AVRDUDE := avrdude
AVRDUDE_MCU := atmega2560
AVRDUDE_PROGRAMMER := wiring
AVRDUDE_BAUDRATE := 115200
AVRDUDE_PORT := /dev/ttyUSB0
AVRDUDE_NO_VERIFY := -V

AVRDUDE_FLAGS := -p $(AVRDUDE_MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)\
  -b $(AVRDUDE_BAUDRATE) $(AVRDUDE_NO_VERIFY) $(AVRDUDE_VERBOSE)\
	$(AVRDUDE_ERASE_COUNTER) -D -q

# Use these as functions
avrdude_write_flash = -U flash:w:$(strip $(1)):i
avrdude_write_eeprom = -U eeprom:w:$(strip $(1)):i


$(OBJDIR)/%.o:	%.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/%.o:	%.s
	$(AS) $(ASFLAGS) -c -o $@ $<

%.eep:	%.elf
	avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load"\
	  --change-section-lma .eeprom=0 --no-change-warnings\
	  -O ihex $< $@

%.hex:	%.eep %.elf
	avr-objcopy -O ihex -R .eeprom $(patsubst %.hex, %.elf, $@) $@


$(TGTDIR)/$(PROGNAME).elf:	$(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(TGTDIR)/test-twi.elf: $(OBJDIR)/twi.o
	$(CC) $(CFLAGS) -o $@ $^ test/test-twi.c

$(TGTDIR)/test-dcmotor.elf: $(OBJDIR)/dcmotor_phy.o
	$(CC) $(CFLAGS) -o $@ $^ test/test-dcmotor.c

$(TGTDIR)/test-encoder.elf: $(OBJDIR)/dcmotor_phy.o
	$(CC) $(CFLAGS) -Itest/include -o $@ $^ test/test-encoder.c \
		test/source/serial.c test/source/ringbuffer.c

all: $(TGTDIR)/$(PROGNAME).elf;

flash: $(TGTDIR)/$(PROGNAME).hex;
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(call avrdude_write_flash, $<)\
	  $(call avrdude_write_eeprom, $(patsubst %.hex, %.eep, $<))

test-dcmotor: $(TGTDIR)/test-dcmotor.hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(call avrdude_write_flash, $<)\
	  $(call avrdude_write_eeprom, $(patsubst %.hex, %.eep, $<))

test-twi: $(TGTDIR)/test-twi.hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(call avrdude_write_flash, $<)\
	  $(call avrdude_write_eeprom, $(patsubst %.hex, %.eep, $<))

test-encoder: $(TGTDIR)/test-encoder.hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(call avrdude_write_flash, $<)\
	  $(call avrdude_write_eeprom, $(patsubst %.hex, %.eep, $<))

test-pid:
	gcc -ggdb -Iinclude/ -I../include -Itest/framework -o test/test-dcmotor-pid \
		source/dcmotor_pid.c test/framework/test-framework.c \
		test/test-dcmotor-pid.c


clean:	
	-rm -f $(OBJDIR)/*.o $(BINS) $(TGTDIR)/*


.PHONY:	clean all flash
