# SPU settling loop with real locals

The candidate replaces the production synthetic SP pointer with two real
locals: a volatile signed counter and a volatile u32 arithmetic value. It
performs 60 iterations multiplying the value by 13. Unsigned arithmetic gives
the original MIPS wraparound a defined C meaning. The initial counter store
uses an ordinary access view so it can occupy the entry jump's delay slot;
subsequent accesses remain volatile. There are no pins or barriers.

Stock GCC281 matches all 92 retail bytes when compiled standalone. Under the
common GCC272 SPU-core configuration it is 96 bytes and scores 93.04348%:
stack restoration precedes JR instead of occupying its delay slot. Production
spu_register_write.c still requires GCC272 for its siblings and retains its
legacy loop. No new production split or matching-function credit is claimed.

```sh
tools/scripts/cc.sh proposals/_spu_Fw1ts/candidate.c /tmp/spu-wait.o
python proposals/_spu_Fw1ts/verify_behavior.py /tmp/spu-wait.o
python proposals/_spu_Fw1ts/verify_behavior.py /tmp/spu-core.o
```

The verifier checks both zero and A5 initial stack fills. It compares every
stack read and write with retail, asserts counter writes 0..60 and value writes
13^(1..61) modulo 2^32, checks canaries and verifies SP restoration. The same
comparison passes on the full combined core. Variants with 59 iterations or a
multiplier of 12 are rejected. This proves the finite loop's state accesses;
it does not establish physical settling time for the nonmatching GCC272 body.
