# SPU register address rounding

Production matches all **164/164 linked bytes** of _spu_FsetRXXa at
`0x8007DB24`, and all **552/552 bytes** of its complete TU. Native GCC 2.7.2
and unmodified GNU as 2.8.1 compile the remainder operation as ordinary C.
There are no instruction-asm blocks; existing pins and the empty constraint
on the shifted result remain accounted for in debt.

When SPU memory mode requires it, a nonaligned address is rounded upward to
a memory unit and shifted by _spu_mem_mode_plus. Offset -1 returns the shifted
low halfword, -2 returns the adjusted byte address, and other offsets write
the shifted halfword to the indexed SPU register, returning the byte address.

GNU as 2.8.1 expands unsigned remainder to the retail division, zero check,
break 7 and mfhi sequence. It also avoids the redundant nops which GNU as 2.7
places at reorder boundaries in two neighboring functions. No assembler or
compiler source is patched, and no binary rewriting is used. The original TU
remains intact. `setup_gas281.sh` pins the release tarball's SHA-256 and CI
caches the native assembler. The assembler itself is built with host `-O0`:
modern Linux GCC at `-O2` produces a broken build that duplicates the `lw`
opcode over the preceding `lui` in symbol-load macros. This host build setting
does not change the PSX compiler's `-O2`. The linked bytes have been checked
with native assembler builds on macOS and Linux.

`tools/tests/test_counter_spu_address.py` verifies the entire linked TU against
a retail SHA-256, including the neighboring functions and all relocations.
