# Gpu_SwapDisplayBuffers — LIBGPU command submission

Integrated in `src/main/main/Gpu_SwapDisplayBuffers.c`: all **688 linked bytes**
at `0x80076C34` match retail. The existing project symbol name is retained;
the function submits GPU commands rather than swapping display buffers.

It waits for a free ring slot with timeout checks and queue draining. With
queueing disabled, or an idle empty queue without a DrawSync callback, it
waits for GPU readiness and calls the command directly. Otherwise it copies
an optional packet, stores the callback and two arguments in a 96-byte
`GpuQueueEntry`, advances the head, restores the interrupt mask and drains
the queue. A nonzero byte count selects the private packet buffer, even when
signed division by four yields no words to copy; a zero count retains the
original pointer. The source preserves these retail edge cases.

The match uses stock native GCC 2.8.1 (`-mno-split-addresses`) and the existing
GNU assembler path. One register pin and six empty barriers are included in
debt. There is no CPU instruction assembly or compiler/assembler patch.

Validation:

- `tools/tests/test_gpu_submission_ds_init.py` guards all linked bytes with
  a fixed retail digest, without requiring retail assets in CI.
- `verify_behavior.py` passes 3,072 retail-oracle cases: empty/nonempty/full
  queues, index wraparound, timeout failure and eventual space becoming
  available, DMA-busy state, delayed GPU readiness, DrawSync callback presence,
  direct execution, packet copying (including partial and negative lengths),
  pointer-only submission, and an empty/nonempty result after draining.
  It compares state snapshots at external calls, queue memory, call order,
  arguments, return values and SP, and independently checks packet contents,
  untouched queue bytes, publication, interrupt-mask restoration and source
  preservation. External queue-drain/timer/interrupt APIs are modeled;
  this finite check does not emulate the whole GPU subsystem.
- The complete `main.exe` retains retail SHA-1
  `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.

Run from the repository root:

```sh
.venv/bin/python proposals/Gpu_SwapDisplayBuffers/verify_behavior.py \
  build/USA/src/main/main/Gpu_SwapDisplayBuffers.c.o
```
