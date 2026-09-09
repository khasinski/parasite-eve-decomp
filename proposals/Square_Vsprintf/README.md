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

The remaining work is to reproduce Psy-Q's eight scheduler nops from source.
No assembly boundary or binary-match accounting has been changed.
