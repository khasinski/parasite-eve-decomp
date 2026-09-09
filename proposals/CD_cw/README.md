# LIBCD CD_cw

Complete C reconstruction of retail `0x8007B558` (1036 bytes), using the
shared CD callback prototypes and interrupt-event structure. Stock GCC 2.8.1
with unsplit addresses scores 91.50193%; there are no pins, barriers or
instruction ASM. The function remains assembly until its full match is
recovered.

The source validates required parameters, waits for an earlier command,
caches Setloc/Setmode parameters, resets completion state and writes the
parameter FIFO followed by the command register. Nonzero mode returns
immediately. Blocking mode waits for completion, dispatches pending callbacks
when necessary, checks both frame and iteration timeouts, copies the eight
response bytes, and reports disk-error completion as -1.

An inline timeout helper reproduces the original intermediate failure result;
the final status accumulator reproduces the original return structure. These
improved the initial direct C reconstruction from 81.312744% to 84.200775%.
Initializing the diagnostic table pointers only after entering the wait,
with an explicit do/while loop, raises this to 89.03089%. Reusing an inline
eight-byte copy helper gives the selected 91.50193%: the output pointer is
copied into a temporary register rather than modified in its saved register.
The final candidate passes all 3072 behavior comparisons. Explicit pointer
barriers, a parameter-count pointer and extra diagnostic temporaries do not
improve this source; no constraints were retained.
Remaining differences include table-address computation, register lifetimes,
and scheduling around diagnostic/callback calls. Earlier experiments with
the 84.200775% source: an equivalent separate inline dispatcher gave no
improvement, split addresses scored
77.189186%, disabling first scheduling 79.084946%, and disabling expensive
optimizations 84.11969%; none improved that stock configuration.

Provenance: LIBCD `BIOS_1.OBJ` exports `CD_cw` at text offset 0xAAC and the
following `CD_vol` at 0xEB8. This exact 1036-byte range matches retail:
170 identical words and 89 differences restricted to relocation fields.
`verify_sdk.py` reproduces that check using `psyk`. This establishes membership
in LIBCD and corrects its old `main/main/CD_cw` manifest classification to
`main/psyq/libcd/CD_cw`, without adding a function boundary or C match credit.
It does not prove that the whole BIOS_1 object matches.

```sh
tools/scripts/cc.sh proposals/CD_cw/candidate.c /tmp/CD_cw.o
tools/objdiff/objdiff-cli diff \
  -1 expected/build/USA/asm/USA/main/psyq/libcd/CD_cw.s.o \
  -2 /tmp/CD_cw.o -o /tmp/CD_cw.json
python proposals/CD_cw/verify_behavior.py /tmp/CD_cw.o
python proposals/CD_cw/verify_sdk.py /path/to/BIOS_1.OBJ
```

The behavior verifier requires Unicorn and pyelftools. It covers 3072 cases:
required/null parameters, nullable result buffers, command-byte truncation,
blocking/nonblocking mode, debug levels, immediate/polled/callback completion,
combined and separate callback events, disk errors and both timeout paths,
including their exact thresholds. It compares returns, state, FIFO/MMIO
traces, external call arguments, callbacks and copied responses. Reversing
the blocking-mode test is rejected as a negative control. Hardware and
external calls are modeled; signed-counter overflow and infinite hardware
waits are outside this finite test set.
