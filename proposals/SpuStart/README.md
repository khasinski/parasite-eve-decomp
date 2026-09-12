# SpuStart with named storage and callback

The source reconstructs startup using `_spu_isCalled`, `_spu_EVdma` and
`_spu_FiDMA`. It replaces the former fabricated data/callback page
structures, absolute page addresses and four register pins. There are no
barriers or instruction asm in this candidate.

Only an initially zero called flag enters startup. The function sets it before
entering the critical section, installs the DMA interrupt callback, opens event
0xF0000009 with specification 0x20 and mode 0x2000, stores the returned handle,
enables that handle and leaves the critical section. It does not check failure
returns, matching retail. API return types follow PsyQ LIBAPI.H: int critical
entry, signed 32-bit OpenEvent/EnableEvent returns and a signed 32-bit event
callback return (the callback argument here is null).

Production matches all **112/112 linked retail bytes** using native GCC 2.7.2
and unmodified GNU as. The assembler schedules symbolic stores and the DMA
callback address into the retail call delay slots. There are no pins,
barriers, fabricated pages or instruction asm. The exact-byte regression in
`tools/tests/test_spu_lifecycle.py` checks relocations at the retail address.

```sh
tools/scripts/cc.sh proposals/SpuStart/candidate.c /tmp/spu-start.o
tools/objdiff/objdiff-cli diff \
  -1 expected/build/USA/src/main/psyq/libspu/SpuStart.c.o \
  -2 /tmp/spu-start.o -o /tmp/spu-start.json
python proposals/SpuStart/verify_behavior.py /tmp/spu-start.o
```

The retail SHA-1-checked Unicorn comparison passes 48 cases: four called-flag
values, two callback-storage patterns, two previous event handles and three
OpenEvent return values (including zero and -1). It compares relevant global
reads/writes, API order and arguments, callback identity, event-handle
propagation and SP restoration. External APIs are modeled with NOP entry stubs
and nonzero return values; their bodies and asynchronous hardware behavior are
not executed. Mutants installing a null DMA callback, changing the event
specification, or enabling handle zero are rejected by assertions. A separate
guard mutant encountered a Unicorn CPU exception and is not counted as a
semantic negative test.
