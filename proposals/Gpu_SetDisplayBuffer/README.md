# Gpu_SetDisplayBuffer — LIBGPU queue drain

Integrated in `src/main/main/Gpu_SetDisplayBuffer.c`: all **608 linked bytes**
match the retail range at `0x80076EE4`. The existing symbol name is retained;
the function drains the GPU command queue (`_exeque`), not a display-buffer setter.

The source uses stock native GCC 2.7.2 and stock GNU as 2.7. GNU as 2.7
schedules the saved interrupt-mask store into the following branch's delay slot.
No compiler/assembler patches, CPU instruction assembly, or binary rewriting
are involved. Six register pins and five empty barriers are recorded in debt.

Recovered shared types describe a 96-byte queue entry (callback, two argument
words, 84-byte packet payload) and the pending DrawSync callback flag at offset
8 of `GpuDebugState`. The three separate volatile tail reads are retained.

Validation:

- `tools/tests/test_formatter_gpu_drain.py` checks the complete linked text
  against its fixed retail SHA-256, without requiring retail assets in CI.
- `verify_behavior.py` compares 960 finite cases with the verified retail EXE:
  empty/nonempty queues, index wraparound, immediate DMA-busy returns, DMA
  starts during draining, delayed GPU readiness, callback presence and pending
  state. It compares callback traces, queue memory, state and return values,
  and also asserts expected queue consumption and callback behavior.
- The complete rebuilt `main.exe` retains SHA-1
  `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.

Run the behavioral comparison from the repository root:

```sh
.venv/bin/python proposals/Gpu_SetDisplayBuffer/verify_behavior.py \
  build/USA/src/main/main/Gpu_SetDisplayBuffer.c.o
```
