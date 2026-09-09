# SPU shutdown

`candidate.c` expresses SpuQuit's complete 116-byte retail behavior as ordinary
C with no pins, barriers or instruction ASM. It clears the called flag before
entering the critical section, clears both callbacks, removes the data callback,
closes and disables the DMA event, then exits the critical section. The function
acts only when the called flag equals one. It reloads the event handle after
CloseEvent. API return values are ignored.

Stock GCC272 gives 88.965515%. The two symbolic zero stores precede their calls
instead of occupying the call delay slots; the argument setup also differs.
The production body still contains its legacy ASM windows until a byte match
is recovered. GCC281 unsplit scored 84.13793% before event-symbol cleanup;
GCC272 with call-used-AT scored 87.24138%, GCC281 76.51724%, and disabling both
GCC281 schedulers with call-used-AT scored 45.103447%. No such flags are kept.

The candidate and production source use the shared _spu_EVdma declaration,
removing the duplicate local D_8009B384 alias. Local API declarations now use
int for EnterCriticalSection and PSX-width s32 results for CloseEvent and
DisableEvent; ExitCriticalSection remains void. Psy-Q 4.6 INCLUDE/LIBAPI.H
lines 36/40/77/78 declares the event results as long (32-bit on PSX), critical
entry as int, and exit as void. These type corrections do not remove the
production ASM windows or claim new matching-function credit.

```sh
tools/scripts/cc.sh proposals/SpuQuit/candidate.c /tmp/spuquit.o
tools/objdiff/objdiff-cli diff \
  -1 expected/build/USA/src/main/psyq/libspu/s_q.c.o \
  -2 /tmp/spuquit.o -o /tmp/spuquit.json
python proposals/SpuQuit/verify_behavior.py /tmp/spuquit.o
```

The 32-case Unicorn comparison covers called flags 0/1/2/FFFFFFFF, two initial
callback patterns, two event handles, and CloseEvent optionally replacing the
handle. It compares all relevant word reads/writes, ordered API calls and
arguments, final state and stack. External calls are modeled with nonzero
return values; two-word NOP stubs and second-word hooks avoid partial execution
of their original prologues. This is a finite function-level comparison, not
physical interrupt concurrency or kernel execution verification.

Negative controls for a nonzero guard and a non-null data callback argument
are rejected. An omitted-reset experiment hit a Unicorn CPU exception rather
than the expected assertion, so it supplies no additional negative-control claim. Production prototype/alias cleanup
retains the main retail SHA, all 191 overlays, and source/organization/debt gates.
