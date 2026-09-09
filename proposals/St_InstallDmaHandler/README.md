# Guarded exception-handler patch installation

New C reconstruction of the 172-byte retail function at 0x8007A134.
It saves the caller RA, enters the critical section, invokes BIOS B0:56,
reads C0 table entry 6 and examines six instruction words at handler+0x28.
Only a complete match with the original template enables copying the six-word
replacement. Both match and mismatch paths flush the cache and exit the
critical section. An already patched handler is left unchanged.

The BIOS service and the identical template/patch sequence are documented in
[PSX-SPX's BIOS patch reference](https://psx-spx.consoledev.net/kernelbios/).
The retail label St_DmaCompleteCallback points to the original six-word data
template, not a callable C body. D_8007A1F8 points to the replacement template.
The candidate references those existing words; it does not embed new machine
instructions or change the production boundaries.

BiosC0Table exposes the verified entry at offset 0x18. Pointer loops implement
comparison and copying. Two register bindings and two empty constraints
capture incoming RA and place selector 0x56 in T1 for the BIOS call. All CPU
algorithmic work is C. A tied RA input/output constraint prevents GCC281 from
substituting an unrelated register for an output-only capture; the maintained
compiler is GCC272. No CPU instruction ASM or postpasses are used.

**Not exact:** stock GCC272 scores 58.930233% (164 bytes versus 172 retail).
The same pointer-loop C with GCC281 scores 55.32558%. Indexed versions were
larger and scored at most 38.372093%. Register allocation, loop scheduling,
BIOS jump-register selection and return handling differ. In particular, C
restores RA from its stack frame while retail reloads the global save slot.
Production ASM is unchanged and no matching-function credit is claimed.

```sh
tools/scripts/cc.sh proposals/St_InstallDmaHandler/candidate.c /tmp/st-install.o
python proposals/St_InstallDmaHandler/verify_behavior.py /tmp/st-install.o
```

All **384 cases** pass against the SHA-1-checked retail body: complete match,
a mismatch in each of six words, already-installed replacement, two kernel
addresses, two table addresses, optional BIOS-time modification of the last
word, three external return values and two caller return addresses. Assertions
check selector 0x56 at BIOS entry, saved RA at every external call, complete
kernel snapshots and call order, unchanged templates/table, canaries, return
PC, SP and callee-saved registers. Mutants comparing only five words, selecting
the wrong handler offset or requesting BIOS service 0x57 fail assertions.

BIOS, critical-section and cache routines are stubbed. The verifier does not
execute the patched exception handler, implement cache effects or model
interrupt timing. It assumes callees preserve the global RA save slot; arbitrary
corruption or reentrant overwriting of that slot is not equivalent to the
retail return mechanism and remains unresolved.
