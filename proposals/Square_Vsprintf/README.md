# Square_Vsprintf

Current best verified candidate: `constrained_gcc281.c`, **99.500916%** under
stock GCC281/MASPSX, with all 1,471 retail-oracle cases passing. This remains
a nonmatching proposal. Earlier scores below record the search history.

`Square_Vsprintf` is a variadic Psy-Q formatter at `0x80071A84` (2,180 bytes).
The `Square_` prefix is this project's symbol name. Psy-Q 4.6 and 4.7
`LIBC.LIB` archives export the same object as `sprintf`; its ABI is `int (char
*dest, char *format, ...)`, not the ISO C `vsprintf(char *, const char *,
va_list)` interface. The prologue stores `a1..a3` and begins the argument
cursor at the first variadic slot.

The recovered candidate describes the real formatter rather than a wrapper:

- parser for `-`, `+`, space, `#`, `0`, decimal width, `*`, precision, and
  `h`/`l`/`L` modifiers;
- conversions `d`, `i`, `u`, `o`, `p`, `x`, `X`, `c`, `s`, and `n`;
  floating-point conversions are absent, as documented for Psy-Q `sprintf`;
- Psy-Q's `#s` extension: when the alternate-form flag is present on `%s`,
  the pointed-to value is a byte-length-prefixed string.  Ordinary `%s` is
  NUL-terminated; a precision uses `memchr` to bound that scan;
- the 0x200-byte reverse numeric workspace, followed by the 12-byte format
  specification copied from `D_80094528`;
- string and numeric padding behavior implemented by the retail function.

It is intentionally a proposal, not a manifest C unit. The Psy-Q 4.6 and 4.7
`SPRINTF` object is `0x890` bytes and compares to the retail range with only
34 link-time relocation words different: the format template, jump table,
hexadecimal digit tables, local branch targets, and the three imported libc
calls. The other 514 words, including the three trailing padding nops, is
identical. This establishes the exact upstream object and its exported name;
the archive records its source as `C:\\PSX\\SRC\\C2\\SPRINTF.C`.

GCC 2.7.2 with `-fno-strength-reduce -fno-force-mem` is the closest available
stock profile. It emits the retail 0x250-byte stack frame and exact register
save layout. Its 0x864-byte body is 32 bytes shorter than the Psy-Q object's
0x884 bytes because it omits eight nops; every other opcode has the same
histogram. Objdiff reports 95.88%, while the normalized instruction streams
have a 513-instruction common subsequence out of the retail 545. Its generated
`.rodata` switch table is exactly 0xB4 bytes (45 entries), matching
`jtbl_80011644`; every pair of cases that shares a retail target also shares a
candidate target. The table therefore belongs to this function and is not a
separate blocker.

The remaining work is to reproduce Psy-Q's eight scheduler nops and the
few resulting register-allocation choices from source.  A sweep of the stock
GCC 2.7.2 optimisation switches did not improve on this profile: for example,
`-fno-cse-skip-blocks` creates a nearly equal-length body but drops the objdiff
score to 91.99% by changing the format and variadic-cursor registers.  No
assembly boundary or binary-match accounting has been changed.

## Constrained stock-pipeline candidate (2026-09-09)

`constrained_registers.c` improves objdiff from 95.88% to **98.9781%**.
It retains GCC 2.7.2 and `-fno-strength-reduce -fno-force-mem`, with no
compiler or MASPSX changes and no instruction assembly. Four empty memory
barriers after the numeric argument-cursor stores prevent speculative flag
loads. For `%n`, two local register constraints and two input-only barriers
preserve the flag load, cursor store, and subsequent bit test.
These constraints are experimental; minimization remains pending an exact match.

The numeric conversions and `%n` now have the target instruction sequence.
The remaining instruction-order differences are:

- Target +0x594: loading the uppercase digit-table address finishes in the
  jump delay slot. The candidate completes the address first and adds a nop
  after the jump, increasing the body by four bytes.
