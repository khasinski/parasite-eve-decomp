# Four Psy-Q math, display and streaming functions

| SDK symbol | SDK object | Retail address | Text bytes |
| --- | --- | --- | ---: |
| `__adddf3` | LIBMATH/ADDDF3.OBJ | 0x80072A74 | 884 |
| `__divdf3` | LIBMATH/DIVDF3.OBJ | 0x80072F64 | 736 |
| `PutDispEnv` | LIBGPU/SYS.OBJ | 0x800755F0 | 1272 |
| `StCdInterrupt` | LIBCD/C_011.OBJ | 0x8007C564 | 2332 |

All 5,224 linked text bytes match retail, including relocated instructions.
The regression test compiles and links each entire function at its retail
address and checks its size and SHA-256. `provenance.json` independently records
Psy-Q 4.3 signature matches, SDK label offsets, known-byte and relocation-wildcard
counts, and hashes of the source signature databases.

The old `Math_Mul64` label actually identified double addition (`__adddf3`).
`Math_Div64` becomes `__divdf3`, and `Render_StepEntityPool` becomes `PutDispEnv`.
The symbol map, split configuration and C callers now use those SDK names.
Their report identities remain `libmath/ADDDF3/__adddf3`,
`libmath/DIVDF3/__divdf3`, `libgpu/SYS/PutDispEnv` and
`libcd/C_011/StCdInterrupt`, all in `main-psyq`. Existing range-based attribution
already proved these identities; moving ASM to C does not change the denominator.
These remain individual linked units, not reconstructed complete SDK TUs.

The software double helpers reuse `MathU64` and `MathDoubleBits` for mantissa
arithmetic, alignment, rounding and exponent packing. The `Math_Neg64` prototype
now describes its explicit output pointer, and its definition shares that type.
`PutDispEnv` takes and returns `DISPENV *`; its callers share that declaration.
The cached rectangle halfwords retain the original unsigned reads followed by
sign extension. State-relative cache accesses and the timing tables remain
reconstruction debt. The final memcpy uses an unprototyped function-pointer
call to prevent GCC's builtin expansion; the pointer arguments and word-sized
count retain their original calling convention.

`StCdInterrupt` reuses the 32-byte `StHEADER` and the four-byte `CdlLOC`.
Its frame comparisons read the low halfword of the 32-bit frame counter, and
its location assignment preserves the unaligned four-byte copy. CD port reads
and DMA register accesses are volatile. The two discarded status halfwords
and the eight discarded port reads are preserved. The source also retains the
original conditional subheader fill: when that path is disabled, the later
copy still reads those stack bytes. No initialization has been invented to
change the retail behavior.

The streaming caller exposes previously omitted arguments: `dma_execute` takes
a seventh, unused word, and `mem2mem` a fourth, unused word. Both shared
prototypes and definitions now represent those calls; the helper machine code
and all updated callers remain unchanged, as verified by the complete EXE.

All four use native, unmodified GCC 2.7.2 and the existing GNU assembler route.
There are no instruction-bearing inline ASM, compiler/assembler modifications,
new asm units, or ABI-changing compiler flags. Two removal passes over pins and
empty constraints leave 18 pins, 18 barriers and four gotos, recorded in debt.
The remaining statement expression orders one cached rectangle access.
The global constrained-unit count remains 28 and the legacy asm-body count 71.
The unmatched `__muldf3` experiment is not included or credited.

Validation: `make -j8 check` reproduces the whole retail executable SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`. `make ci` passes 328 tests.
The complete progress/report audits pass; each of these four SDK grid units
reports 100% matched code.

Semantic C now totals 2,411,460 bytes and 10,529 functions, including 397/516
Psy-Q functions and 68,876 Psy-Q bytes. The report also includes BIOS-trampoline
credit introduced separately by commit `b300b7ea`: 692 bytes / 57 functions
across the project, including 524 bytes / 43 Psy-Q functions. With that existing
policy, the displayed totals are 2,412,152 bytes / 10,586 functions, and Psy-Q
is 440/516 with 88.64% matched code. This change contributes only the four C
functions and 5,224 bytes listed above.
