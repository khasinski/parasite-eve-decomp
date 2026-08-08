# parasite-pc Port Plan: Adopting rage-pc's Clean-Tree Discipline

Synthesized from 11 parallel comparison findings. Every recommendation is de-duplicated across dimensions and cross-referenced to concrete `rage-pc` source. Paths are absolute where they matter for execution.

---

## 1. Executive Summary

- **The core rage-pc win is one invariant: no tracked file is ever a build output.** Every artifact is either hand-authored-and-committed OR machine-generated-and-gitignored, with a `rm -rf` + regenerate step that makes the generated set disposable. With that, `git status` clean is the steady state and dirtiness *only ever means authored edits*. parasite-pc violates this in at least four places (build auto-runs split, `xargs touch` on tracked src, dated progress files, denominator counted from generated asm).

- **Build must be decoupled from disassembly.** rage-pc's `make build`/`make check` never invoke splat; `split` is a separate, explicit, user-run target writing only into gitignored `asm/`, `linkers/`, `build/`. parasite-pc's `build: split-if-needed` makes `make check` regenerate asm as a side effect — the root non-idempotency.

- **Progress/badges must be a pure function of committed inputs.** rage-pc counts from the committed config subsegment list + committed `.c` (never a working-tree glob), writes only two ~107-byte date-free badge JSONs, and prints the dated table to stdout. parasite-pc `rglob`s the worktree, reads the gitignored generated asm tree for the denominator, and stamps `date.today()` into tracked `docs/PROGRESS.md` — so `make progress` dirties 3 tracked files on a clean tree with zero source change (empirically reproduced in the findings).

- **Twins/duplicates are a "one canonical + N derived" problem, not "N committed copies."** rage-pc is a single EXE so it never faced this, but its discipline is the model: one manifest → many generated. parasite-pc commits 8 full `+N` duplicate configs + per-twin `sym.txt` + 6853 thin TUs, and stamps shared room code in-place into 377 committed configs. This is the single largest source of config sprawl and denominator jitter.

- **rage-pc mechanized its matching methodology; parasite-pc only wrote it down.** rage-pc has `objverify.py` (per-object bit-identity verifier) with a whole family of verified-edit tools on top (`try_drop_pins/barriers/includes.py`, param-name adopters). parasite-pc has the stronger *prose* (GOAL.md, ASM_AND_GTE_POLICY.md) but almost none of the automation — despite a 447-pin / 402-alias / 213-barrier backlog that is tailor-made for it.

- **Centralize declarations behind a stable include tree, but only after header-dep tracking exists.** rage-pc has one `common.h` types hub + `sys/types.h` for SDK-spelled names + libc-as-headers, zero local typedefs, `-Iinclude`/`-Isrc` flat includes, and `cpp -MD` + `-include $(C_DEPS)`. parasite-pc has 683 files redefining `typedef unsigned char u8`, 4318 `../room_lib/` relative escapes, and **no** dependency tracking — which is the blocker that must be closed *before* any header centralization.

---

## 2. CRITICAL Fixes (parasite-pc is actively broken here)

These four eliminate the non-idempotent build, the dirty-after-`make progress` tree, and the twin sprawl. Do them roughly in this order; C-1 and C-2 are prerequisites for trusting everything downstream.

### C-1. Decouple `build`/`check` from `split` (kills the root non-idempotency)

**rage-pc:** `all: build check`; `build: $(OUT_BIN)`; `check: $(OUT_BIN)`. Neither invokes splat. `split` (Makefile:55-60) is standalone: `rm -rf $(ASM_DIR) $(LD_SCRIPT) $(UNDEFINED_*) ...` then `splat split` then the post-passes. README documents `make split` → `make build` → `make check`. `make split` also does **not** touch any source file.

**parasite-pc now:** `build: split-if-needed` (Makefile:85); `split-if-needed` (80-83) runs `make split` when `linkers/USA/main.ld` or `asm/USA/main/header.s` is missing; `check: build` (116). And `split` (75-78) ends with `grep -rl 'INCLUDE_ASM(' src/main --include='*.c' | xargs touch`, thrashing mtimes of committed `.c`.

