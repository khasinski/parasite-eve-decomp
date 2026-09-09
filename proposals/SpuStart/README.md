# SpuStart with named storage and callback

The candidate reconstructs startup using `_spu_isCalled`, `_spu_EVdma` and
`_spu_FiDMA`. It removes the production source's fabricated data/callback page
structures, absolute page addresses and four register pins. There are no
barriers or instruction asm in this candidate.

Only an initially zero called flag enters startup. The function sets it before
entering the critical section, installs the DMA interrupt callback, opens event
0xF0000009 with specification 0x20 and mode 0x2000, stores the returned handle,
enables that handle and leaves the critical section. It does not check failure
returns, matching retail. API return types follow PsyQ LIBAPI.H: int critical
entry, signed 32-bit OpenEvent/EnableEvent returns and a signed 32-bit event
callback return (the callback argument here is null).

Stock GCC281 produces 108 bytes against retail's 112 bytes and scores **91.25%**
against `expected/build/USA/src/main/psyq/libspu/SpuStart.c.o`. The called flag's
address is shared across its load and store, the event store uses v0 rather
than AT, and the stack restoration occupies the return delay slot. The
production object contains literal addresses from its synthetic page source;
comparison against that object instead gives 91.07143%, so it is not the score
reported above. GCC272's source has additional NOPs around the symbolic stores
and callback address setup. GCC281 unsplit addressing and call-used-AT variants
did not improve the initial production-object comparison.

This is a proposal, not a production replacement or a newly matched function.
It does not establish the original SDK translation-unit boundaries.

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
