# parasite-pc — Parasite Eve (PS1, SLUS-006.62) matching decompilation.

VERSION    ?= USA
BASENAME   := main
export PE_VERSION := $(VERSION)
TARGET_SHA := 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
ifeq ($(VERSION),JP)
TARGET_SHA := 411bd943982140a9772ff23f90a2587e2724cac8
endif

ROOT       := $(abspath .)
PY         ?= $(if $(wildcard $(ROOT)/.venv/bin/python),$(ROOT)/.venv/bin/python,python3)
SPLAT_CFG  := configs/$(VERSION)/$(BASENAME).yaml

BUILD      := build/$(VERSION)
ASM_DIR    := asm/$(VERSION)/$(BASENAME)
SRC_DIR    := src/$(BASENAME)
ifeq ($(VERSION),JP)
SRC_DIR    := src-jp/$(BASENAME)
endif
LD_SCRIPT  := linkers/$(VERSION)/$(BASENAME).ld
TARGET_BIN := assets/$(VERSION)/$(BASENAME).exe
PE_IMG     := disc/extracted/PE.IMG
PE_IMG_SHA := 146c0ce7308bf9fdc2ba5a84230e198db0663f3b
PE_IMG_OUT := $(BUILD)/PE.IMG
ifeq ($(VERSION),JP)
PE_IMG     := disc/jp/extracted/PE.IMG
endif
OVERLAY    ?= fx_field
OVERLAY_CONFIG_DIR ?= configs/$(VERSION)/overlays
OVERLAY_CONFIGS = $(wildcard $(OVERLAY_CONFIG_DIR)/*.yaml)
OVERLAY_NAMES = $(basename $(notdir $(OVERLAY_CONFIGS)))
OVERLAY_CFG = $(OVERLAY_CONFIG_DIR)/$(OVERLAY).yaml
OVERLAY_ORIG_DIR := original/$(VERSION)/overlays
# The retail binary is named by the config's target_path, not by the overlay
# name: overlays may be renamed (room_mNNN_<place>) while the asset files and
# the CI asset checkout keep their original names.
OVERLAY_TARGET_PATH := $(shell sed -n 's/^  target_path: *//p' $(OVERLAY_CFG) 2>/dev/null)
OVERLAY_ORIG := $(or $(OVERLAY_TARGET_PATH),$(OVERLAY_ORIG_DIR)/$(OVERLAY).bin)
OVERLAY_ASM_DIR := asm/$(VERSION)/overlays/$(OVERLAY)
OVERLAY_BUILD := $(BUILD)/overlays/$(OVERLAY)
OVERLAY_LD := linkers/$(VERSION)/overlays/$(OVERLAY).ld
OVERLAY_UNDEFINED_FUNCS := linkers/$(VERSION)/overlays/undefined_funcs_auto.$(OVERLAY).txt
OVERLAY_UNDEFINED_SYMS := linkers/$(VERSION)/overlays/undefined_syms_auto.$(OVERLAY).txt
# The one symbol script an overlay links: main's symbols taken from the built
# main.elf, the loader ABI list, and the overlay's own window.
OVERLAY_LINK_SYMBOLS := linkers/$(VERSION)/overlays/link_symbols.$(OVERLAY).txt
OVERLAY_ABI := linkers/$(VERSION)/overlay_abi.txt
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
OVERLAY_VMA_OVERLAP := boot_display menu_memcard
NM         := mipsel-none-elf-nm
OBJCOPY    := mipsel-none-elf-objcopy

ASM_SRCS := $(shell find $(ASM_DIR) -name '*.s' -not -path '*/matchings/*' -not -path '*/nonmatchings/*' 2>/dev/null)
C_SRCS   := $(shell find $(SRC_DIR) -name '*.c' 2>/dev/null)

ASM_OBJS := $(ASM_SRCS:%.s=$(BUILD)/%.s.o)
C_OBJS   := $(C_SRCS:%=$(BUILD)/%.o)

OBJS := $(ASM_OBJS) $(C_OBJS)

# Header dependencies are emitted by the stock preprocessor through cc.sh.
# Without these, editing a shared ABI header can leave stale objects behind
# and make an incremental `make check` falsely pass.
C_DEPS := $(C_OBJS:.o=.o.d)
-include $(C_DEPS)

.PHONY: overlay-collision-check overlay-collision-baseline overlay-collisions expected objdiff-config progress-audit report report-audit all build check check-sources source-policy-check ci verify verify-clean clean diff distclean overlay-build overlay-build-all overlay-check overlay-check-all overlay-clean overlay-extract overlay-split pe-img-repack pe-img-check permute progress debt debt-check debt-baseline organization-check organization-baseline test drop-pins drop-barriers drop-aliases shift-audit shift-test shift-full-test shift-check shift-baseline split split-if-needed tools

all: verify

split:
	rm -rf $(ASM_DIR) $(LD_SCRIPT) $(UNDEFINED_SYMS) $(UNDEFINED_FUNCS) $(ADDR_ALIASES)
	@$(PY) -m splat split $(SPLAT_CFG)
	@find $(ASM_DIR) -name '*.data.s' -o -name '*.rodata.s' | xargs $(PY) tools/scripts/collapse_zero_data.py
	@$(PY) tools/scripts/ld_postpass.py $(LD_SCRIPT) $(ASM_DIR)/header.s
	@grep -rl 'INCLUDE_ASM(' $(SRC_DIR) --include='*.c' 2>/dev/null | xargs touch 2>/dev/null || true

split-if-needed:
	@if [ ! -f "$(LD_SCRIPT)" ] || [ ! -f "$(ASM_DIR)/header.s" ] || \
	    [ "$(SPLAT_CFG)" -nt "$(LD_SCRIPT)" ]; then \
	    $(MAKE) split; \
	fi

build: split-if-needed
	@$(MAKE) --no-print-directory $(OUT_BIN)

# Assemble split MIPS asm with maspsx-aware path (no preprocessing needed —
# splat output already uses macro.inc and is GNU-as compatible).
$(BUILD)/asm/%.s.o: asm/%.s
	@mkdir -p $(dir $@)
	$(AS) -EL -G0 -mips4 -32 -no-pad-sections -Iinclude -I$(ASM_DIR) -o $@ $<

# Compile C with PSYQ GCC 2.7.2 → maspsx → GNU as.
$(BUILD)/src/%.c.o: src/%.c | $(BUILD)
	@mkdir -p $(dir $@)
	$(CC_WRAPPER) $< $@

# The JP target keeps its own sources in src-jp/.
$(BUILD)/src-jp/%.c.o: src-jp/%.c | $(BUILD)
	@mkdir -p $(dir $@)
	$(CC_WRAPPER) $< $@

$(BUILD):
	@mkdir -p $@

# Link to ELF using splat-generated linker script.
UNDEFINED_SYMS := linkers/$(VERSION)/undefined_syms_auto.$(BASENAME).txt
UNDEFINED_FUNCS := linkers/$(VERSION)/undefined_funcs_auto.$(BASENAME).txt
UNDEFINED_MANUAL := linkers/$(VERSION)/undefined_syms_manual.txt
ADDR_ALIASES := linkers/$(VERSION)/undefined_addr_aliases.$(BASENAME).txt

# Link only the absolute addresses that no object actually defines. Feeding
# splat's complete auto files to ld would override real definitions forever.
$(ADDR_ALIASES): $(OBJS) $(UNDEFINED_MANUAL) $(UNDEFINED_SYMS) $(UNDEFINED_FUNCS) tools/scripts/gen_undefined_addr_aliases.py
	@$(PY) tools/scripts/gen_undefined_addr_aliases.py --nm $(NM) --output $@ \
	      --source $(UNDEFINED_SYMS) --source $(UNDEFINED_FUNCS) \
	      --manual $(UNDEFINED_MANUAL) $(OBJS)

$(ELF): $(OBJS) $(LD_SCRIPT) $(UNDEFINED_MANUAL) $(ADDR_ALIASES)
	$(LD) -EL -T $(LD_SCRIPT) -T $(UNDEFINED_MANUAL) -T $(ADDR_ALIASES) \
	      -Map $(BUILD)/$(BASENAME).map -o $@

# Extract PSX executable from ELF.
$(OUT_BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

check: build
	@echo "$(TARGET_SHA)  $(OUT_BIN)" | shasum -c -

# Fresh-clone guard: every committed `c` subsegment must have a tracked source.
check-sources:
	@$(PY) tools/scripts/check_c_subseg_sources.py

source-policy-check:
	@$(PY) tools/scripts/check_source_policy.py
	@$(PY) tools/scripts/check_flag_markers.py
	@$(PY) tools/scripts/check_includes.py

# Structural checks suitable for a fresh public clone without retail assets.
# Byte matching is verified by the full build workflow; the legacy unit-test
# suite remains available through `make test` but is not a CI gate because its
# source-path fixtures do not track translation-unit merges.
ci: check-sources source-policy-check debt-check organization-check

# Canonical acceptance target when the local retail image/toolchain is present.
# shift-check reads the build, so it runs after `check` has produced it.
verify: ci check
	@$(MAKE) --no-print-directory shift-check
	@$(MAKE) --no-print-directory overlay-collision-check

# Use before merging structural, toolchain, manifest, or shared-header changes.
verify-clean:
	@$(MAKE) --no-print-directory clean
	@$(MAKE) --no-print-directory verify

test:
	@$(PY) -m unittest discover -s tools/tests -p 'test_*.py'

diff:
	@./tools/asm-differ/diff.py --help >/dev/null 2>&1 || (echo "asm-differ deps missing; run: .venv/bin/pip install -r tools/asm-differ/requirements.txt" && exit 1)
	@./tools/asm-differ/diff.py -mwo $(FUNC)

permute:
	@$(PY) tools/decomp-permuter/permuter.py $(FUNC)

progress:
	@$(PY) tools/scripts/progress_report.py

# Crutch-debt tracker: count pins/barriers/aliases/gotos still to remove.
# PE1_NOP slots are not debt.
debt:
	@$(PY) tools/scripts/crutch_debt.py --write

debt-check:
	@$(PY) tools/scripts/crutch_debt.py --check

debt-baseline:
	@$(PY) tools/scripts/crutch_debt.py --update-baseline

organization-check:
	@$(PY) tools/scripts/organization_debt.py --check

organization-baseline:
	@$(PY) tools/scripts/organization_debt.py --update-baseline

# Verified crutch removal (byte-identical only; run per file/dir): make drop-pins FILE='src/main/draw/*.c'
drop-pins:
	@$(PY) tools/scripts/try_drop_pins.py $(FILE)
drop-barriers:
	@$(PY) tools/scripts/try_drop_barriers.py $(FILE)
drop-aliases:
	@$(PY) tools/scripts/try_drop_externs.py $(FILE)

# Shiftability (docs/CODE_ORGANIZATION.md): inventory of fixed addresses and
# the padded-relink comparison. Both read the existing build only.
shift-audit:
	@$(PY) tools/scripts/shift_audit.py
shift-test:
	@$(PY) tools/scripts/shift_test.py $(if $(PAD),--pad $(PAD),)
# Main and all overlays relinked with main shifted by PAD (default 0x10):
# every image difference must be an address field moved by PAD. Needs
# `make build overlay-build-all`; writes only to a temporary directory.
shift-full-test:
	@$(PY) tools/scripts/shift_full_test.py $(if $(PAD),--pad $(PAD),) --jobs $(OVERLAY_JOBS)

# Shiftability ratchet: fail when a fixed-address count grows past
# tools/shift_baseline.json. Needs `make build`; the overlay counts are only
# checked when `make overlay-build-all` has built every overlay (CI sets
# SHIFT_REQUIRE_OVERLAYS=1 so they are never skipped there).
shift-check:
	@$(PY) tools/scripts/shift_audit.py --check $(if $(SHIFT_REQUIRE_OVERLAYS),--require-overlays,)

shift-baseline:
	@$(PY) tools/scripts/shift_audit.py --update-baseline

# Overlay global names that more than one overlay defines
# (docs/OVERLAY_SYMBOL_COLLISIONS.md). The check fails when a new conflicting
# name or a name private to one overlay appears; it needs `make overlay-build-all`
# and is skipped with a message when the overlays are not built (CI sets
# SHIFT_REQUIRE_OVERLAYS=1 so it is never skipped there).
overlay-collision-check:
	@$(PY) tools/scripts/overlay_symbol_collisions.py --check $(if $(SHIFT_REQUIRE_OVERLAYS),--require-overlays,)

overlay-collision-baseline:
	@$(PY) tools/scripts/overlay_symbol_collisions.py --update-baseline --write

overlay-collisions:
	@$(PY) tools/scripts/overlay_symbol_collisions.py --write

# Disassemble the retail binaries into objdiff's target objects. This is a
# fresh second split against an empty source tree - never a copy of our own
# build, which would compare the build against itself and prove nothing.
# Needs `make build` for the main map and `make overlay-build-all` for the
# overlay maps; gen_expected says which modules are missing.
expected:
	@$(PY) tools/scripts/gen_expected.py

objdiff-config:
	@$(PY) tools/scripts/objdiff_config.py

progress-audit: expected objdiff-config
	@$(PY) tools/scripts/audit_progress.py

OBJDIFF ?= tools/objdiff/objdiff-cli

# The report decomp.dev ingests: every function in every shipped binary,
# each compared against its retail disassembly.
report: progress-audit
	@$(OBJDIFF) report generate -p . -o $(BUILD)/report.json
	@$(PY) tools/scripts/audit_report.py
	@echo "wrote $(BUILD)/report.json"

report-audit:
	@$(PY) tools/scripts/audit_report.py

# Build every configured overlay; OVERLAY_JOBS bounds the parallel makes.
# Each overlay logs to build/USA/overlay-build.<name>.log and a failure
# replays its tail - a parallel build that hides why it failed costs a CI
# round trip per bug.
# Overlays link against the built main executable's symbols, so main is
# built first.
OVERLAY_JOBS ?= 4
overlay-build-all: build | $(BUILD)
	@test -n "$(OVERLAY_NAMES)" || { echo "no configured overlays in configs/$(VERSION)/overlays"; exit 1; }
	@$(PY) tools/scripts/parallel_overlay_make.py --jobs $(OVERLAY_JOBS) \
	    --target overlay-build $(OVERLAY_NAMES)

overlay-extract:
	@if [ -f "$(OVERLAY_ORIG)" ]; then echo "have $(OVERLAY_ORIG)"; exit 0; fi; \
	test -f $(OVERLAY_CFG) || { echo "missing overlay config: $(OVERLAY_CFG)"; exit 1; }; \
	$(PY) tools/scripts/extract_overlay_config_target.py "$(OVERLAY_CFG)" --pe-img "$(PE_IMG)"

# Splat resolves a shared unit (`../room_lib/<unit>`) through the overlay's
# own source directory, which Git drops when the overlay has no other C: a
# missing directory makes the shared source look absent, and splat would then
# overwrite it with an INCLUDE_ASM stub.
OVERLAY_SRC_PATH = $(shell sed -n 's/^  src_path: *//p' $(OVERLAY_CFG) 2>/dev/null)
overlay-split: overlay-extract
	@test -f $(OVERLAY_CFG) || { echo "missing overlay config: $(OVERLAY_CFG)"; exit 1; }
	@mkdir -p $(OVERLAY_SRC_PATH) $(OVERLAY_BUILD)
	@# Main's symbol file names the main addresses the overlay uses; splat lists
	@# every one of them as outside the overlay's segments. Keep that in a log.
	@$(PY) -m splat split $(OVERLAY_CFG) > $(OVERLAY_BUILD)/splat.log 2>&1 || \
	    { cat $(OVERLAY_BUILD)/splat.log; exit 1; }
	@# The overlay links at offsets from the floating window block, whose
	@# window symbols come from the built main (linkers/USA/overlay_windows.txt).
	@$(PY) tools/scripts/overlay_windows.py rebase-ld $(OVERLAY_LD)
	@find $(OVERLAY_ASM_DIR) \( -name '*.data.s' -o -name '*.rodata.s' -o -path '*/data/*.s' \) \
	    | xargs $(PY) tools/scripts/collapse_zero_data.py

# Assemble and compile exactly the units the generated linker script links.
# A shared library source (src/overlays/room_lib/) is compiled into every
# overlay that lists it; splat's per-function disassembly of it lands beside
# the overlay's asm and is not linked.
# PE_OVERLAY gives the room interface names their per-overlay spelling
# (include/pe1/room_interface.h).
overlay-build: overlay-split
	@grep -o '$(OVERLAY_BUILD)/asm/[^ ()]*\.s\.o' $(OVERLAY_LD) | sort -u | while read obj; do \
	    asm="$${obj#$(OVERLAY_BUILD)/}"; asm="$${asm%.o}"; \
	    mkdir -p "$$(dirname "$$obj")"; \
	    $(AS) -EL -G0 -mips4 -32 -no-pad-sections -Iinclude -I$(OVERLAY_ASM_DIR) \
	        -o "$$obj" "$$asm" || exit $$?; \
	done
	@grep -o '$(OVERLAY_BUILD)/src[-a-z]*/[^ ()]*\.c\.o' $(OVERLAY_LD) | sort -u | while read obj; do \
	    src="$${obj#$(OVERLAY_BUILD)/}"; src="$${src%.o}"; \
	    mkdir -p "$$(dirname "$$obj")"; \
	    $(CC_WRAPPER) "$$src" "$$obj" -DPE_OVERLAY=$(OVERLAY) || exit $$?; \
	done
	@$(PY) tools/scripts/overlay_link_symbols.py $(OVERLAY_BUILD) \
	    --ld $(OVERLAY_LD) $(if $(filter $(OVERLAY),$(OVERLAY_VMA_OVERLAP)),--pin-defined) \
	    --out $(OVERLAY_LINK_SYMBOLS) \
	    --config $(OVERLAY_CFG) --configs $(OVERLAY_CONFIG_DIR) \
	    --main-elf $(ELF) --abi $(OVERLAY_ABI) \
	    --catalogue $(OVERLAY_UNDEFINED_FUNCS) \
	    --catalogue $(OVERLAY_UNDEFINED_SYMS) \
	    --catalogue $(OVERLAY_CONFIG_DIR)/sym.$(OVERLAY).txt
	# Copied functions retain their original link VMAs, which can overlap other sections.
	$(LD) $(if $(filter $(OVERLAY),$(OVERLAY_VMA_OVERLAP)),--no-check-sections) -EL -T $(OVERLAY_LD) -T $(OVERLAY_LINK_SYMBOLS) \
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
	@$(PY) tools/scripts/check_overlay_hashes.py

# PE.IMG with every overlay slice replaced in place by its built binary
# (after `make overlay-build-all`). Fails when an overlay outgrows its slice
# and checks main's PE.IMG sector tables; the retail build must give back the
# original image (pe-img-check). The image itself is never committed.
pe-img-repack:
	@$(PY) tools/scripts/repack_pe_img.py --pe-img $(PE_IMG) --input-sha1 $(PE_IMG_SHA) \
	    --configs $(OVERLAY_CONFIG_DIR) --main-elf $(ELF) --out $(PE_IMG_OUT)

pe-img-check: pe-img-repack
	@echo "$(PE_IMG_SHA)  $(PE_IMG_OUT)" | shasum -c -

overlay-clean:
	rm -rf $(OVERLAY_ORIG_DIR) asm/$(VERSION)/overlays linkers/$(VERSION)/overlays $(BUILD)/overlays

clean:
	rm -rf $(BUILD)

distclean: clean
	rm -rf $(ASM_DIR) $(LD_SCRIPT) linkers/$(VERSION)/undefined_*.txt

-include local.mk
