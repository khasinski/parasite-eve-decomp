# MulMatrix0 CPU-side reconstruction

New C reconstruction of the 268-byte retail function at 0x800785D4.
The proposal directory previously contained only target metadata.
The function loads the left matrix into five GTE rotation control registers,
processes the three columns of the right matrix with MVMVA, and writes the
rotation result to a third matrix. All inputs are consumed before output
stores. The result may therefore overlap either input in the tested layouts.
The output translation remains unchanged; padding receives the high half of
the final signed IR3 word, matching the original full-word store.

Indexing, packing and stores are C. COP2 transfers and commands use the
existing pe1/gte.h hardware interface plus an MTC2 register-1 transfer.
The shared MVMVA macro supplies hazard NOPs. There are no register pins,
empty barriers, CPU algorithm instruction assembly or postpasses.

**Not an instruction match:** stock GCC272 and GCC281 each produce 324 bytes
and objdiff reports 0.0% against the 268-byte retail body. The candidate uses
GCC272. Access widths, register allocation, packing, scheduling and hazard
spacing differ. Production ASM is unchanged and no matching-function credit
is claimed.

```sh
tools/scripts/cc.sh proposals/MulMatrix0/candidate.c /tmp/mul-matrix0.o
python proposals/MulMatrix0/verify_behavior.py /tmp/mul-matrix0.o
```

**The verifier tests CPU-side behavior and the GTE interface, not GTE
arithmetic or timing.** COP2 instructions are replaced with NOPs in both
bodies and serviced by a code hook. Commands receive scripted signed-halfword
IR results. Rotation control register 4 and VZ transfers are normalized to
their low signed halfword; unused raw upper bits are not compared.

All **12,303 cases** pass: 343 boundary-result combinations plus 1,024 random
matrix/result cases, each in nine memory layouts. Layouts cover separate
buffers, output equal to either input, identical inputs/all three buffers,
and four partial-overlap arrangements. Assertions check control-register
write order and values at every command, all three input columns, result-read
order, the entire 192-byte arena, final modeled registers, padding, returned
pointer, SP and canaries. Mutants targeting the wrong control register,
selecting the wrong input column or preserving padding fail assertions.

This is finite interface coverage, not proof of full hardware equivalence,
GTE flags, pipeline hazards, exact memory access widths, concurrent observers
or every possible overlap.
