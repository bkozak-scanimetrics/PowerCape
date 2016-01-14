#!/usr/bin/make -f
###############################################################################
# Makefile for the cape's power firmware				      #
###############################################################################
###############################################################################
#			       EXTERNAL PROGRAMS			      #
###############################################################################
CC	:= gcc
LD	:= gcc
###############################################################################
#				     VARS				      #
###############################################################################
PRG    := twiboot

DEFS	:=
CFLAGS	:= -Wall -Wno-unused-result
LDFLAGS :=

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
SRC_TREE += $(CURDIR)

SRC_DIRS += $(SRC_TREE)
INC_DIRS += $(SRC_TREE)

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
CLEAN_FILES += mpmboot $(PRG)
CLEAN_DIRS  += $(BUILD_DIR)
###############################################################################
#                                  FUNCTIONS                                  #
###############################################################################
dep_obj = $(OBJ_DIR)/$(patsubst %.d,%.o,$(notdir $(1)))
###############################################################################
#				     RULES				      #
###############################################################################
all: optomized

optomized: CFLAGS += -DNDEBUG=1 -Os
optomized: LDFLAGS += -Os
optomized: $(PRG)

asg_gen: CFLAGS += -fverbose-asm
asm_gen: CFLAGS += -DNDEBUG=1 -Os
asm_gen: $(ASM_GEN)

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

$(PRG): $(OBJ_FILES) | $(BUILD_DIR)/.dir_dummy
	$(TRACE_LD)
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)ln -sf $@ mpmboot

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
