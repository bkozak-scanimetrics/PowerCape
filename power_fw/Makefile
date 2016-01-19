#!/usr/bin/make -f
###############################################################################
# Makefile for the cape's power firmware				      #
###############################################################################
###############################################################################
#			       EXTERNAL PROGRAMS			      #
###############################################################################
CC	:= avr-gcc
LD	:= avr-gcc
OBJCOPY := avr-objcopy
OBJDUMP := avr-objdump
SIZE	:= avr-size
###############################################################################
#				     VARS				      #
###############################################################################
PRG    := power
TARGET := atmega328p
CPUCLK := 8000000

DAY   := $(shell date +%d)
MONTH := $(shell date +%m)
YEAR  := $(shell date +%y)

DEFS	:= -I. -DF_CPU=$(CPUCLK) -DDAY=$(DAY) -DMONTH=$(MONTH) -DYEAR=$(YEAR)
CFLAGS	:= -std=c99 -Wall -mmcu=$(TARGET) $(DEFS)
LDFLAGS := -mmcu=$(TARGET)

NO_DEPS_TARGETS += clean directories dir_clean

ifeq ($(V),1)
  TRACE_CC =
  TRACE_LD =
  TRACE_AR =
  TRACE_AS =
  TRACE_DP =
  Q=
else
  TRACE_CC = @echo "  CC       " $<
  TRACE_LD = @echo "  LD       " $@
  TRACE_AR = @echo "  AR       " $@ $^
  TRACE_AS = @echo "  AS       " $<
  TRACE_DP = @echo "  DP       " $<
  Q=@
endif
###############################################################################
#				  BUILD DIRS				      #
###############################################################################
SRC_DIRS += src
INC_DIRS += src inc

BUILD_DIR := build
OBJ_DIR	  := $(BUILD_DIR)/obj
DEP_DIR	  := $(BUILD_DIR)/dep

ASM_GEN_DIR := $(BUILD_DIR)/asm_gen

LDFLAGS += -Wl,-Map,$(BUILD_DIR)/$(PRG).map
###############################################################################
#				  BUILD FILES				      #
###############################################################################
C_PATHS	  += $(foreach dir, $(SRC_DIRS),$(wildcard $(dir)/*.c))
C_FILES	  += $(foreach f, $(C_PATHS),$(notdir $(f)))
OBJ_FILES += $(foreach f,$(C_FILES),$(OBJ_DIR)/$(patsubst %.c,%.o,$(f)))
DEP_FILES += $(foreach f,$(C_FILES),$(DEP_DIR)/$(patsubst %.c,%.d,$(f)))
ASM_GEN	  += $(foreach f,$(C_FILES),$(ASM_GEN_DIR)/$(patsubst %.c,%.s,$(f)))

LIBS :=

INCLUDES += $(foreach f,$(INC_DIRS),-I$(f))
CFLAGS	 += $(INCLUDES)

vpath %.c $(SRC_DIRS)

C_DIRS += $(SRC_DIRS) $(INC_DIRS)

CLEAN_FILES += $(foreach dir,$(C_DIRS),$(wildcard $(dir)/*~))
CLEAN_FILES += Makefile~
CLEAN_DIRS  += $(BUILD_DIR)
###############################################################################
#                                  FUNCTIONS                                  #
###############################################################################
dep_obj = $(OBJ_DIR)/$(patsubst %.d,%.o,$(notdir $(1)))
###############################################################################
#				     RULES				      #
###############################################################################
all: optomized

optomized: CFLAGS += -DNDEBUG=1 -Os -flto
optomized: LDFLAGS += -Os -flto
optomized: $(BUILD_DIR)/$(PRG).elf lst text

debug: CFLAGS += -g -DDEBUG=1 -Os
debug: $(BUILD_DIR)/$(PRG).elf lst text

asg_gen: CFLAGS += -fverbose-asm
asm_gen: CFLAGS += -DNDEBUG=1 -Os -flto
asm_gen: $(ASM_GEN)

eeprom: ehex ebin esrec

lst: $(BUILD_DIR)/$(PRG).lst

directories: $(BUILD_DIR)/.dir_dummy $(OBJ_DIR)/.dir_dummy
directories: $(DEP_DIR)/.dir_dummy

.SECONDARY: $(OBJ_FILES)

%.dir_dummy:
	mkdir -p $(dir $(@))
	@touch $(@)

$(DEP_DIR)/%.d: %.c | $(DEP_DIR)/.dir_dummy
	$(TRACE_DP)
	$(Q)$(CC) $(CFLAGS) -MF $@ -M -MT "$(call dep_obj,$@) $@" $<

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)/.dir_dummy
	$(TRACE_CC)
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

$(ASM_GEN): $(ASM_GEN_DIR)/%.s : %.c | $(ASM_GEN_DIR)/.dir_dummy
	$(TRACE_CC)
	$(Q)$(CC) $(CFLAGS) -S $< -o $@


$(BUILD_DIR)/%.elf: $(OBJ_FILES) | $(BUILD_DIR)/.dir_dummy
	$(TRACE_LD)
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

$(BUILD_DIR)/%.lst: $(BUILD_DIR)/%.elf | $(BUILD_DIR)/.dir_dummy
	$(OBJDUMP) -h -S $< > $@
	$(SIZE) $<

# Rules for building the .text rom images

text: hex bin srec

hex:  $(BUILD_DIR)/$(PRG).hex
bin:  $(BUILD_DIR)/$(PRG).bin
srec: $(BUILD_DIR)/$(PRG).srec

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)/.dir_dummy
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

$(BUILD_DIR)/%.srec: $(BUILD_DIR)/%.elf | $(BUILD_DIR)/.dir_dummy
	$(OBJCOPY) -j .text -j .data -O srec $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)/.dir_dummy
	$(OBJCOPY) -j .text -j .data -O binary $< $@

# Rules for building the .eeprom rom images

ehex:  $(PRG)_eeprom.hex
ebin:  $(PRG)_eeprom.bin
esrec: $(PRG)_eeprom.srec

$(BUILD_DIR)/%_eeprom.hex: %.elf | $(BUILD_DIR)/.dir_dummy
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@ \
	|| { echo empty $@ not generated; exit 0; }

$(BUILD_DIR)/%_eeprom.srec: %.elf | $(BUILD_DIR)/.dir_dummy
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@ \
	|| { echo empty $@ not generated; exit 0; }

$(BUILD_DIR)/%_eeprom.bin: %.elf | $(BUILD_DIR)/.dir_dummy
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@ \
	|| { echo empty $@ not generated; exit 0; }

clean:
	rm -f $(CLEAN_FILES)
	rm -rf $(CLEAN_DIRS)

ifeq (,$(filter $(MAKECMDGOALS), $(NO_DEPS_TARGETS)))

#next two conditonals prevent make from running on dry run or when
#invoked for tab-completion
ifneq (n,$(findstring n,$(firstword $(MAKEFLAGS))))
ifneq (p,$(findstring p,$(firstword $(MAKEFLAGS))))
-include $(DEP_FILES)
endif
endif

endif
