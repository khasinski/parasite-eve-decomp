# SPU register-write TU: C remainder reconstruction

`candidate.c` preserves the existing seven-function TU and replaces the
handwritten divu/zero-check/break/mfhi block in _spu_FsetRXXa with
`rem = value % unit`. Stock GCC272 and pinned upstream MASPSX's supported
`--expand-div` option generate the original unsigned divide and divide-by-zero
trap sequence. No compiler or assembler patch is used.

The candidate is not ready for production. _spu_FsetRXXa scores 97.560974%
(168 candidate bytes versus 164 retail bytes); all six siblings remain 100%.
MASPSX inserts a nop between mfhi v0 and the dependent beqz v0. Retail has
that branch immediately after mfhi, with its own nop delay slot. This shifts
later branch targets by one word. The remainder calculation is reconstructed
in C, but no new whole-function behavioral or hardware-timing claim is made.
The existing _spu_Fw1ts synthetic stack/register model also remains unchanged.

```sh
tools/scripts/cc.sh proposals/spu_register_write/candidate.c /tmp/spu-register-c.o
tools/objdiff/objdiff-cli diff \
  -1 expected/build/USA/src/main/psyq/libspu/spu_register_write.c.o \
  -2 /tmp/spu-register-c.o -o /tmp/spu-register-c.json
```

Without --expand-div the zero-divisor trap is absent (90.243904%); that variant
is not retained. ASPSX 2.21 with --dont-expand-li produces the same remaining
nop and score. Input-only, tied-output and memory barriers after the remainder
also retain 97.560974%; none is included. Upstream remu handling calls
_handle_nop_before_next_instruction with its result register, explaining why
disabling the separate mfhi/mflo hazard option does not remove this nop.
Do not modify the toolchain or restore instruction ASM to call this matched.

The production allocator constraints were independently tested for removal:
offset 99.268295%, shifted 97.439026%, unit 99.268295%, shift 99.51219%,
address 99.02439%. All existing pins are retained. These measurements use the
production divider source, not the new 97.560974% candidate.
