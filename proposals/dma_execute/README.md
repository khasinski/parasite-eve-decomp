# dma_execute reconstruction

Retail entry 0x8007CEAC, 424 bytes. `candidate.c` is a first C reconstruction,
not a production match and not yet behaviorally verified against hardware or
an emulator. The manifest remains ASM.

Disassembly establishes six parameters: DMA channel, memory address, block
count, block size, control word, and an unsigned-byte interrupt-enable flag.
The last two are loaded from caller stack offsets 16 and 20; the flag uses
lbu. A volatile stack word retains two hardware readbacks.

The candidate uses the observed 16-byte DMA channel stride, with address,
block and control words at offsets 0, 4 and 8. Polling accesses control by
its direct address; configuration advances a word pointer through the channel. D_8009B348 is accessed as both a word and byte 2,
represented as a union, rather than unrelated pointer casts. D_8009B344 is a
word register pointer and D_8009B32C a byte register pointer. These are views
of the observed hardware accesses, not recovered original source type names.

Behavior reconstructed from retail:

- Poll DMA control bit 24, bounded by 0x10000 loop iterations. On exhaustion,
  print the control word and continue configuring the transfer.
- Set the channel bit in interrupt byte 2 only when the flag equals 1;
  otherwise clear it. Read back the complete interrupt word.
- Set bit channel*4+3 in the register reached through D_8009B344.
- Write the transfer address and `(blockCount << 16) | blockSize`.
- Wait for CD status bit 6, then write and read back the DMA control word.

Initial stock-toolchain objdiff scores:

| Compiler/options | Percent |
| --- | ---: |
| GCC 2.8.1, unsplit addresses (initial candidate) | 59.896225 |
| GCC 2.8.1, default split | 60.084908 |
| GCC 2.7.2, default | 61.009434 |
| GCC 2.7.2, no expensive optimizations | 61.92453 |
| GCC 2.8.1, split, no expensive optimizations | 56.75472 |

The low score needs source-shape and register-lifetime work, not promotion.
Channel and transfer sizes are assumed to be valid hardware inputs; the
candidate's signed shifts have not been generalized to arbitrary integers.

## Addressing and shared-store refinement

The current candidate reaches **78.77358%** with stock GCC 2.8.1,
`-mno-split-addresses -fno-schedule-insns`, and no pins or barriers.
Direct control-register addressing improved the original 59.896225% to
67.5%. Making the interrupt byte store common to both branches gave
69.57547%; disabling the first scheduling pass gave the current score.
The second scheduling pass alone gave 71.49056%, no-force-mem 69.669815%,
and no-expensive-optimizations 64.15094% on that shared-store version.

The remaining diff includes AT versus v0 for polling addresses, printf's
symbolic address/delay slot, interrupt-byte working registers, and ordering
of priority, block and readback operations. None of these measurements is
an exact match or behavioral validation; production remains unchanged.

## C_011 object and Darwin search

Psy-Q 4.6 `LIBCD.LIB` proves that `dma_execute` is the local function at
offset `0x948` in `C_011.OBJ`; the object exports `StCdInterrupt` and has
exactly 2800 text bytes, matching the complete retail range through this
helper. This replaces the earlier assumption that the preceding code belonged
to the renderer.

A stock GCC 2.8.1 search is running on `darwine` in
`/home/hasik/sprintf-permuter-20260909/dma-execute`, bounded to 20 minutes and
four low-priority workers. Its target is the freshly generated 424-byte retail
object. Moving the DMA priority bit into a named temporary is semantically
neutral and reduced the permuter penalty from 1820 to 985. Independently
compiling that source locally and comparing it with objdiff raises the function
match from 78.77358% to **86.68868%**. It remains a proposal until the generated
code is exact and the MMIO behavior has been reviewed.
