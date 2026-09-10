# LIBETC interrupt state fragment

`src/main/psyq/libapi/intr_state.c` contains the contiguous trapIntr, setIntr,
stopIntr and restartIntr fragment of PsyQ LIBETC's INTR.OBJ. The libapi directory
follows the existing placement of intr_callbacks.c; these routines belong to
LIBETC. This is not a claim that the whole SDK object is reconstructed.

| Project symbol | SDK text offset | Retail file offset | Bytes |
| --- | --- | --- | --- |
| Render_InitSceneGeom (trapIntr) | 0x26C | 0x64700 | 464 |
| Sys_SetIntrCallback (setIntr) | 0x43C | 0x648D0 | 328 |
| Sys_SaveDisableIntr (stopIntr) | 0x584 | 0x64A18 | 160 |
| Sys_RestoreIntr (restartIntr) | 0x624 | 0x64AB8 | 120 |

All 1072 bytes of the combined TU match retail after linking with native stock
GCC 2.8.1, GNU assembly and `-mno-split-addresses`. The dispatcher adds 464 bytes
to the previously matched 608-byte fragment. stopIntr retains its existing I_MASK
pointer pin. The other three functions have no register pins, compiler barriers
or instruction assembly.

The legacy symbol Render_InitSceneGeom is actually the interrupt dispatcher.
It marks callback execution active, intersects pending, enabled and unmasked IRQs,
acknowledges each bit before calling its registered handler, and scans again for
new pending IRQs. An unsupported interrupt increments a consecutive-occurrence
counter. When its previous value exceeds 0x800, the dispatcher prints the status
and mask, resets that counter and clears I_STAT. A quiet invocation resets the
counter immediately. It clears the in-callback flag before ReturnFromException.

setIntr returns the previous handler. If it changes while callbacks are active,
it masks interrupts, updates the handler and enabled masks, adjusts automatic
PAD/root-counter acknowledgement for channels 0/4/5/6, and restores the updated
hardware mask. restartIntr restores the saved entry hook, interrupt mask and DMA
control, marks the state active, and leaves the critical section. It returns null
if already active, otherwise the state pointer.

## Source constraints

The dispatcher's two equivalent diagnostic calls are intentional. GCC merges
that conditional after scheduling the format address before the two register
reads. Replacing it with one direct printf call gives 460 bytes instead of the
retail 464: the address moves after the reads and its final instruction fills
the call delay slot. The retained branch does not appear in the binary and does
not add null-pointer handling; both source paths perform the same operations.
This is a C source-shape constraint found with decomp-permuter, not a claim that
the original SDK source used this condition. The final candidate was simplified
and independently recompiled and linked locally.

The one-iteration scope around setIntr's root-counter calls likewise preserves
GCC's register allocation; it does not implement an interrupt polling loop.
No compiler changes, assembly rewrites or new ASM constraints are used.

## Boundary and type evidence

PsyQ 4.6 LIBETC.LIB's INTR.OBJ has a 1728-byte text section. Its text corresponds
to retail beginning at file offset 0x64494; the memory-clear helper follows
restartIntr at SDK offset 0x69C. Comparing against that SDK object finds no
differences outside its declared relocation fields:

- trapIntr: 77 identical and 39 relocated instruction words;
- setIntr: 70 identical and 12 relocated instruction words;
- restartIntr: 21 identical and 9 relocated instruction words.

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
An earlier experimental Unicorn test faulted on the retail implementation and
is not counted as validation evidence. The clean acceptance build runs 292
repository tests and verifies the complete executable against retail SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
