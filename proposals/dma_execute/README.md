# dma_execute reconstruction

Retail entry 0x8007CEAC, 424 bytes. `candidate.c` is a first C reconstruction,
not a production match and not yet behaviorally verified against hardware or
an emulator. The manifest remains ASM.

Disassembly establishes six parameters: DMA channel, memory address, block
count, block size, control word, and an unsigned-byte interrupt-enable flag.
The last two are loaded from caller stack offsets 16 and 20; the flag uses
lbu. A volatile stack word retains two hardware readbacks.

The candidate models a 16-byte DMA channel with address, block and control
words at offsets 0, 4 and 8. D_8009B348 is accessed as both a word and byte 2,
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

Current stock-toolchain objdiff scores:

| Compiler/options | Percent |
| --- | ---: |
| GCC 2.8.1, unsplit addresses (candidate) | 59.896225 |
| GCC 2.8.1, default split | 60.084908 |
| GCC 2.7.2, default | 61.009434 |
| GCC 2.7.2, no expensive optimizations | 61.92453 |
| GCC 2.8.1, split, no expensive optimizations | 56.75472 |

The low score needs source-shape and register-lifetime work, not promotion.
Channel and transfer sizes are assumed to be valid hardware inputs; the
candidate's signed shifts have not been generalized to arbitrary integers.
