# CD_getsector2 ordinary-C reconstruction

This candidate reconstructs the complete 256-byte low-level LIBCD sector
reader with the SDK pointer/count signature and typed volatile MMIO accesses.
It contains no register pins, barriers or instruction assembly. Stock GCC
2.8.1 with `-mno-split-addresses` emits the retail size and scores 98.828125%
in objdiff. After resolving the production symbol aliases, only byte `0x98`
differs: the immediate of one branch target.

The target loads the status pointer, executes its load-hazard `nop` once, then
branches back to the byte load at offset `0x8C`. MASPSX places that generated
hazard `nop` at the loop label, so the ordinary-C object branches to `0x88`.
All other linked instruction bytes match. The production source retains its
constrained status loop until the stock pipeline can express this label
placement without CPU instruction assembly.

`CD_getsector2` and `CD_getsector` are built together in production
`src/main/psyq/libcd/sector_read.c`, matching the complete 492-byte retail
range and recording their shared Psy-Q `libcd/bios.c` provenance.

```sh
tools/scripts/cc.sh proposals/CD_getsector2/candidate.c /tmp/CD_getsector2.o
tools/objdiff/objdiff-cli diff \
  -1 expected/build/USA/src/main/psyq/libcd/sector_read.c.o \
  -2 /tmp/CD_getsector2.o -o /tmp/CD_getsector2.json CD_getsector2
```
