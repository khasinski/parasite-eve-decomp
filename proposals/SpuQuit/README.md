# SPU shutdown

`candidate.c` expresses SpuQuit's complete 116-byte retail behavior as ordinary
C with no pins, barriers or instruction ASM. It clears the called flag before
entering the critical section, clears both callbacks, removes the data callback,
closes and disables the DMA event, then exits the critical section. The function
acts only when the called flag equals one. It reloads the event handle after
CloseEvent. API return values are ignored.

Production matches all **116/116 linked retail bytes** using native GCC 2.7.2
and unmodified GNU as. Both former instruction-asm windows are now ordinary C;
the assembler places the global stores in the retail call delay slots. There
are no pins or barriers. The exact-byte regression in
`tools/tests/test_spu_lifecycle.py` includes relocations at the retail address.

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
