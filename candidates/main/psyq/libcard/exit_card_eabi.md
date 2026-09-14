# `_ExitCard`: reduced EABI comparison reference

This is not the requested readable, unpinned PSX implementation and is not
in the production manifest. The pure-C semantic baseline remains
`readable/exit_card.c`. This reference preserves the earlier full byte match
while reducing its constraints, without modifying GCC or MASPSX.

The retained source has six explicit register bindings (including the
global return address) and two empty ASM barriers for the BIOS selector and
call target. It contains no instruction ASM. Compared with the earlier
seven-binding/four-barrier reference, the handler pointer is unpinned and
the handler/source barriers are removed. Both scheduling passes are disabled.
The post-tested `do/while` is expressed as `for (;;)` with an explicit exit;
this does not make it an ordinary pre-tested loop.

## Exact validation

Native, unmodified GNU GCC 2.95.2 configured for `mipstx39el-unknown-elf`,
using `-O2 -G0 -mips1 -mcpu=3000 -msoft-float -mno-abicalls -mabi=eabi
-fno-schedule-insns -fno-schedule-insns2`, followed by unchanged MASPSX 2.56
and GNU MIPS assembler/linker, produces **112/112 identical bytes**.

The linked `.text` is compared directly with `assets/USA/main.exe` at
file offset `0x6ed14`, length 112. SHA-256:
`1a015396c49b62fc04510e787020cdeabc33cdd74e0d262d2345c0cbc6aa0aa6`.
No register normalization, relocation masking or instruction rewriting is used.

Link address: `0x8007e514`; symbols:

| Symbol | Address |
| --- | --- |
| savedReturnAddress | 0x800a34f0 |
| EnterCriticalSection | 0x80072714 |
| ExitCriticalSection | 0x80072724 |
| FlushCache | 0x800726c4 |
| templateStart | 0x8007e584 |
| templateEnd | 0x8007e590 |

Compiler on darwine:
`/home/hasik/psyq-exitcard-abi-20260914/source/gcc-2.95.2/gcc/cc1`.
Compiler SHA-256:
`b5760a365853baeaab49b808de695a2e822936b8f12f7a8afef88d7ceb64e013`.

## Bounded experiments

384 variants were compiled, assembled, linked and compared: 32 subsets of
the five non-ABI bindings; 256 combinations of those subsets and removal
of three barriers with the explicit-exit loop; 96 combinations of binding
subsets and scheduling flags with two barriers. The RA and selector bindings
were retained throughout. The smallest matching binding count in this
experiment is six, not a proven lower bound for other source shapes.
Removing either the handler binding or the copied-word binding matches;
removing both does not in these variants.

Experiments and all intermediate sources/binaries are in
`/tmp/psyq-exit-minimize/`: `search.py`, `barriers.py`, `scheduling.py` and
their JSON results. These scripts use `/tmp/psyq-no-pins/check.py` as the
compile/link harness. The selected probe is `s-2-02`; `retained` verifies
the checked-in source including its explanatory header.

This narrows the allocator/scheduler question but does not remove the PSX
ABI issue. In GCC 2.8.1, `config/mips/mips.h` defines `RETURN_ADDR_RTX(0, ...)`
as memory via `RETURN_ADDRESS_POINTER_REGNUM`; `expr.c`'s
`expand_builtin_return_addr` uses it directly. Thus the builtin is not a
direct replacement for the original global RA save. The same target header
reserves four argument words via `REG_PARM_STACK_SPACE` and
`OUTGOING_REG_PARM_STACK_SPACE` for ordinary calls. No new production
function or progress percentage is claimed.
