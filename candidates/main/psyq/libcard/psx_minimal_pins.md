# LIBCARD with minimal bindings on the existing PSX compilers

`exit_card_psx.c` and `patch_card2_psx.c` are new source-shape candidates,
not byte matches or production promotions. They require neither EABI nor
an additional compiler. Compile with the existing `tools/scripts/cc.sh`;
GCC272 is its default. GCC281 was tested separately.

Each function has two register bindings and one empty input barrier:

- global `returnAddress` in ra represents the SDK's global save/restore;
- the B0 service selector belongs in t1, consumed by the BIOS call;
- the barrier preserves that selector setup before the indirect call.

No local data pointer, copy word, loop end or BIOS jump-target register is
pinned. The structs express the observed table pointer at offset 0x18 or
0x16C; they are layout windows, not a claim to reconstruct the full BIOS.
The loops copy the actual fixed template lengths (three and five words).

| Candidate | Stock GCC272 | Stock GCC281 | Retail |
|---|---:|---:|---:|
| `_ExitCard` | 112 bytes, mismatch | 112 bytes, mismatch | 112 bytes |
| `_patch_card2` | 108 bytes, mismatch | 108 bytes, mismatch | 112 bytes |

Equal size does not mean byte equality. Both still reserve 16 bytes of
outgoing argument space and have different register allocation/scheduling.
These candidates supersede no existing matching reference: the earlier
EABI candidates remain available for byte-level comparison.

A fully unpinned/no-ASM experiment also exists in `/tmp/psyq-no-pins/`.
It uses `__builtin_return_address(0)` and six defined EABI arguments to put
the BIOS selector in t1. It is larger and does not match; it is not suitable
for the PSX ABI, which would put that sixth argument on the stack.

## Validation

128 randomized differential cases were run for each function/compiler pair,
plus both fully unpinned EABI variants: 768 cases, 1536 MIPS executions.
Checks cover BIOS selector, call order, exact copied words, global RA-slot
write, final SP/return address and callee-saved registers. Private stack
traffic and caller-saved register values may differ; this deliberately
checks the modeled external behavior, not instruction identity. Code was
linked at a scratch execution address so longer candidates could not
collide with the original instruction-template storage. BIOS/cache services
were modeled, not hardware-emulated.

Reproduction artifacts and test: `/tmp/psyq-no-pins/stock.py`, `search.py`,
`verify.py`, source/object/ELF variants and JSON size comparisons.
All compiler/MASPSX binaries remained unchanged. There is no new matching
function, progress/debt baseline update or change to the production manifest.

Link aliases for `_ExitCard`: savedReturnAddress=0x800A34F0,
templateStart=0x8007E584, entry=0x8007E514.
For `_patch_card2`: savedReturnAddress=0x800A34E0,
templateStart=0x8007E3C8, entry=0x8007E470.
Shared calls: EnterCriticalSection=0x80072714,
ExitCriticalSection=0x80072724, FlushCache=0x800726C4.
