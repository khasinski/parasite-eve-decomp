# `_patch_card2`: linked byte match, pending toolchain integration

`patch_card2.c` matches all 112 bytes at `0x8007E470` (retail file offset
`0x6EC70`). The SDK 4.3 LIBCARD/PATCH provenance identifies this as
`_patch_card2`; the current assembly symbol is `func_8007E470`.

Linked SHA-256:
`52d272cf95e2863eaef8e901126f37b4785a5364c7f383385b2e3c7724220936`.

The routine enters the critical section, invokes BIOS B0 selector 0x57,
loads the pointer at table offset 0x16C, reads the original instruction at
that pointer +0x9C8, and copies five instruction words there. It flushes the
cache and returns without calling ExitCriticalSection. The original global
RA save slot is shared with `_patch_card`; this is not a reentrant routine.
The unused instruction read is retained explicitly rather than discarded.

## Compiler and linker

Use the unmodified native GCC 2.95.2 configured for
`mipstx39el-unknown-elf`, with:

```
-O2 -G0 -mips1 -mcpu=3000 -msoft-float -mno-abicalls -mabi=eabi
```

Preprocess the C source before invoking cc1. Assemble its output with the
repository's unchanged MASPSX (`--aspsx-version=2.56`) and GNU as:
`-EL -G0 -march=r3000 -mtune=r3000 -no-pad-sections`.

Link with:

```
savedReturnAddress = 0x800A34E0;
EnterCriticalSection = 0x80072714;
FlushCache = 0x800726C4;
templateStart = 0x8007E3C8;
templateEnd = 0x8007E3DC;
SECTIONS {
  .text 0x8007E470 : SUBALIGN(4) { *(.text) }
  /DISCARD/ : { *(.reginfo) *(.mdebug) }
}
```

The native compiler currently resides on darwine at
`/home/hasik/psyq-exitcard-abi-20260914/source/gcc-2.95.2/gcc/cc1`.
Its SHA-256 is
`b5760a365853baeaab49b808de695a2e822936b8f12f7a8afef88d7ceb64e013`.
The source audit compared 1778 original source/header/MD/configure files
against the GNU 2.95.2 archive and found no modifications. No instruction
postprocessing or new instruction ASM is used. EABI is a means of generating
this routine's bytes, not evidence of which compiler originally produced it.

## Validation and remaining work

The final named source was compiled and compared after linking. A 256-case
differential test (512 MIPS executions) checked randomized source words,
original instruction values, table/code addresses and initial registers.
Assertions cover BIOS selector, exact five-word writes, global RA save,
call order, unchanged SP and callee-saved registers, and return address.
BIOS and cache services are modeled stubs; real hardware cache behavior is
not emulated. The validated binary is identical to the final named binary.

Minimization removed the kernel pointer pin and its empty barrier.
Seven register bindings remain (including the global RA binding), together
with four empty barriers. Removing each of the six local pins or each
retained barrier individually broke the tested byte match; the global RA
binding expresses the original entry/return convention. No instruction ASM
or explicit NOP was added. These constraints are candidate debt, not yet
part of the production debt baseline.

Local diagnostic artifacts, compiler probes and executable test:
`/tmp/psyq-card-eabi-next/`. Prior EABI compiler build/audit:
`/tmp/psyq-exitcard-next/` and the darwine directory above.

This source is deliberately outside the production manifest. Integrating the
new native compiler reproducibly on developer and CI hosts, shared-symbol
headers, the existing PATCH TU boundary, production debt and full image
verification remain necessary before production promotion. This candidate
therefore does not change reported matched-function totals.
