# LIBETC interrupt state fragment

`src/main/psyq/libapi/intr_state.c` contains the contiguous setIntr, stopIntr
and restartIntr fragment of PsyQ LIBETC's INTR.OBJ. The libapi directory follows
the existing placement of intr_callbacks.c; these routines belong to LIBETC.
This is not a claim that the whole SDK object is reconstructed.

| Project symbol | SDK text offset | Retail file offset | Bytes | Status |
| --- | --- | --- | --- | --- |
| Sys_SetIntrCallback | 0x43C | 0x648D0 | 328 | Newly matched C |
| Sys_SaveDisableIntr | 0x584 | 0x64A18 | 160 | Existing C, moved into this TU |
| Sys_RestoreIntr | 0x624 | 0x64AB8 | 120 | Newly matched C |

The new contribution is 448 bytes. All 608 bytes of the combined TU match
retail after linking with native stock GCC 2.8.1, GNU assembly and
`-mno-split-addresses`. No new register pins, barriers, instruction assembly,
compiler changes or assembly rewrites were added. stopIntr retains its existing
I_MASK pointer pin. The one-iteration scope around root-counter calls preserves
GCC's register allocation; it does not implement an interrupt polling loop.

setIntr returns the previous handler. If it changes while callbacks are active,
it masks interrupts, updates the handler and enabled masks, adjusts automatic
PAD/root-counter acknowledgement for channels 0/4/5/6, and restores the updated
hardware mask. restartIntr restores the saved entry hook, interrupt mask and DMA
control, marks the state active, and leaves the critical section. It returns null
if already active, otherwise the state pointer.

## Boundary and type evidence

PsyQ 4.6 LIBETC.LIB's INTR.OBJ has a 1728-byte text section. Its text corresponds
to retail beginning at file offset 0x64494; the memory-clear helper follows
restartIntr at SDK offset 0x69C. Comparing the two new routines against that SDK
object finds no differences outside its relocation fields: 70 identical and
12 relocated instruction words for setIntr; 21 identical and 9 relocated words
for restartIntr. This supports combining the contiguous fragment instead of
creating two more small files.

To inspect an independently obtained SDK library, extract INTR.OBJ with `psyk
extract`, then use `psyk list -r -c INTR.OBJ`. Patch type 74 covers the low 26 bits
of jump instructions; types 82/84 cover the low 16-bit relocation field. The
comparison masks only those fields at the SDK's declared patch offsets.

The shared InterruptCallbackState records active/in-callback flags, eleven
handlers, enabled/saved interrupt masks, saved DMA control, a twelve-word BIOS
jump buffer and a 1024-word IRQ stack. SDK SETJMP.H defines the twelve jump-buffer
words (PC, SP, FP, S0–S7, GP). The initializer's 0x41A-word clear and stack/entry
addresses support the total 0x1068-byte layout. Compile-time assertions cover
its field offsets. Existing halfword storage declarations remain ABI-compatible.

## Verification

```sh
.venv/bin/python proposals/intr_state/verify_bytes.py
make -j4 verify-clean
```

The byte verifier independently compiles and links the fragment at its retail
address and compares every function byte, including resolved relocations.
A separate experimental Unicorn behavioral test also faults on the retail
implementation and is not included as validation evidence.

`make -j4 verify-clean` passes all 292 repository tests and the full executable
SHA-1 check: `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