**Steps:**
1. Change `/Users/hasik/Projects/parasite-pc/Makefile:85` from `build: split-if-needed` to `build: $(OUT_BIN)`; drop the `split-if-needed` target (80-83) or keep it only as a manual convenience.
2. Remove the `xargs touch` line from the `split` target (Makefile:78).
3. Document the fresh-clone order in `README.md`: `make split` → `make build` → `make check`. Update any CI script to call `make split` first.
4. **Risk:** low. Fresh clones/CI must now call `make split` explicitly (split is already idempotent into gitignored paths).

### C-2. Make progress deterministic and clean (kills the dirty-tree-after-`make progress`)

**rage-pc:** `tools/scripts/progress_report.py` counts from the committed config: for each `c` subsegment does `src = SRC_ROOT / f"{name}.c"`; if absent, counts it as one asm function. It never reads the generated asm tree. Writes only `docs/badges/functions.json` + `code.json` (date-free `{schemaVersion,label,message,color}`); prints the dated table to stdout. The narrative table is static in `README.md`.

**parasite-pc now (all three reproduced on a clean tree → 3 dirtied files):**
- `docs/PROGRESS.md` (progress_report.py ~L1386) and `docs/progress.html` header embed `date.today()` → guaranteed daily diff.
- `row_for()` (~L1317) does `src_dir.rglob("*.c")` and `count_asm_funcs()` reads whatever `.s` exist under `asm/USA/overlays` → denominator drifts (`total 10091→10090`).
- `docs/progress.html` is a tracked 268 KB / 2558-row per-TU listing → diff magnet.

**Steps:**
1. **De-date:** move the `Generated ... on <date>` line out of the text written to `docs/PROGRESS.md` (progress_report.py ~L1386, ~L1427) and out of the `progress.html` header (progress_site.py ~L60-75); print to stdout instead.
2. **Config-driven counting:** refactor `row_for()`/`count_asm_funcs()` to iterate the committed config subsegments (reuse `parse_segments`) and look up the exact named `src/.../{name}.c`, mirroring rage-pc's `SRC_ROOT / f"{name}.c"`. Stop reading `asm/USA/overlays` and `asm/USA/main` for counts. Re-baseline and commit badges once after the semantics shift.
3. **Stop committing heavy generated views:** `git rm --cached docs/progress.html docs/PROGRESS.md`; add both to `.gitignore`; keep only `docs/badges/*.json` tracked; move any human-facing table into `README.md`. Regenerate HTML in CI if it's served via Pages.
4. **Guard:** add a `make check-clean` target / CI step running `make progress` then `git diff --exit-code docs/badges` (and `docs/PROGRESS.md` if still tracked). Fails loudly if drift returns. Sequence this *last* (after 1-3 land).
5. **Files:** `tools/scripts/progress_report.py` (~L1275 `count_asm_funcs`, ~L1317 `row_for`, ~L1386 date, ~L1408-1420 room glob), `tools/scripts/progress_site.py` (badge block ~L54-58, header ~L60-75), `.gitignore`, `README.md`, `Makefile` (progress 144-146 + new check-clean).
6. **Risk:** medium — headline totals will move slightly once counting is config-driven and twins counted once; that is the correct, honest number. Coordinate with the GOAL.md baseline.

### C-3. Enforce committed-vs-generated discipline (fixes fresh-clone breakage + tree sprawl)

**rage-pc:** `.gitignore` blanket-ignores generated output with a `.gitkeep` allowlist, never per-path enumeration: `asm/**` + `!asm/.gitkeep`; `linkers/**/*.ld`, `linkers/**/undefined_*.txt`, `linkers/**/addr_halves.*.txt` with only `!linkers/**/undefined_syms_manual.txt` kept; `build/`, plus `*.d`, `-.*`, `*.lreg`, `*.greg`. `git ls-files asm/` returns just `.gitkeep`. The one literal-baking spot (PS-EXE header) is gitignored asm rewritten in place by `tools/scripts/symbolise_header.py` — never a committed `_header.c`.

