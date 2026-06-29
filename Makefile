# parasite-pc - Parasite Eve (PS1, SLUS-006.62) matching decompilation.

VERSION    := USA
BASENAME   := main
TARGET_SHA := 452fb033f2eaa4b18aa20a5bca60b8125af3a37b

ROOT       := $(abspath .)
PY         ?= .venv/bin/python
SPLAT_CFG  := configs/$(VERSION)/$(BASENAME).yaml

BUILD      := build/$(VERSION)
ASM_DIR    := asm/$(VERSION)/$(BASENAME)
SRC_DIR    := src/$(BASENAME)
LD_SCRIPT  := linkers/$(VERSION)/$(BASENAME).ld
TARGET_BIN := assets/$(VERSION)/$(BASENAME).exe

ELF        := $(BUILD)/$(BASENAME).elf
OUT_BIN    := $(BUILD)/$(BASENAME).exe

CC_WRAPPER ?= tools/scripts/cc.sh
AS         = mipsel-none-elf-as
LD         = mipsel-none-elf-ld
OBJCOPY    = mipsel-none-elf-objcopy

ASM_SRCS := $(shell find $(ASM_DIR) -name '*.s' -not -path '*/nonmatchings/*' 2>/dev/null)
C_SRCS   := $(shell find $(SRC_DIR) -name '*.c' 2>/dev/null)

ASM_OBJS := $(ASM_SRCS:%.s=$(BUILD)/%.s.o)
C_OBJS   := $(C_SRCS:%=$(BUILD)/%.o)

OBJS := $(ASM_OBJS) $(C_OBJS)

.PHONY: all split build check ci clean distclean

all: check

split:
	rm -rf $(ASM_DIR) $(LD_SCRIPT) $(UNDEFINED_SYMS) $(UNDEFINED_FUNCS) $(UNDEFINED_MANUAL)
	@$(PY) -m splat split $(SPLAT_CFG)

build:
	@$(MAKE) split
	@$(MAKE) $(OUT_BIN)

# Assemble split MIPS asm with maspsx-aware path (no preprocessing needed -
# splat output already uses macro.inc and is GNU-as compatible).
$(BUILD)/asm/%.s.o: asm/%.s
	@mkdir -p $(dir $@)
	$(AS) -EL -G0 -mips4 -32 -no-pad-sections -Iinclude -I$(ASM_DIR) -o $@ $<

# Compile C with PSYQ GCC 2.7.2 -> maspsx -> GNU as.
$(BUILD)/src/%.c.o: src/%.c | $(BUILD)
	@mkdir -p $(dir $@)
	$(CC_WRAPPER) $< $@

$(BUILD):
	@mkdir -p $@

# Link to ELF using splat-generated linker script.
UNDEFINED_SYMS := linkers/$(VERSION)/undefined_syms_auto.$(BASENAME).txt
UNDEFINED_FUNCS := linkers/$(VERSION)/undefined_funcs_auto.$(BASENAME).txt
UNDEFINED_MANUAL := linkers/$(VERSION)/undefined_syms_manual.txt

$(ELF): $(OBJS) $(LD_SCRIPT) $(UNDEFINED_SYMS) $(UNDEFINED_FUNCS) $(UNDEFINED_MANUAL)
	$(LD) -EL -T $(LD_SCRIPT) -T $(UNDEFINED_SYMS) -T $(UNDEFINED_FUNCS) \
	      -T $(UNDEFINED_MANUAL) \
	      -Map $(BUILD)/$(BASENAME).map -o $@

$(UNDEFINED_MANUAL): configs/$(VERSION)/sym.$(BASENAME).txt $(SPLAT_CFG) tools/scripts/gen_linker_aliases.py
	@mkdir -p $(dir $@)
	@$(PY) tools/scripts/gen_linker_aliases.py $< $(SPLAT_CFG) $(ASM_DIR) > $@

# Extract PSX executable from ELF.
$(OUT_BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

check: build
	@echo "$(TARGET_SHA)  $(OUT_BIN)" | shasum -c -

ci:
	@$(PY) -m py_compile $$(find tools/scripts -name '*.py' -type f | sort)
	@$(PY) -c 'import pathlib, yaml; [yaml.safe_load(p.read_text(encoding="utf-8")) for p in sorted(pathlib.Path("configs").glob("**/*.yaml"))]'
	@test -z "$$(git ls-files assets disc extracted_assets asm build sdk '*.EXE' '*.IMG' '*.STR' '*.TIM' '*.XA' '*.BIN' '*.ISO')"
	@! git grep -n -E 'postpass|CC_POSTPASS|rewrite_|old-gcc|PE_STOCK|decompiled C translation|generated nonmatching' -- README.md configs include tools src

clean:
	rm -rf $(BUILD)

distclean: clean
	rm -rf $(ASM_DIR) $(LD_SCRIPT) linkers/$(VERSION)/undefined_*.txt

help:
	@echo "Targets:"
	@echo "  split       Run splat to (re)generate asm + linker script from $(SPLAT_CFG)"
	@echo "  build       Regenerate split asm, compile, and link -> $(OUT_BIN)"
	@echo "  check       Verify $(OUT_BIN) SHA-1 matches the target ($(TARGET_SHA))"
	@echo "  ci          Run source-only checks that do not need game assets"
	@echo "  clean       Remove build/"
	@echo "  distclean   Also remove generated asm and linker script"
