# Gte_RotateVec

Production matches **352/352 linked bytes** at `0x80078934`, the complete
original text range. Native GCC 2.7.2 and unmodified MASPSX use default flags.
This LIBGTE entry retains its existing project name and source category.

Loads the matrix into the GTE, then splits each signed 32-bit vector
component into signed high and low parts at bit 15. The high pass uses MVMVA
with SF=0, the low pass uses SF=1. MAC1/2/3 are combined as a wrapping
`(high << 3) + low` and stored through the returned output pointer. Only
the three vector components are written. Retail's truncation of the signed
chunks to GTE IR registers, including INT_MIN, is retained.

All CPU operations are C. Each COP2 transfer and command has an individual
single-instruction macro. Eight pins and ten empty barriers are counted in
debt. Six barriers define architectural `$0` for the original relative
branches; three prevent speculative shifts in the split branches and one
retains the output pointer. Nineteen redundant barriers were removed.

`tools/tests/test_scale_long_vector.py` verifies every linked text byte against
the retail SHA-256. The full main executable retains its retail SHA-1.
`verify_behavior.py` passes **3,428 cases** against retail and a CPU-side
integer model: signed boundary values and random vectors, MAC overflow,
input/output overlaps, preserved memory, pointer return and stack. It also
checks COP2 transfer delays, loaded matrix words and both IR input triples.
GTE MAC results are scripted: GTE arithmetic and pipeline timing beyond the
transfer delays are not emulated.

```sh
tools/scripts/cc.sh src/main/gte/Gte_RotateVec.c /tmp/rotate-vector.o
.venv/bin/python proposals/Gte_RotateVec/verify_behavior.py /tmp/rotate-vector.o
```