**parasite-pc now:** enumerates generated paths per-version; has vestigial `!asm/USA/main/header.s` and `!asm/USA/main/main/068774_pre_InitGeom.s` exceptions (yet `git ls-files asm/` is empty — misleading); commits `objdiff.json` (753 KB / 1883 units, regenerated on split); commits overlay `_header.c` for only 2 of many overlays that declare a `c` header subsegment.

**Steps:**
1. Convert the generated section of `.gitignore` to rage-pc's blanket + `.gitkeep` style; add `.gitkeep` files so empty `asm/`/`linkers/` dirs survive clone. Replace the blunt global `*.s` with a scoped `asm/**` ignore (safer — won't catch any hand-authored `.s` under `src/`).
2. Drop the vestigial `!asm/USA/main/header.s` and `!...068774_pre_InitGeom.s` exceptions.
3. **objdiff.json:** either gitignore it and generate on demand (`make objdiff-config`), or adopt rage-pc's static `"units": []` form (465 bytes) and let objdiff-cli auto-discover. `git rm --cached objdiff.json`. Document `make objdiff-config` in README Quick Start.
4. **Overlay headers:** port `tools/scripts/symbolise_header.py` (new `tools/scripts/symbolise_overlay_header.py`), change overlay header subsegments from `c` (e.g. `[0x0, c, fx_field_header]`) to a `header`/`data` asm subseg rewritten in place at split, and gitignore the generated output. Removes the entire "config references an uncommitted-but-required `.c`" failure class. *Stopgap if the asm conversion is too large:* commit every referenced `_header.c` and add a fresh-clone guard.
5. **Fresh-clone guard:** add a split-time check that every `c`/`.rodata`/`.data` subsegment in `main.yaml` and overlay configs has a tracked source file, failing loudly. Wire into the `split` target (Makefile 75-83). Directly prevents recurrence.
6. **Files:** `.gitignore`, `objdiff.json`, `tools/scripts/objdiff_config.py`, `configs/USA/overlays/*.yaml` (header subsegments), new `tools/scripts/symbolise_overlay_header.py` + fresh-clone guard, `configs/USA/main.yaml`.
7. **Risk:** medium — must confirm every referenced header is reproducible from `make split` before ignoring; validate with `overlay-check-all` staying green.

### C-4. Data-drive twin/duplicate overlays (kills 8 dup configs + per-twin sym + 6853 thin TUs)

**rage-pc:** no twin mechanism (single EXE), but the model is "one manifest → many generated" (`symbolise_header.py`, `gen_undefined_addr_aliases.py`). Its transferable rule: model twins as ONE canonical committed source + N machine-derived gitignored outputs, never N committed physical copies.

**parasite-pc now:** `tools/scripts/catalog_pe_overlays.py` already computes `duplicate_group`/`duplicate_count`/`canonical_vram` (enrich_match_metadata ~L452-470) and advises "decompile one representative before touching siblings." But 8 `+N` twin configs (`room_m291+2.yaml`, `room_m326+1.yaml`, …), their per-twin `sym.*.txt`, and `room_lib_stamp.py`'s in-place edits of 377 committed configs make twins physical duplicates.

