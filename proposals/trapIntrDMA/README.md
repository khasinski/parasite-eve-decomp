# trapIntrDMA reconstruction

New C reconstruction of the 384-byte retail dispatcher at 0x80074520.
It snapshots the seven pending channel flags, acknowledges each flagged
channel before invoking its current callback, and repeats until the current
status register has no pending channel flags. Callbacks can change later
slots, raise further flags or replace the status-register pointer; subsequent
accesses use the updated state.

The error path checks the high status byte and bit 15, prints the status,
and prints seven channel address registers. DmaChannelRegisters gives those
records their verified 16-byte stride. The channel-array base is reloaded for
each diagnostic call, matching the original behavior if printf changes it.
The source uses shared callback/pointer declarations and volatile views for
status accesses. There are no pins, barriers, instruction ASM or postpasses.

**Not exact:** stock GCC281 unsplit gives 95.25% (372 candidate bytes versus
384 retail). GCC272 gives 95.052086%; default GCC281 gives 85.03125%.
Explicit base-pointer/do-loop and indexed-loop variants did not improve the
maintained candidate. A saved base register, pointer induction and stack
layout still differ. No production ASM replacement or matching credit is
claimed.

```sh
tools/scripts/cc.sh proposals/trapIntrDMA/candidate.c /tmp/trap-dma.o
python proposals/trapIntrDMA/verify_behavior.py /tmp/trap-dma.o
```

All **4,608 cases** pass against the SHA-1-checked retail body and a separate
Python dispatch model: all 128 pending masks, four callback-presence masks,
three error/master states and three callback-mutation modes. Tests cover
acknowledgement order/values, callback snapshots, new IRQs, callback changes,
status-pointer replacement, diagnostic pointer/argument reloads, final global
state, channel-record preservation, callee-saved registers, SP and canaries.
Mutants using the wrong acknowledgement bit, omitting the outer rescan or
using a 12-byte diagnostic stride fail assertions.

The interrupt-register model implements writable bits, write-one-to-clear
pending flags and master-flag recomputation. These rules were cross-checked
against [DuckStation's DMA register implementation](https://github.com/stenzek/duckstation/blob/master/src/core/dma.cpp),
including its DICR write/reset masks and UpdateMasterFlag. The tested CPU
bodies are executed unchanged. A code hook services register reads and
applies modeled acknowledgement effects after the original SW; it avoids
Unicorn's problematic memory-write hooks on branch-delay stores.

Callbacks and printf are stubbed. An alternate synthetic status port and
alternate channel table exercise pointer reloads. A deliberately stale master
bit exercises the high-byte diagnostic branch. This is finite dispatch/MMIO
model coverage, not proof of DMA transfers, interrupt delivery/timing,
concurrent memory ordering or complete hardware behavior.
