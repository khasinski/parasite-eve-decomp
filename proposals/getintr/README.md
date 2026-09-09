# LIBCD getintr

`candidate.c` reconstructs the complete 1372-byte retail function at
`0x8007AAB4` in C. Stock GCC 2.8.1 with `-mno-split-addresses` scores
95.997086% in objdiff; it has no register pins, barriers or instruction ASM.
It is not yet a byte-exact production replacement.

The routine stabilizes the interrupt register, drains up to eight response
bytes, acknowledges the controller, updates status and lid-open count, and
routes acknowledgement/completion/ready/end/error events into the three
shared result buffers. Copies preserve the original eight-byte countdown.
`CdInterruptEvents` supplies the shared, volatile event fields.

Reconstruction exposed a conflicting declaration of `D_8009AFC4`:
`getintr` loads and stores a full word (retail `0x8007AC08` and
`0x8007AC5C`). LIBCD's BIOS_1.OBJ places `CD_status` at data offset 0x14,
followed by `CD_status1` at 0x18. The shared declaration is now `u32`;
`CdStatus` and command retry explicitly read the low byte, retaining their
original byte loads. This also removes the contradictory `g_CdStatus`
source declaration. Signedness of the SDK's original declaration is not
established by its symbol table; the unsigned storage type accommodates
all observed bitwise status operations.

Validation (run from repository root):

```sh
tools/scripts/cc.sh proposals/getintr/candidate.c /tmp/getintr.o
tools/objdiff/objdiff-cli diff \
  -1 expected/build/USA/asm/USA/main/psyq/libcd/getintr.s.o \
  -2 /tmp/getintr.o -o /tmp/getintr.json
python proposals/getintr/verify_behavior.py /tmp/getintr.o
```

The Python verifier requires Unicorn and pyelftools. Its 8448 finite cases
compare retail MIPS against the linked candidate, including all interrupt
values 0–7, stable/changing interrupt samples, response lengths 0/1/2/8,
status bits, command flags, diagnostic modes and prior status words.
It compares return values, both status words, lid-open count, event bytes,
all three result buffers, every FIFO/MMIO access and initialized diagnostic
arguments. Diagnostics are stubbed and hardware is modeled; this is not an
exhaustive proof of physical controller timing. Changing error mask 0x1D
to 0x19 is rejected as a negative control.

Remaining differences include an extra status mask in retail, address-load
scheduling around diagnostic calls, and event/address register allocation.
GCC 2.7.2 scores 90.399414%; GCC 2.8.1 split addresses 84.705536%; disabling
first/second scheduling passes scores 92.63265%/92.64723%. None improves
the selected unconstrained source. Production remains the retail assembly.
