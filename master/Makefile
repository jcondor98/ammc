# AVR Multi Motor Control -- Paolo Lucchesi
# Makefile for master controller firmware
.POSIX:
.SUFFIXES: # Reset all implicit rules
.PRECIOUS: $(OBJDIR)/%.o # Keep objects when generated as intermediate targets

PROGNAME := ammc-master

# Repository structure
SRCDIR := source
INCDIR := include
OBJDIR := objects
RESDIR := resources
TGTDIR := target

# Search paths
vpath %.c $(SRCDIR)
vpath %.s $(SRCDIR)

OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(wildcard $(SRCDIR)/*.c))
TARGET := $(TGTDIR)/$(PROGNAME).elf

# AVR Compiler and Programmer setup
# Atmel ATMega2560 @16MHz, gnu99
CC := avr-gcc
AS := avr-gcc
CFLAGS := -Wall --std=gnu99 -DF_CPU=16000000UL -O3 -funsigned-char\
 -funsigned-bitfields  -fshort-enums -Wstrict-prototypes\
 -mmcu=atmega2560 -I$(INCDIR) -DAVR -D__AVR_3_BYTE_PC__
ASFLAGS := -x assembler-with-cpp $(CFLAGS)

# Avrdude settings
AVRDUDE := avrdude
AVRDUDE_PROGRAMMER := wiring
AVRDUDE_PORT := /dev/ttyACM0
AVRDUDE_CONFIG != find /usr/share/arduino/hardware -name avrdude.conf
AVRDUDE_NO_VERIFY := -V

AVRDUDE_FLAGS := -p m2560 -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER) -b\
 115200 $(AVRDUDE_NO_VERIFY) $(AVRDUDE_VERBOSE) $(AVRDUDE_ERASE_COUNTER)\
 -D -q -C $(AVRDUDE_CONFIG)

# Use these as functions
avrdude_write_flash = -U flash:w:$(strip $(1)):i
avrdude_write_eeprom = -U eeprom:w:$(strip $(1)):i


$(OBJDIR)/%.o:	%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/%.o:	%.s
	$(AS) $(ASFLAGS) -c -o $@ $<

%.eep:	%.elf
	avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load"\
	  --change-section-lma .eeprom=0 --no-change-warnings\
	  -O ihex $< $@

%.hex:	%.eep %.elf
	avr-objcopy -O ihex -R .eeprom $(patsubst %.hex, %.elf, $@) $@


$(TARGET):	$(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

all: $(TARGET) ;

flash: $(TARGET);
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(call avrdude_write_flash, $@)\
	  $(call avrdude_write_eeprom, $(patsubst %.hex, %.eep, $@))

clean:	
	rm -f $(OBJDIR)/*.o $(BINS) $(TGTDIR)/*


.PHONY:	clean all flash
