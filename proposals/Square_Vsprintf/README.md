# Square_Vsprintf

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
