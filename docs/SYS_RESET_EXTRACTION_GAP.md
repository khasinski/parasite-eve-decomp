# Restored sys_reset extraction

The old scanner slice contained only PE.IMG sector 927 (0x800 bytes), linked
at 0x8010BCF8. It ended at a return instruction and omitted its delay slot,
three following functions, and the decoder tables referenced by existing C.

The restored slice contains sectors 927 through 964 (0x13000 bytes). Sector
965 begins the existing menu_memcard slice, so the windows do not overlap.
The sys_reset SHA-1 is e7331867b4d1464da4d5f386d9137eaa1c98422b. The private
assets commit is 5a4694459790054896ad29b64921da617c7b171e, pinned by CI.

## Code boundaries

| Function | End exclusive | Size | Previous coverage |
| --- | --- | --- | --- |
| func_8010C4CC | 0x8010C4FC | 0x30 | First 0x2C bytes only |
| func_8010C4FC | 0x8010C860 | 0x364 | Missing |
| func_8010C86C | 0x8010C89C | 0x30 | Missing |
| func_8010C89C | 0x8010CBFC | 0x360 | Missing |

The missing word at 0x8010C4F8 is sw at,0(t0), the delay slot of jr ra.
All direct branch targets stay within their respective function ranges.
Both decoder functions have a completion return and a suspended-state return;
the latter blocks are branch targets, not separate functions. Three zero
words at 0x8010C860..0x8010C86B are inter-function padding. Existing boot
display sources call func_8010C86C and func_8010C89C; the memcard video header
also declares the latter. These entries are not speculative disassembler
labels inferred from data.

## Data boundary evidence

Code ends at file offset 0xF04, address 0x8010CBFC. That address is the
compressed-table source used by the already matching
SysReset_ExpandDecoderTable. Decoding the stream with its recovered algorithm
consumes offsets 0xF04..0x1D10 inclusive (0xE0D bytes) and produces 0x11000
bytes. After its four-word XOR reconstruction, the output exactly equals
the initialized table at file offset 0x1E64, address 0x8010DB5C.
The table ends at 0x8011EB5C, followed by the limit and saved-state storage
referenced by both decoders. This is evidence for the code/data split, not
a decompilation score.

The restored data is an explicit data subsegment, so it is not counted as
new executable code. Existing matching C functions are unchanged. The four
functions above remain original assembly; no score-zero C claim is made.
The newly restored executable function bytes total 1784: four delay-slot
bytes plus three full functions totaling 1780 bytes.

## Verification

On darwine, stock toolchain make OVERLAY=sys_reset overlay-check reproduces
the entire 0x13000-byte slice, and make verify passes. Assembly inspection
uses the disc target and stock GNU objdump. Research artifacts are under
scratch/match_inventory locally and /home/hasik/fx-search-archives/match_inventory
on darwine. The complete overlay build and report audit pass. The report
contains 11,675 functions, of which 11,628 receive semantic-C credit; all
three restored entries are present. Code coverage is 3,532,448 / 3,558,868
bytes (99.25763%). This coverage measure does not substitute for the
weighted Levenshtein score used to accept C candidates.

The published CI run [37333637624](https://github.com/khasinski/parasite-eve-decomp/actions/runs/37333637624)
on commit 48a5cd21c995a17752f5d2039478d62d00e96b39 passed. Its
SLUS_006.62_report artifact confirms the counts above. This is the current
inventory report provenance, replacing the pre-extraction baseline.