- Target +0x87C: stack restoration occupies the return delay slot. GCC 2.7.2
  emits restoration before the return and a nop in the delay slot.

The candidate body is 0x888 bytes; the target body is 0x884. The expected
objdiff symbol includes another twelve bytes of trailing padding (0x890
in total), which also affect the reported percentage. Relocated jump targets
and jump-table entries shift with the extra instruction; 98.9781% is an
objdiff score, not a byte-match claim.

GCC 2.8.1 fixes the return slot but changes the template-copy registers,
parser scheduling, and address/switch-table generation. With the same
constraints and `-mno-split-addresses`, it scores 98.77372%. Disabling the
second scheduler or changing the aggregate copy did not produce an exact
candidate. Production remains the original ASM unit.

Reproduce the current comparison from the repository root:

```sh
tools/scripts/cc.sh proposals/Square_Vsprintf/constrained_registers.c /tmp/sprintf-candidate.o
tools/objdiff/objdiff-cli diff -1 expected/build/USA/src/main/psyq/libc/Square_Vsprintf.c.o -2 /tmp/sprintf-candidate.o -o /tmp/sprintf-diff.json Square_Vsprintf
```


Inspection of GCC 2.7.2 `mips_epilogue_delay_slots` explains the remaining
return ordering: a nonempty frame saving more than RA gets zero epilogue
delay slots. `function_epilogue` then emits stack restoration before the
return. This is a compiler limitation for this frame shape, so further work
should focus on the GCC 2.8.1 source variant, not more 2.7.2 scheduling flags.

## GCC 2.8.1 parser reconstruction (2026-09-09)

`constrained_gcc281.c` now scores **99.16058%** under stock GCC 2.8.1,
`-mno-split-addresses -fno-strength-reduce -fno-force-mem`, and unchanged
MASPSX. Its return sequence matches. The template copy uses explicit typed
field loads/stores with three local register constraints and one memory
barrier after the stores. A fourth pin on the precision value and an
input barrier on the three loaded values were both removable, including
when removed together.

The parser preserves the starting format pointer explicitly for the width
and precision star arms. This reproduces the original `base + 1` and
`base + 2` accesses and their temporary lifetimes. It eliminates all parser
instruction differences without parser pins or barriers.

The complete objdiff now has just one executable-code window differing:

```text
                    target                         candidate
+0x594              lui a3, %hi(digits_upper)       lui a3, %hi(digits_upper)
+0x598              j hexadecimal                  addiu a3, a3, %lo(digits_upper)
+0x59C              addiu a3, a3, %lo(digits_upper)  j hexadecimal
+0x5A0                                             nop
```

The candidate body remains 0x888 bytes versus the retail 0x884 bytes.
Objdiff also reports three deleted trailing padding nops, since the expected
symbol includes the SDK object's padding. This percentage is not an exact
linked-byte match. Production still uses the original assembly.

