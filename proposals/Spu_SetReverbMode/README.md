# Transfer-mode state setter (current name Spu_SetReverbMode)

The ordinary-C candidate replaces the production fabricated
`SpuReverbDataPage` and AT-bound page pointer with two named scalar stores.
The page calculation 0x800A0000 - 0x4BE8 addresses `D_8009B418`. Its consumers
in `spu_transfer.c` choose programmed I/O when this value is nonzero. The
function stores the unmodified argument in `D_8009B38C`, stores/returns one
only for argument one, and stores/returns zero otherwise. The existing symbol
name is retained; no unsupported SDK export rename is made.

Stock GCC272 produces 52 bytes against retail's 48, scoring 86.666664% with
objdiff against `expected/build/USA/src/main/akao/Spu_SetReverbMode.c.o`.
The remaining difference is the final symbolic store before JR followed by
NOP rather than SW in the JR delay slot. There are no pins, barriers or
instruction asm. GCC281 unsplit has the same score; split addressing scores
55%. Disabling both instruction scheduling passes does not change those
results. A local AT-pinned pointer to the real symbol also fails to improve
these profiles and was discarded.

```sh
tools/scripts/cc.sh proposals/Spu_SetReverbMode/candidate.c /tmp/spu-mode.o
python proposals/Spu_SetReverbMode/verify_behavior.py /tmp/spu-mode.o
```

The SHA-1-checked retail comparison executes both bodies without external API
stubs. Twelve cases cover mode 0, 1, 2, -1, INT_MIN and INT_MAX with two initial
storage patterns. It asserts both ordered writes, normalized return value,
termination and SP preservation. The candidate remains a proposal: production
code and matching-function accounting are unchanged.