**Steps:**
1. Promote `catalog_pe_overlays.py`'s duplicate data into a **committed manifest** — `configs/USA/overlays/twins.yaml` (or `docs/assets/overlay-duplicates.json`) keyed by `duplicate_group`, mapping duplicate room ids → canonical, preserving the per-room vram/`room_delta` semantics the `+N` naming encodes.
2. Keep ONE canonical config + `sym.txt` per unique binary; gitignore (or delete) the 8 `+N` duplicate configs + their `sym.*.txt`, regenerating siblings from the manifest.
3. Reroute `room_lib_stamp.py` so it emits into a **gitignored generated dir** rather than editing committed `configs/USA/overlays/*.yaml` + `sym.*.txt` + `src/overlays/{room}/{name}.c` in place. Same for `init_overlay_from_trace.py` (currently `write_text` into `configs/USA/overlays`, ~L108-109).
4. Add `make overlays-regen` that `rm -rf`s the generated overlay configs/stamps then re-derives from the manifest (mirror rage-pc's `rm -rf ...; splat split`), so a rebuild is byte-identical and never half-updates the tree.
5. Change `overlay-check-all` (Makefile 253-257) to verify each twin's `.bin` SHA against the single built canonical output instead of carrying parallel config/src trees.
6. **Files:** `tools/scripts/catalog_pe_overlays.py` (L452-470, 76-77, 469-470), `tools/scripts/room_lib_stamp.py`, `tools/scripts/init_overlay_from_trace.py` (L108-109), `Makefile` (overlay-check-all + new overlays-regen), new `configs/USA/overlays/twins.yaml`, the 8 `room_m*+*.yaml`.
7. **Risk:** medium-high — must prove every twin config is byte-reproducible from the manifest before untracking (else fresh clones lose data). Stage behind `make room-split-audit` (`tools/scripts/audit_room_split.py`) passing.

---

## 3. Organizational Improvements

### O-1. Config/splat layout — shrink the committed config surface

**rage-pc:** 2 splat configs (`configs/USA/main.yaml`, `configs/PAL/main.yaml`) + centralized `sym.main.txt`/`rel.main.txt`, plus a separate repo-root `decomp.yaml` manifest (name/repo/platform/versions with sha1 + paths). Boilerplate lives in one yaml + one script and cannot drift.

**parasite-pc now:** 378 configs (main + 377 overlays), each repeating ~30 lines of identical `options:` boilerplate (162 room configs = 18,713 lines); 186 per-overlay `sym.room_*.txt`; a nonstandard `pad` in `section_order`; dot-vs-nodot `.rodata` linkage in `main.yaml`.

**Steps:**
1. Script a rewrite of `configs/USA/overlays/*.yaml` stripping keys that equal splat defaults (`compiler:GCC`, `glabel/jlabel/dlabel`, `generate_asm_macros_files:False`, full `section_order`, ASCII encodings, guesser level 2, `subalign:4`, universal `*_path` keys). Keep only per-overlay diffs (name, sha1, vram, subsegments, paths). Verify against a splat dry-run that omitted keys fall back identically. Cuts ~18k lines ~in half.
2. Make `catalog_pe_overlays.py --format splat-yaml` (`make overlay-init`, Makefile 200-212) the single authoritative writer, emitting the trimmed form; regenerated output must byte-match trimmed configs before switch-over.
3. Add a repo-root `decomp.yaml` manifest (model rage-pc's) decoupled from `mizuchi.yaml`, giving target SHA + per-version paths one home instead of hardcoded `Makefile:3-5`.
4. Remove the nonstandard `pad` from `main.yaml` `section_order` unless a subsegment needs it; align on rage-pc's standard 6-section order.

### O-2. src module organization — semantic naming + layout as a committed fact

**rage-pc:** `src/main/<VER>/lib/lib{api,c,cd,...}/` one-function-per-file named after the real symbol (zero `func_*.c`), `src/main/<VER>/main/<module>/` topically-named multi-function files. `C_SRCS := find src ...`; `build` never runs splat, so module identity is a static property of committed files.

**parasite-pc now:** coarse skeleton is sound (`src/main/psyq/<lib>/` SDK tier, ~30 game modules, `src/overlays/`). But interior is address-named (`func_8018F004.c`), ~thousands of stubs.

**Steps (opportunistic, gated by `make check`):**
1. (Covered by C-1 — decoupling build makes the committed tree the sole source of module identity.)
2. Rename `func_ADDR.c` → semantic names *as functions are identified*, prioritizing `src/main/psyq/*` to reach parity with rage-pc's `CdControl.c`-style SDK naming. **Atomically** update the matching splat config subsegment name in the same commit (else split re-emits the old stub). Do NOT mass-rename undecompiled stubs.
3. Later: carve monoliths into topical files as behavior boundaries become clear (rage-pc's `audio.c 2212 lines → 8 files` reorg is the model).

### O-3. include/ and libc centralization (blocked on tooling; see T-1/T-6)

**rage-pc:** one `include/common.h` types hub (s8/u8/…/f32 defined once, used by 299 files), separate `include/sys/types.h` for `u_char/u_short/u_int/u_long`, libc-as-headers (`string.h`, `stdio.h`, `ctype.h`) + one-function-per-file `.c`, per-module `include/game/*.h` prototype headers, zero local typedefs/forward-decls, `-Iinclude -Isrc/main` flat includes, `cpp -MD` + `-include $(C_DEPS)`.

**parasite-pc now:** no `common.h`, no `sys/types.h`; 683 files redefine `typedef unsigned char u8`; divergent aliases `InvU8`/`SaveU8`/`Pe1U8`/`FieldSfxU8`/`AkaoU8`; 156 files carry inline `extern ... __asm__("D_...")`; 4318 `../room_lib/room_lib.h` + 109 `../../../tools/m2c/m2c_macros.h` relative escapes; **no `-MD`**.

**Steps (strict order — dep tracking FIRST):**
1. **Prerequisite:** land header-dependency tracking (T-6 / `cpp -MD` + `-include $(C_DEPS)`). Without it, editing a newly-centralized `common.h` silently leaves stale objects.
2. Create `include/common.h` (s8/u8/…/f32 once, `#include include_asm.h`); mechanically strip the 683 local typedef blocks. Do it **incrementally, per subsystem/overlay**, `make check` gating each — old GCC 2.7.2 is sensitive to declaration order.
3. Create `include/sys/types.h` (`u_char/u_short/u_int/u_long`); replace redefinitions in `pe1/psyq_gpu.h:5`, `psyq_cd.h:4`, etc.
4. Remove divergent aliases → canonical `u8` (`sed` per subsystem + `make check`).
5. Add `-I$ROOT/src/overlays/room_lib -I$ROOT/tools/m2c` to `cc.sh`; rewrite relative escapes to flat names (`"room_lib.h"`, `"m2c_macros.h"`, `"pe1/gte.h"`). Preprocessor-only; codegen unaffected.
6. Hoist the 156 inline `__asm__("D_...")` externs into per-subsystem `include/pe1/*_globals.h` (reproduce exact symbol spelling — load-bearing for matching). Subsystem-by-subsystem.
7. Introduce libc-as-headers + one-function-per-file `.c` (consolidate `Square_Vsprintf.inc`, `src/main/memmove.c`).

---

## 4. Tooling Gaps to Close (adopt/adapt from rage-pc)

### T-1. Port `objverify.py` — the verified-edit core (highest leverage) ⭐

**rage-pc `tools/scripts/objverify.py`:** `snapshot(src)` builds one TU via `cc.sh`, **deletes the `.o` first** ("a stale `.o` has twice made a failed edit look like a pass"), copies to `.baseline`; `try_edit()` writes candidate, rebuilds, reverts unless identical. Compares **sections** (`.text/.data/.rodata/.sdata/.sbss/.bss` via `objcopy --only-section`) + `objdump -r` relocations, so symbol-table noise never false-reverts.

**parasite-pc now:** no equivalent. Closest is `merge_tus.py::text_bytes()` (`.text` only — misses data/rodata/reloc). `canon_protos.py` is its sole consumer; `apply_names.py` only prints "Run make check."

**Steps:** create `tools/scripts/objverify.py` reproducing parasite's exact per-object compile (cc.sh flags, overlay-vs-main). Retrofit `canon_protos.py` and route `apply_names.py` (`git mv` + substitution) through it, transactionally per proposal. This unblocks the entire naming/cleanup family below.

### T-2. Port the verified-edit naming family (on objverify)

`adopt_header_param_names.py`, `adopt_definition_param_names.py`, `rename_params.py`, `name_wide_views.py`, `name_scratchpad_slots.py`. Adapt PIN/ALIAS regexes to parasite's `asm("Name")` convention (ports cleanly) and path roots. High value — PE1 is ~85% done and full of generic `arg0`/`a0` names.

### T-3. Port the matching-crutch removers (on objverify) — advances GOAL.md directly

`try_drop_pins.py` (447-pin backlog), `try_drop_barriers.py` (213), `try_drop_includes.py`, plus a new `try_drop_aliases.py` for the 402 `__asm__("sym")` alias hacks. Each removes one crutch, rebuilds just that object, keeps only bit-identical removals. Mechanizes GOAL.md's manual "Attack order" steps 1-4. Wire a `make drop-pins FILE=...` target.

### T-4. Fix the permuter rig

**rage-pc `make_permuter_rig.py`:** writes `[weight_overrides]` zeroing all 14 fabrication weights + `perm_reorder_decls=0` (at 100, ~94% of permutations fail to compile — "busy while almost nothing is tried"). `check_base()` refuses bases containing `#include`, file-scope `.include`, `asm("symbol")` aliases, or register pins. `measure_candidate.py` reports **exact/aligned/equal** and warns "trust `exact`, not the aligned score."

**parasite-pc now:** `make_permuter_scratch.sh` writes a bare 2-line `settings.toml` (default weights — the exact wasteful config), no base gate, no exact/aligned/equal scorer.

**Steps:** rewrite `make_permuter_scratch.sh` / `make_overlay_permuter_scratch.py` to emit the tuned `[weight_overrides]` + `check_base()` gate; extend `func_cmp.py` (or add `measure_candidate.py`) to report exact/aligned/equal; document "trust exact" in GOAL.md and the `permute:` target output. Same GCC 2.7.2 family, so rage-pc's weights are a safe starting point.

### T-5. Symbol-naming / link idempotency

- **Stop linking splat's raw auto-undefineds.** parasite's `Makefile:107-110` feeds `undefined_syms_auto.main.txt` + `undefined_funcs_auto.main.txt` directly to `ld`, freezing every address. Port `gen_undefined_addr_aliases.py`: at link time run `nm` over the real objects and distill to only genuinely-unresolvable addresses; link that. Apply to overlays too (replace/augment `overlay_extra_undefineds.py`).
- **Port `symbolise_data_words.py`** (`.word 0x80xxxxxx` tables → `sym + N`) into the `split` target so jump tables relocate.
- **Converge on one name store.** Names live in `configs/USA/sym.main.txt` (splat symbol_addrs), C stays plain — not `asm("func_...")` labels. Add `tools/scripts/check_name_consistency.py` flagging any sym-config name that disagrees with the filename/identifier. Eliminate residual pins in `src/main/save/save4.c`, `Save_ProcessDataCallback.c`; update `docs/global_names.md`.
- Add a `nonmatching_labels.main.txt`-style committed table so nonmatching-asm regen stops grepping source labels (structural precondition for dropping pins safely).

### T-6. Header-dependency tracking (prerequisite for O-3)

Add `cpp -MD -MF "$OUT.d"` to `tools/scripts/cc.sh`; add `C_DEPS := $(C_OBJS:.o=.o.d)` + `-include $(C_DEPS)` to the Makefile; delete the `xargs touch` hack (already in C-1). Gitignore `*.d`, `-.*`, `*.lreg`, `*.greg` (cc1 reads stdin, so it names dumps after `-`). Small, low-risk, and unblocks safe header centralization.

### T-7. Tools tracking + git hygiene

- Replace the ~90-line hand-curated per-file `tools/scripts/*.py` allow/deny block in `.gitignore` with directory/pattern ignores (e.g. `tools/scripts/local/`, `tools/scripts/decomp_auto/`). grep for Makefile references before moving anything.
- Move harness/machine ignores out of the committed `.gitignore` into `.git/info/exclude` (rage-pc's trick): `**/.claude/worktrees/`, `**/.claude/checkpoints/`, `**/.claude/mailbox/`, `**/.claude/agent-registry.json`, `**/.claude/scheduled_tasks.{lock,json}`, `**/.claude/assistant-daemon-state.json`, plus `.idea/`/`.vscode/`. Keeps the published ignore file project-only. Document in README/setup since `.git/info/exclude` is per-clone.
- (Optional) Adopt rage-pc's branch-per-task worktree convention (`codex/func-<ADDR>`, sibling `parasite-pc-<tool>-<id>`) alongside the proposal farm, with a periodic `git worktree prune`.

---

## 5. Recommended Execution Order

Ordered by dependency and risk. **Safe/low-risk** items can proceed immediately; **needs-review** items require a `make check` / `overlay-check-all` gate or a re-baseline.

**Phase 0 — Prerequisites (safe, do first, no behavior change to output bytes)**
1. **T-6** header-dependency tracking (`cpp -MD` + `-include $(C_DEPS)`, gitignore `*.d`/`-.*`/`*.lreg`/`*.greg`). *Safe.* Unblocks O-3.
2. **T-1** port `objverify.py`; retrofit `canon_protos.py`. *Safe* (only gates edits). Unblocks T-2/T-3/T-5.

**Phase 1 — Critical clean-tree fixes (do before trusting badges or fresh clones)**
3. **C-1** decouple `build`/`check` from `split`; drop `xargs touch`. *Safe, small.* Foundational.
4. **C-2** deterministic + de-dated progress; stop committing `progress.html`/`PROGRESS.md`. *Needs-review* — re-baseline badges once; totals move slightly.
5. **C-3** committed-vs-generated `.gitignore` overhaul; gitignore/static `objdiff.json`; overlay-header post-pass + fresh-clone guard. *Needs-review* — verify `overlay-check-all` green; must confirm headers reproducible before untracking.
6. **T-7** move harness ignores to `.git/info/exclude`; pattern-based tools ignore. *Safe.*

**Phase 2 — Twin/duplicate consolidation (highest-value structural, most review)**
7. **C-4** data-drive twins via committed `twins.yaml`; reroute stampers to gitignored generated dir; `make overlays-regen`. *Needs-review, high* — stage behind `make room-split-audit`; prove byte-reproducibility before untracking the 8 dup configs.
8. **O-1** trim overlay-config boilerplate; make the generator authoritative; add `decomp.yaml`. *Needs-review* — regenerated configs must byte-match trimmed form.

**Phase 3 — Link idempotency + naming automation (incremental, gated)**
9. **T-4** permuter rig fix (weights + `check_base` + exact/aligned/equal). *Safe-ish* — improves reliability; re-tune weights if match rates drop.
10. **T-5** `gen_undefined_addr_aliases.py` + `symbolise_data_words.py` + name-consistency checker; drop residual save/ pins. *Needs-review* — do `main.exe` first, `make check`, then overlays; link-order sensitive.
11. **T-2** + **T-3** verified-edit naming + crutch-removal sweeps. *Safe per-object* (objverify auto-reverts), but gate a full `make check` after each subsystem sweep. Directly burns down the 447-pin / 402-alias / 213-barrier backlog.

**Phase 4 — Organizational polish (large, opportunistic, gated)**
12. **O-3** include/libc centralization (`common.h` → `sys/types.h` → aliases → flat includes → globals headers → libc). *Needs-review, incremental* — one subsystem at a time, `make check` each. Blocked on Phase 0 T-6.
13. **O-2** semantic `func_ADDR.c` renames (psyq/ first), atomic with config subsegment updates. *Needs-review* per rename.
14. **C-2 step 4 / determinism guard** `make check-clean` CI gate — land **last**, after all non-idempotencies above are fixed, so it isn't perpetually red.

**Dependency notes:** T-6 → O-3. T-1 → {T-2, T-3, T-5 apply-side}. C-1 → C-2 (config-driven counting assumes build no longer emits stubs). C-3/C-4 must precede the C-2 determinism guard (step 14). O-1 and C-4 touch the same overlay configs — do C-4 first, then O-1 trims the survivors.