Turning address splitting on allows scheduling the low address half into
the jump slot, but also changes the template-address registers, both digit
address registers and switch-table dispatch. It does not produce an exact
replacement. With splitting disabled, GCC represents the address load as an
indivisible `la` macro; its MIPS `high`/`low` patterns require
`mips_split_addresses`. See the stock GCC 2.8.1
[`movsi`, `high` and `low` patterns](https://github.com/gcc-mirror/gcc/blob/releases/gcc-2.8.1/gcc/config/mips/mips.md).
Further work should resolve those source/register choices with address
splitting enabled; neither tool patches nor inserted CPU assembly are used.

Reproduce:

```sh
tools/scripts/cc.sh proposals/Square_Vsprintf/constrained_gcc281.c /tmp/sprintf-gcc281.o
tools/objdiff/objdiff-cli diff -1 expected/build/USA/src/main/psyq/libc/Square_Vsprintf.c.o -2 /tmp/sprintf-gcc281.o -o /tmp/sprintf-gcc281.json Square_Vsprintf
```

## Constraint reduction (2026-09-09, formatter detour)

The GCC 2.8.1 candidate retains **99.16058%** after removing the local
register pins on the template's `flags` and `width`, and the input-only
barrier on `%n`'s `isShort` test. Each was tested separately and all three
were then removed together. The resulting `.text` and `.rodata` are identical
to the previous candidate, including relocation records. The candidate now
has three local register pins and six empty barriers. This is a reduction
in experimental constraints, not promotion to a matched C unit.

A fresh address-splitting trial scores 98.79379%. Pinning the digit pointer
and constraining the template pointer does not restore the target switch
dispatch. With splitting enabled, that dispatch materializes the complete
table address before adding the index (five instructions); the target adds
the index to the high part and uses the low relocation on the load (four).
This is separate from the uppercase digit-table scheduling problem. Stock
GCC's `mips_check_split` commentary describes this indexed-address limitation;
changing only the digit pointer's register cannot resolve the complete TU.

## Narrow memory constraints (2026-09-09)

The five full memory clobbers in `constrained_gcc281.c` are now input-only
constraints on the actual stored objects: `work.spec` after the template
copy, and `args` after each numeric argument-cursor update. Fresh stock GCC
2.8.1 compilations before and after have identical `.text`, `.rodata`,
`.rel.text`, and `.rel.rodata` contents. This preserves 99.16058% without
claiming writes to unrelated memory. All three pins and six empty barriers
were individually tested for removal again; each removal changes the output.
The remote permuter's checked-in base inputs retain the earlier source as a
stable starting point for the running experiment.

## Refined address-splitting candidate (2026-09-09)

`constrained_split_gcc281.c` improves the split-address profile from 98.79379%
to **98.85949%**. An empty read/write constraint on the completed template
pointer prevents reuse of its high-address temporary for the first field
load. Pinning `digits` to a3, without a following barrier, also restores both
digit-table address pairs. The uppercase address now finishes in the jump
delay slot, exactly as in retail. The complete objdiff has only the switch
dispatch window and the expected object's three trailing padding nops differing.

The remaining executable window is:

```text
target                              candidate
sll   v0, v1, 2                     lui   v0, %hi(table)
lui   at, %hi(table)                 addiu v0, v0, %lo(table)
addu  at, at, v0                     sll   v1, v1, 2
lw    v0, %lo(table)(at)             addu  v1, v1, v0
                                    lw    v0, 0(v1)
```

This candidate is an alternative search seed, not an improvement over the
99.16058% unsplit candidate and not a matched production replacement. It uses
four pins and seven empty constraints; further minimization follows an exact
match. On darwine, `split-refined` uses this source with the preprocessor alias
expanded and the existing split compiler wrapper. Its baseline permuter
penalty is 325. A separate four-worker search runs alongside the original
28-worker search, saving all unique improvements over its baseline.

## Retail behavioral oracle (2026-09-09)

`verify_behavior.py` links a candidate object at the retail formatter and jump-table
 addresses, then runs both it and the SHA-1-verified USA executable in
Unicorn MIPS emulation. The rest of the executable remains retail, including
`memmove`. The BIOS A0 services used by the retail `strlen` and `memchr` thunks
are modeled identically for both executions; a PSX BIOS ROM is not required.

The 1,471 deterministic cases cover integer conversions and edge values,
flags, width/precision (including stack-passed `*` arguments), modifiers,
strings and the length-prefixed `#s` extension, characters, `%n` writes, and
mixed arguments. Checks compare return values, destination bytes and guards,
and argument memory. Both current split and unsplit candidates pass. The
rejected penalty-120 hexadecimal candidate fails case 832 (`before:%x:after`,
value zero): it emits a NUL in place of the digit `0`.

```sh
python3 -m venv /tmp/sprintf-oracle-venv
/tmp/sprintf-oracle-venv/bin/pip install unicorn pyelftools
tools/scripts/cc.sh proposals/Square_Vsprintf/constrained_gcc281.c /tmp/sprintf.o
/tmp/sprintf-oracle-venv/bin/python proposals/Square_Vsprintf/verify_behavior.py /tmp/sprintf.o
```

The script requires `mipsel-none-elf-ld` on PATH and defaults to
`build/USA/main.exe`. It rejects an EXE whose SHA-1 differs from retail. These
finite tests help reject incorrect permutations; they do not establish a full
semantic proof, model PSX hardware timing or replace exact linked-byte checks.

## Saved remote permutations: behavioral audit

Rechecked all 15 saved unsplit improvements (penalties 120 through 150) from
`darwine:/home/hasik/sprintf-permuter-20260909/unsplit` using the local stock
GCC281/MASPSX configuration and the retail oracle. None passed: 12 produced
retail output mismatches; output-125-1, output-125-2 and output-125-5 encountered
CPU exceptions at address zero. Exceptions are recorded separately from
verified output mismatches. Exact source SHA-256 identities and classifications
are in `permuter/unsplit-audit.json`; this audit does not claim new matches.
At that audit, the valid 99.16058% candidate remained unchanged. The original remote
search was confirmed live beyond 1.94 million iterations and split-refined
beyond 1.26 million; neither was restarted during this audit.

## Corrected hexadecimal prefix scheduling

The remote penalty-120 candidate moved `--src` outside the alternate-form
condition. That changes the source span even without `#`, causing the zero
hexadecimal regression. The maintained candidate now computes only
`prefixEnd = src - 1` before the condition. Inside the condition it writes the
conversion character through that temporary, subtracts two from `src`, and
writes the leading zero. Without the flag, `src` remains unchanged.

This ordinary-C change improves objdiff from 99.16058% to **99.500916%** against
`expected/build/USA/asm/USA/main/psyq/libc/Square_Vsprintf.s.o`. No compiler flags,
pins or barriers were added. Both the repaired experimental source and the
readable maintained source pass all 1,471 oracle cases. Two alternative repairs
that reassigned the temporary to `src` scored 99.31193% and 99.484406%; keeping
the two-byte adjustment together produced the retained result. This repairs
the invalid permutation rather than accepting its lower permuter penalty.
Production assembly and match accounting remain unchanged until exactness.


## Separate hexadecimal prefix updates (2026-09-10)

The maintained unsplit source now scores **99.68807%** (previously
99.500916%). Inside the alternate-form condition it decrements `src`, stores
the conversion character, then decrements `src` again and stores zero. An
empty `+r` constraint between those two stores preserves the two pointer
updates instead of folding them into a single subtraction by two. Removing
that constraint loses the matching prefix instruction sequence. The unused
`prefixEnd` temporary is removed. There are now three pins and seven empty
constraints in this candidate; no instruction ASM or tool changes were added.

Ignoring the jump-table symbol name, the only remaining instruction mismatch
is the uppercase digit-table address: its low-half ADDIU precedes the jump,
followed by an extra NOP, whereas retail schedules ADDIU in the jump delay
slot. Candidate text is 2184 bytes versus 2180 retail. This remains a proposal,
not a replacement for production ASM.

The same prefix update applied to `constrained_split_gcc281.c` improves it to
**99.39449%**. Its digit-table selection matches, but the switch dispatch
materializes the jump-table address and indexes it differently. Stock AT
allocation and scheduler/CSE flag trials did not close that remaining gap.

Both maintained sources pass all 1,471 retail-oracle cases, including zero
hexadecimal values without alternate form. Percentages are object-diff scores,
not matched production-function credit. The existing remote `unsplit-refined`
search was confirmed live at more than 150,000 iterations; its previously
rejected saved candidates were not accepted or substituted for these sources.
