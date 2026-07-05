# parasite-pc — Parasite Eve (PS1, SLUS-006.62) matching decompilation.

VERSION    := USA
BASENAME   := main
TARGET_SHA := 452fb033f2eaa4b18aa20a5bca60b8125af3a37b

ROOT       := $(abspath .)
PY         := $(ROOT)/.venv/bin/python
SPLAT_CFG  := configs/$(VERSION)/$(BASENAME).yaml

BUILD      := build/$(VERSION)
ASM_DIR    := asm/$(VERSION)/$(BASENAME)
SRC_DIR    := src/$(BASENAME)
LD_SCRIPT  := linkers/$(VERSION)/$(BASENAME).ld
TARGET_BIN := assets/$(VERSION)/$(BASENAME).exe
PE_IMG     := disc/extracted/PE.IMG
OVERLAY    ?= ovl_009
OVERLAY_SLICE ?= scanner
OVERLAY_EXE ?= assets/$(VERSION)/$(BASENAME).exe
OVERLAY_SYM := configs/$(VERSION)/sym.$(BASENAME).txt
OVERLAY_MAIN_CFG := configs/$(VERSION)/$(BASENAME).yaml
OVERLAY_CONFIG_DIR ?= configs/$(VERSION)/overlays
OVERLAY_CONFIGS = $(wildcard $(OVERLAY_CONFIG_DIR)/*.yaml)
OVERLAY_NAMES = $(basename $(notdir $(OVERLAY_CONFIGS)))
OVERLAY_TRACE ?=
OVERLAY_TRACE_SHEET_OUT ?= build/$(VERSION)/overlay-trace-sheet.updated.csv
OVERLAY_TRACE_COMPLETE ?= build/$(VERSION)/overlay-trace-complete.csv
OVERLAY_TRACE_CONFIG_DIR ?= $(BUILD)/overlay-trace-configs
OVERLAY_TRACE_BATCH ?=
OVERLAY_TRACE_DOMAIN ?=
OVERLAY_TRACE_ANY_DOMAIN ?=
OVERLAY_TRACE_FORMAT ?= md
OVERLAY_TRACE_STRICT ?=
OVERLAY_TRACE_LIMIT ?=
OVERLAY_TRACE_ACTION ?=
OVERLAY_CFG = $(OVERLAY_CONFIG_DIR)/$(OVERLAY).yaml
OVERLAY_ORIG_DIR := original/$(VERSION)/overlays
OVERLAY_ORIG := $(OVERLAY_ORIG_DIR)/$(OVERLAY).bin
OVERLAY_ASM_DIR := asm/$(VERSION)/overlays/$(OVERLAY)
OVERLAY_BUILD := $(BUILD)/overlays/$(OVERLAY)
OVERLAY_LD := linkers/$(VERSION)/overlays/$(OVERLAY).ld
OVERLAY_UNDEFINED_FUNCS := linkers/$(VERSION)/overlays/undefined_funcs_auto.$(OVERLAY).txt
OVERLAY_UNDEFINED_SYMS := linkers/$(VERSION)/overlays/undefined_syms_auto.$(OVERLAY).txt
OVERLAY_EXTRA_UNDEFINEDS := linkers/$(VERSION)/overlays/undefined_extra.$(OVERLAY).txt
OVERLAY_OUT := $(OVERLAY_BUILD)/$(OVERLAY).bin

ELF        := $(BUILD)/$(BASENAME).elf
OUT_BIN    := $(BUILD)/$(BASENAME).exe

# CD image build (Disc 1): repack the original disc contents with our exe.
MKPSXISO   := tools/mkpsxiso-bin/mkpsxiso
DISC_XML   := disc/disc1.xml
DISC_DATA  := disc/extracted
ISO_STAGE  := $(BUILD)/iso
ISO_BIN    := $(BUILD)/parasite-eve-$(VERSION)-disc1.bin
ISO_CUE    := $(BUILD)/parasite-eve-$(VERSION)-disc1.cue

CC_WRAPPER := tools/scripts/cc.sh
AS         := mipsel-none-elf-as
LD         := mipsel-none-elf-ld
OBJCOPY    := mipsel-none-elf-objcopy

ASM_SRCS := $(shell find $(ASM_DIR) -name '*.s' -not -path '*/matchings/*' -not -path '*/nonmatchings/*' 2>/dev/null)
C_SRCS   := $(shell find $(SRC_DIR) -name '*.c' 2>/dev/null)

ASM_OBJS := $(ASM_SRCS:%.s=$(BUILD)/%.s.o)
C_OBJS   := $(C_SRCS:%=$(BUILD)/%.o)

OBJS := $(ASM_OBJS) $(C_OBJS)

.PHONY: expected objdiff-config report all build check clean diff distclean func-build func-diff func-target overlay-build overlay-check overlay-check-all overlay-clean overlay-extract overlay-func-diff overlay-init overlay-permuter-scratch overlay-split overlay-yaml permute progress proposal-smallest proposal-status split tools

all: build check

split:
	rm -rf $(ASM_DIR) $(LD_SCRIPT) $(UNDEFINED_SYMS) $(UNDEFINED_FUNCS)
	@$(PY) -m splat split $(SPLAT_CFG)
	@grep -rl 'INCLUDE_ASM(' src/main --include='*.c' 2>/dev/null | xargs touch 2>/dev/null || true

build: $(OUT_BIN)

# Assemble split MIPS asm with maspsx-aware path (no preprocessing needed —
# splat output already uses macro.inc and is GNU-as compatible).
$(BUILD)/asm/%.s.o: asm/%.s
	@mkdir -p $(dir $@)
	$(AS) -EL -G0 -mips4 -32 -no-pad-sections -Iinclude -I$(ASM_DIR) -o $@ $<

# Compile C with PSYQ GCC 2.7.2 → maspsx → GNU as.
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

# Extract PSX executable from ELF.
$(OUT_BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

check: $(OUT_BIN)
	@echo "$(TARGET_SHA)  $(OUT_BIN)" | shasum -c -

diff:
	@./tools/asm-differ/diff.py --help >/dev/null 2>&1 || (echo "asm-differ deps missing; run: .venv/bin/pip install -r tools/asm-differ/requirements.txt" && exit 1)
	@./tools/asm-differ/diff.py -mwo $(FUNC)

permute:
	@$(PY) tools/decomp-permuter/permuter.py $(FUNC)

func-target:
	@test -n "$(FUNC)" || (echo "usage: make func-target FUNC=func_800XXXXX" && exit 2)
	@$(PY) tools/scripts/decomp_pipeline.py target $(FUNC)

func-build:
	@test -n "$(FUNC)" || (echo "usage: make func-build FUNC=func_800XXXXX [SRC=path.c]" && exit 2)
	@$(PY) tools/scripts/decomp_pipeline.py build $(FUNC) $(if $(SRC),--source $(SRC),)

func-diff:
	@test -n "$(FUNC)" || (echo "usage: make func-diff FUNC=func_800XXXXX [SRC=path.c]" && exit 2)
	@$(PY) tools/scripts/decomp_pipeline.py diff $(FUNC) $(if $(SRC),--source $(SRC),)

proposal-status:
	@$(PY) tools/scripts/decomp_pipeline.py status

proposal-smallest:
	@$(PY) tools/scripts/decomp_pipeline.py list-smallest $(if $(N),--limit $(N),) $(if $(MIN_SIZE),--min-size $(MIN_SIZE),) $(if $(RETRY),--retry $(RETRY),)

progress:
	@$(PY) tools/scripts/progress_report.py
	@$(PY) tools/scripts/progress_site.py

# Snapshot the current byte-verified build as objdiff's target objects.
expected: check
	@rsync -a --delete build/USA/ expected/build/USA/
	@echo "expected/build/USA refreshed"

objdiff-config:
	@$(PY) tools/scripts/objdiff_config.py

report: objdiff-config
	@tools/objdiff/objdiff-cli report generate -p . -o build/USA/report.json
	@echo "wrote build/USA/report.json"

overlay-permuter-scratch:
	@$(PY) tools/scripts/make_overlay_permuter_scratch.py \
	    $(if $(FUNC),--func $(FUNC),) \
	    $(if $(RANK),--rank $(RANK),) \
	    $(if $(C_RANK),--c-rank $(C_RANK),)

overlay-func-diff:
	@$(PY) tools/scripts/overlay_func_diff.py \
	    $(if $(FUNC),--func $(FUNC),) \
	    $(if $(RANK),--rank $(RANK),) \
	    $(if $(C_RANK),--c-rank $(C_RANK),) \
	    $(if $(SRC),--src $(SRC),) \
	    $(if $(EXPECT_MISMATCH),--expect-mismatch,) \
	    $(if $(QUIET),--quiet,)

overlay-extract:
	@if [ -f "$(OVERLAY_ORIG)" ]; then echo "have $(OVERLAY_ORIG)"; exit 0; fi; \
	slice="$(OVERLAY_SLICE)"; \
	if [ -f "$(OVERLAY_CFG)" ]; then \
	    slice="$$( \
	        $(PY) tools/scripts/overlay_config_slice.py "$(OVERLAY_CFG)" \
	            --pe-img $(PE_IMG) --main-exe $(OVERLAY_EXE) \
	            --sym-file $(OVERLAY_SYM) --config-yaml $(OVERLAY_MAIN_CFG) \
	            --fallback "$(OVERLAY_SLICE)" \
	    )"; \
	fi; \
	$(PY) tools/scripts/catalog_pe_overlays.py $(PE_IMG) $(OVERLAY_EXE) \
	    --sym-file $(OVERLAY_SYM) --config-yaml $(OVERLAY_MAIN_CFG) \
	    --format extract --overlay $(OVERLAY) --target-slice $$slice \
	    --out-dir $(OVERLAY_ORIG_DIR)

overlay-yaml:
	@$(PY) tools/scripts/catalog_pe_overlays.py $(PE_IMG) $(OVERLAY_EXE) \
	    --sym-file $(OVERLAY_SYM) --config-yaml $(OVERLAY_MAIN_CFG) \
	    --format splat-yaml --overlay $(OVERLAY) --target-slice $(OVERLAY_SLICE) \
	    $(if $(VRAM),--vram $(VRAM),)

overlay-init:
	@mkdir -p $(dir $(OVERLAY_CFG))
	@$(PY) tools/scripts/catalog_pe_overlays.py $(PE_IMG) $(OVERLAY_EXE) \
	    --sym-file $(OVERLAY_SYM) --config-yaml $(OVERLAY_MAIN_CFG) \
	    --format splat-yaml --overlay $(OVERLAY) --target-slice $(OVERLAY_SLICE) \
	    $(if $(VRAM),--vram $(VRAM),) > $(OVERLAY_CFG)
	@echo "wrote $(OVERLAY_CFG)"

overlay-split: overlay-extract
	@test -f $(OVERLAY_CFG) || { echo "missing overlay config: $(OVERLAY_CFG)"; exit 1; }
	@$(PY) -m splat split $(OVERLAY_CFG)

overlay-build: overlay-split
	@find $(OVERLAY_ASM_DIR) -path '*/matchings/*' -prune -o -name '*.s' -print | while read asm; do \
	    obj="$(OVERLAY_BUILD)/$$asm.o"; \
	    mkdir -p "$$(dirname "$$obj")"; \
	    $(AS) -EL -G0 -mips4 -32 -no-pad-sections -Iinclude -I$(OVERLAY_ASM_DIR) \
	        -o "$$obj" "$$asm" || exit $$?; \
	done
	@if [ -d src/overlays/$(OVERLAY) ]; then \
	    find src/overlays/$(OVERLAY) -name '*.c' -print | while read src; do \
	        obj="$(OVERLAY_BUILD)/$$src.o"; \
	        mkdir -p "$$(dirname "$$obj")"; \
	        $(CC_WRAPPER) "$$src" "$$obj" || exit $$?; \
	    done; \
	fi
	@$(PY) tools/scripts/overlay_extra_undefineds.py $(OVERLAY_BUILD) \
	    --out $(OVERLAY_EXTRA_UNDEFINEDS) \
	    --existing $(OVERLAY_UNDEFINED_FUNCS) \
	    --existing $(OVERLAY_UNDEFINED_SYMS) \
	    --symbols $(OVERLAY_CONFIG_DIR)/sym.$(OVERLAY).txt
	$(LD) -EL -T $(OVERLAY_LD) -T $(OVERLAY_UNDEFINED_FUNCS) -T $(OVERLAY_UNDEFINED_SYMS) -T $(OVERLAY_EXTRA_UNDEFINEDS) \
	    -Map $(OVERLAY_BUILD)/$(OVERLAY).map -o $(OVERLAY_BUILD)/$(OVERLAY).elf
	$(OBJCOPY) -O binary $(OVERLAY_BUILD)/$(OVERLAY).elf $(OVERLAY_OUT)

overlay-check: overlay-build
	@EXPECTED=$$(shasum $(OVERLAY_ORIG) | cut -d' ' -f1); \
	ACTUAL=$$(shasum $(OVERLAY_OUT) | cut -d' ' -f1); \
	if [ "$$EXPECTED" = "$$ACTUAL" ]; then \
	    echo "$(OVERLAY): OK ($$ACTUAL)"; \
	else \
	    echo "$(OVERLAY): SHA mismatch"; \
	    echo "expected $$EXPECTED  $(OVERLAY_ORIG)"; \
	    echo "actual   $$ACTUAL  $(OVERLAY_OUT)"; \
	    exit 1; \
	fi

overlay-check-all:
	@test -n "$(OVERLAY_NAMES)" || { echo "no configured overlays in configs/$(VERSION)/overlays"; exit 1; }
	@for overlay in $(OVERLAY_NAMES); do \
	    $(MAKE) overlay-check OVERLAY=$$overlay || exit $$?; \
	done

overlay-clean:
	rm -rf $(OVERLAY_ORIG_DIR) asm/$(VERSION)/overlays linkers/$(VERSION)/overlays $(BUILD)/overlays

clean:
	rm -rf $(BUILD)

distclean: clean
	rm -rf $(ASM_DIR) $(LD_SCRIPT) linkers/$(VERSION)/undefined_*.txt

-include local.mk
