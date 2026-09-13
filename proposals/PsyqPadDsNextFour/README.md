# Four more Psy-Q LIBPAD / LIBDS functions

| Link symbol | SDK object / label | Retail address | Text bytes |
| --- | --- | --- | ---: |
| `LIBDS_DSSYS_1_text_A9C` | LIBDS/DSSYS_1.OBJ / text_A9C | 0x80080404 | 648 |
| `LIBPAD_PADSEQD_text_108` | LIBPAD/PADSEQD.OBJ / text_108 | 0x80084C4C | 612 |
| `LIBDS_DSSYS_2_text_3D0` | LIBDS/DSSYS_2.OBJ / text_3D0 | 0x8007E964 | 548 |
| `_padInitSioMode` | LIBPAD/PADMAIN.OBJ / _padInitSioMode | 0x80082E00 | 532 |

All 2,340 linked text bytes match retail, including relocations. The regression
test links each complete C function at its original address and checks its size
and SHA-256. `provenance.json` independently records full-function comparisons
against Psy-Q 4.3 LIBDS and 4.2 LIBPAD signatures, excluding only their relocation
wildcards. Private `text_*` labels do not claim recovered original C names.

The functions implement CD read-state event handling, controller response-state
advancement, CD queue completion/retry dispatch, and controller SIO setup.
Existing shared CD and controller structures replace raw field offsets. The
response handler proves that `CardObj.fn_18` returns an integer state increment;
its type and all three callback assignments now agree without a function-pointer
cast in the caller. Other provisional field names remain provisional.

Compilation uses native stock GCC 2.7.2 with the existing GNU assembler route.
No compiler/assembler changes or instruction-bearing inline ASM are used.
After two constraint-removal passes, the added debt is 13 register pins,
6 empty constraints, and 13 gotos. The SIO table-address pin to register 1
preserves the original 32-byte frame without changing the compiler ABI.
Integer pointer sums in two queue/table paths preserve operand ordering.
Existing instruction-constrained unit and legacy asm-body counts are unchanged.

Report ownership already covers these four ranges before their conversion:
changing ASM to C does not change the Psy-Q category denominator or grid identity.
These remain individual linked units, not reconstructed complete SDK objects.

Validation: `make -j8 check` reproduces the entire retail executable SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`. `make ci` passes all 326 tests,
including the four complete linked-byte regressions. These checks do not claim
independent hardware execution.

`make report` passes its semantic audit with 2,403,256 credited bytes and
10,521 functions overall. Psy-Q advances from 385/516 functions and 74.50% code
to 389/516 and 77.49%, with the denominator unchanged. All four SDK grid units
report 100% matched code.
