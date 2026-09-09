# StopRCnt in the counter translation unit

`candidate.c` reconstructs the ordinary CPU operation in C and retains all
five functions from `psyq/libapi/counter.c` in their existing source order.
StopRCnt clears the selected bit mask in the interrupt mask register and
returns 1. The index is the low 16 bits of the argument. Unlike GetRCnt and
ResetRCnt, it does not reject indices of 3 or above; adding a range check
would change the original behavior.

Stock GCC 2.7.2 and pinned, unchanged MASPSX give these upstream objdiff
results against the complete expected counter object:

| Function | Match |
| --- | ---: |
| SetRCnt | 100% |
| GetRCnt | 100% |
| StartRCnt | 100% |
| StopRCnt | 98.46154% |
| ResetRCnt | 100% |

StopRCnt has the correct 52-byte length and instruction ordering. Only the
three-instruction indexed mask load differs: the target uses v0 as both
address scratch and load destination, whereas MASPSX expands the compiler's
`lw v0,D_8009B7D4(a0)` using AT for the address scratch. This affects four
register operands across the LUI, ADDU and LW; all other instructions match.
The plain candidate requires no pins, barriers or instruction assembly.
It has not replaced production's legacy assembly window.

Evidence and unsuccessful alternatives:

- Stock MASPSX's `is_addend and r_source` load expansion explicitly uses AT.
  Its older-version `addiu_at` branch also uses AT and emits another address
  instruction. Changing the tool is outside the accepted pipeline.
- Pointer addition, explicit byte-offset arithmetic and a pointer-to-array
  expression all produce the same 98.46154% result. They are not retained.
- Volatile table loads change scheduling and score 44.23077%; the mask table
  is ordinary constant data, so this is not a justified final qualifier.
- GCC 2.8.1 changes all five functions with address splitting enabled.
  Disabling splitting retains the same indexed-load limitation and also
  changes GetRCnt and ResetRCnt's branch/return layouts.
- The tested 2.8.1 strength-reduction, force-memory, expensive-optimization,
  scheduler and CSE options do not recover a matching whole TU.
- Stock GCC 2.7.2 rejects `-msplit-addresses` as an invalid option.

Reproduce from the repository root:

```sh
tools/scripts/cc.sh proposals/StopRCnt/candidate.c /tmp/counter-candidate.o
tools/objdiff/objdiff-cli diff -1 expected/build/USA/src/main/psyq/libapi/counter.c.o -2 /tmp/counter-candidate.o -o /tmp/counter-candidate.json
```
