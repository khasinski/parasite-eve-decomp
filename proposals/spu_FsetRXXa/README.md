# SPU register address rounding

`_spu_FsetRXXa` rounds a nonaligned address upward when the SPU memory mode
requires it, then shifts the address by `_spu_mem_mode_plus`. Offset -1 returns
the shifted low sixteen bits, -2 returns the adjusted byte address, and other
offsets write the shifted halfword into the indexed SPU register before
returning the adjusted byte address.

Production now expresses the shift in C. Its v0 shift-count pin and the
read/write barrier on the a3 result retain the target allocation. The previous
inline nop and srlv instruction block is removed. The older division block
still remains; production is not yet plain C.

`candidate.c` also replaces that block with unsigned `%`, using only stock
MASPSX --expand-div to reproduce the divide-by-zero branch and break 7.
It is not an exact match: MASPSX adds a nop after mfhi before the dependent
branch. In the full source-unit trial it scores 97.560974% in upstream objdiff;
all remaining instruction differences are consequences of that extra nop.
Source-input/output barriers on the remainder did not remove it. ASPSX 2.21
and --dont-expand-li do not change this result; the default 2.56 has the same
extra instruction. The stock processor's remu handler calls
_handle_nop_before_next_instruction after expansion when no HI/LO spacing
nops are returned. No tool patch or postpass was made.

The candidate is kept separate from the build until its generated code is
exact. A switch to GCC 2.8.1 additionally changes allocation and scheduling;
the tested split-address and scheduler settings did not match.
