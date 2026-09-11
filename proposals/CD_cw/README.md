# LIBCD CD_cw

`src/main/psyq/libcd/CD_cw.c` replaces all 1036 bytes at retail
`0x8007B558`. Stock GCC 2.8.1, `-mno-split-addresses`,
`-fno-expensive-optimizations` and GNU assembly produce an exact object
comparison and an exact linked byte comparison. No register pins, compiler
barriers, instruction assembly or compiler modifications are needed.

The function validates required parameters, waits for the previous command,
caches Setloc/Setmode parameters, resets completion state, and writes the
parameter FIFO followed by the command register. Nonzero mode returns
immediately. Blocking mode handles completion callbacks and both timeouts,
copies the optional eight-byte response, and returns -1 for disk errors.

`CdCommandTables` describes the three adjacent 32-entry arrays at
`0x8009B0FC`, `0x8009B17C` and `0x8009B1FC`. CD_cw uses the first array to
reset ready events and the third to count parameters; getintr uses the middle
array to control status updates on interrupt 3. Field offsets and total size
are asserted in the shared header. The initial parameter-count check retains
the existing symbol for the third array; the FIFO loop addresses the same
array through the structure, without stepping beyond a separate C array.

The separate parameter/opcode captures, the zero initialized before parameter
validation, and the unsigned-short completion snapshot retain the stock
compiler's matching register lifetimes. They are source-shape constraints,
not evidence that these were the original local names or declarations.

LIBCD `BIOS_1.OBJ` exports `CD_cw` at text offset `0xAAC` and the following
`CD_vol` at `0xEB8`. This 1036-byte SDK range has 170 identical words and
89 differences confined to relocation fields when compared with retail.
This is one matching fragment of BIOS_1, not a whole-object match claim.

```sh
tools/scripts/cc.sh src/main/psyq/libcd/CD_cw.c /tmp/CD_cw.o
.venv/bin/python proposals/CD_cw/verify_bytes.py
.venv/bin/python proposals/CD_cw/verify_behavior.py /tmp/CD_cw.o
.venv/bin/python proposals/CD_cw/verify_sdk.py /path/to/BIOS_1.OBJ
```

The behavior suite passes 3072 retail/candidate comparisons covering parameter
validation, nullable results, command-byte truncation, blocking/nonblocking
mode, debug levels, callbacks, disk errors and both timeout thresholds. It
compares return values, state, FIFO/MMIO traces, external call arguments and
copied responses. CD_sync and hardware are modeled in this standalone suite;
infinite waits and signed-counter overflow are outside the finite test set.
The byte verifier links at the original address and compares every byte with
the SHA-1-checked retail executable.
