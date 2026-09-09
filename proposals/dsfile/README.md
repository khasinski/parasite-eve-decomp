# Psy-Q DSFILE translation unit

`candidate.c` contains all six functions of the original TU in one C file. It uses the shared
`DslFILE` and `DslDirectoryCacheEntry` layouts and local byte-oriented ISO-9660
volume, path-table and directory-record types. No instruction ASM, pins or
empty barriers are present. Only `DsSearchFile` is public; the five helpers
are static, consistent with the SDK object's sole exported definition.

## Original object evidence

Psy-Q 4.6 `LIBDS.LIB`'s `DSFILE.OBJ` contains 2400 text bytes. Comparison with
retail 0x80081414..0x80081D74 gives 429 identical instruction words and
171 differences, exactly at all 171 relocation sites. These comprise 47
jump/call relocations and 62 HI/LO pairs. Opcode and register bits match even
at those sites. `verify_sdk_tu.py` repeats this check against an extracted
SDK object without changing the toolchain or input binaries.

The object's 0x2400-byte BSS corresponds exactly to the contiguous storage
for 64 file entries (0x600), 128 directory entries (0x1600) and one sector
buffer (0x800). The candidate now owns these three static arrays, the two
initialized state words, and all 19 diagnostic/signature/name strings.
Readable C names retain the retail linker names through symbol aliases.
Every owned symbol's offset matches retail; the 490 emitted read-only bytes
and eight initialized state bytes are byte-identical. Storage ownership does
not change any of the six function match percentages below.

The SDK sections have six additional trailing zero bytes in read-only data
and eight in initialized data. These remain a layout question for promotion;
the candidate does not invent unused fields or padding arrays to absorb them.

| Function | TU offset | Retail bytes | Current match |
| --- | ---: | ---: | ---: |
| DsSearchFile | 0x000 | 736 | 90.46196% |
| _cmp | 0x2E0 | 32 | 100% |
| DS_newmedia | 0x300 | 708 | 79.63842% |
| DS_searchdir | 0x5C4 | 164 | 98.902435% |
| DS_cachefile | 0x668 | 668 | 93.97605% |
| ds_read | 0x904 | 92 | 100% |

This is a source reconstruction candidate, not a promoted production TU.
The production manifest retains its current boundaries until the entire
unit matches; no additional production progress is credited by this draft.
The standalone DsSearchFile source was consolidated here.

## Behavioral verification

`verify_behavior.py` compares the three cache helpers against verified retail
MIPS using synthetic volume descriptors, path tables and directory sectors.
The checker places owned data at retail addresses and checks its symbols and
initialized bytes before execution. All 259 cases agree on return values, both complete caches, the sector buffer,
the cached-directory state and diagnostic/read-call traces. Cases include
read failures, invalid volume identifiers, empty tables, root/parent names,
cache hits, directory/file limits and diagnostic levels. `ds_read` is modeled
for these tests; `CdIntToPos` executes the actual retail implementation.

The focused DsSearchFile checker adds 768 cases, including `DS_cachefile`'s
actual error return -1. The source preserves the retail zero test rather
than changing it to a general error check. These finite tests supplement
instruction comparison; they do not verify physical CD hardware behavior.
Negative controls reading the wrong parent-ID byte or returning zero instead
of -1 on a directory read failure are both rejected.

```sh
tools/scripts/cc.sh proposals/dsfile/candidate.c /tmp/dsfile.o
python3 proposals/dsfile/verify_behavior.py /tmp/dsfile.o
python3 proposals/DsSearchFile/verify_behavior.py /tmp/dsfile.o
python3 proposals/dsfile/verify_sdk_tu.py /path/to/DSFILE.OBJ
cc -std=gnu11 -fsyntax-only -Werror=uninitialized -I include \
  -include tools/tests/host_psyq.h proposals/dsfile/candidate.c
```

The behavioral checkers require Unicorn, pyelftools and MIPS binutils.
The SDK checker requires `psyk`. Extract the object from an available SDK
with `psyk extract LIBDS.LIB` in a temporary directory.

## Confirmed historical API aliases

The SDK object's external relocations establish these names for the current
retail symbols. Production renaming and signature cleanup are still pending.

| Current symbol | SDK import |
| --- | --- |
| CdRom_GetDiskType | DsShellOpen |
| CdIntToPos | DsIntToPos |
| CdRom_StartRead | DsRead |
| Sys_VSyncTimeout | DsReadSync |

Remaining matching work includes pointer/index induction, register allocation,
unaligned four-byte copies and symbolic-address scheduling. In particular,
DS_searchdir's indexed load expands through AT rather than the target's v0.
The compiler and MASPSX remain unmodified.

## DS_cachefile source refinement

`DS_cachefile` improved from 66.98203% to 93.97605% without compiler pins,
empty barriers or instruction ASM. Its loop uses one typed ISO directory
record cursor and increments the file count after processing a record,
breaking at 64 before the next sector-boundary check. A switch selects the
two fixed cache slots for `.` and `..`; remaining fields use indexed cache
accesses. The one-based directory lookup retains the subtraction in the
field address rather than subtracting from the index before multiplication.
All 1027 behavioral cases still pass and the other five function scores
are unchanged. Remaining differences include unaligned-copy registers,
indexed-load expansion and symbolic-address scheduling.
