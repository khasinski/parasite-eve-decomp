# MulRotMatrix CPU-side reconstruction

New C reconstruction of the 232-byte retail function at 0x800786E4.
The function multiplies each input matrix column through the rotation matrix
already held in GTE control registers. It does not load or replace those
control registers. All three input columns are consumed before output writes,
so the matrix is safely updated in place. Translation is preserved. The
retail final SWC2 writes the whole IR3 word, including the matrix padding;
the C writes that padding explicitly from the signed IR3 result.

CPU packing, indexing and stores are C. Hardware transfers and the MVMVA
command remain COP2 operations using the existing pe1/gte.h interface plus
an MTC2 register-1 transfer. No register pins, empty barriers, CPU instruction
assembly or postpasses are added. The two hazard NOPs come from the existing
gte_mvmva macro. A macro groups the three output reads because pointer output
parameters in an inline function caused this old compiler to spill them.

**Not an instruction match:** stock GCC272 produces 228 bytes against
232 retail and objdiff reports 0.0%. The register allocation, halfword
accesses, packing, instruction order and hazard spacing differ. A packed-word
variant also scored 0.0% and was not retained. Production ASM is unchanged;
this contributes no matching-function credit.

```sh
tools/scripts/cc.sh proposals/MulRotMatrix/candidate.c /tmp/mul-rot.o
python proposals/MulRotMatrix/verify_behavior.py /tmp/mul-rot.o
```

**Verification is limited to CPU-side behavior and the GTE interface.**
Unicorn does not execute the PSX GTE here. The verifier replaces COP2
instructions in both bodies with NOPs and services their transfers and
commands from a code hook. MVMVA returns scripted signed-halfword IR values;
its arithmetic, flags and timing are not emulated. VZ transfers are normalized
to their low signed halfword. This checks the same consumed vectors and
commands, rather than requiring unused upper bits of the raw VZ source word
to be identical.

All 1,367 cases pass: 343 boundary-result combinations plus 1,024 random
matrix/result cases. Assertions check three input columns, command and result
read order, all output bytes, final modeled data registers, padding,
translation, return pointer, SP and canaries. Mutants selecting the wrong
first-column input or preserving the old padding are rejected by assertion.
This does not establish complete GTE hardware equivalence, instruction timing,
memory access widths or behavior with concurrent observers.
