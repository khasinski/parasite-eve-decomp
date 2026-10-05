# Field point tiles: CPU work in C

`func_800D1DEC` (792 bytes) and `func_800D2104` (620 bytes) now perform the
SZ3 depth shift and stack store in C. Their previous `gte_stszotz` calls hid
CPU `sra` and `sw` instructions inside the GTE macro. Vector loads, RTPS,
depth read, projected-coordinate store and hazard nops now each use an
individual instruction macro.

Native stock GCC 2.7.2 (`-O2 -G0 -funsigned-char -mips1 -mcpu=3000`) and stock
MASPSX in 2.56 mode produce linked score **0**, with all 1412 bytes identical
to the retail range `main.exe[0xC25EC:0xC2B70]`. Full `make -j32 verify` on
darwine also passed, including the complete `main.exe` byte comparison.

Debt: one `$12` SZ3 pin, one empty tied depth-pointer constraint and three
explicit hazard nops per function. All 16 subsets of the four initial
barriers were checked; both post-store barriers were removable. Removing
either remaining pointer barrier changes an instruction; removing either
pin changes four instructions. The retained debt is in the ratchet baseline.

Research: `/home/hasik/fx-search-archives/point_tile` on darwine. `trim10`
is the retained candidate; `main_verify.log` records the whole-main check.
This closes hidden CPU assembly in two existing C functions; it does not
represent two newly discovered functions or a match of the scene_e20 flare.
