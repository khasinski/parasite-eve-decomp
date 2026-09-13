# Four Psy-Q controller and CD callbacks

| Link symbol | SDK object / private label | Retail address | Text bytes |
| --- | --- | --- | ---: |
| `LIBPAD_PADPORTD_text_388` | LIBPAD/PADPORTD.OBJ / text_388 | 0x8008486C | 636 |
| `LIBDS_DSSYS_1_text_4A4` | LIBDS/DSSYS_1.OBJ / text_4A4 | 0x8007FE24 | 720 |
| `LIBDS_DSREADY_text_FC` | LIBDS/DSREADY.OBJ / text_FC | 0x80081E70 | 732 |
| `LIBPAD_PADIF_text_26C` | LIBPAD/PADIF.OBJ / text_26C | 0x80084168 | 892 |

All 2,980 linked text bytes match retail, including relocations. The regression
links each complete function at its retail address and verifies its full size
and SHA-256. `provenance.json` separately records whole-function matches against
Psy-Q 4.3 signatures, source hashes, SDK offsets and relocation wildcard counts.
Private `text_*` labels identify SDK locations, not recovered original C names.
The output-selection function also matches PADPORTM/text_4EC, but the complete
linked object matches PADPORTD; its stronger attribution supplies text_388.

The PADPORTD function selects controller output channels using five-byte
capability records and a shared budget. The PADIF function transfers controller
bytes while servicing the other port and its child objects. Both reuse the
existing 240-byte `CardObj` and `PadCapabilityRecord` layouts. The PADPORTD
callback assignment now declares its actual `CardObj *` argument.

DSSYS_1 handles VSync-driven timeout countdowns and CD command states. DSREADY
handles sector delivery, saved callbacks, and read retries. The latter confirms
four previously provisional fields in `DsAsyncReadState`: `nextSector`,
`lastDeliveredSector`, `retryPending`, and `retriesRemaining`. The existing
initializer uses those same names; size and retry-field offsets are asserted.
The event handler takes a word and masks it internally; the registration site
explicitly casts to the existing SDK byte-event callback typedef.

All four compile with native, unmodified GCC 2.7.2 and the existing GNU assembler
route. There are no toolchain edits or instruction-bearing inline ASM. Two
constraint-removal passes leave 15 pins, 12 empty constraints and 35 gotos,
recorded in the debt baseline. One pair of empty constraints orders the DSREADY
prologue saves; its temporary output is used only by the next empty constraint
and is then overwritten by the incoming event. Integer address sums preserve
operand order in controller table accesses. Existing constrained-unit and
legacy asm-body counts remain unchanged.

The four ranges were already assigned to Psy-Q before conversion. Their SDK
grid identities and the category denominator remain stable when ASM becomes C.
These are individual linked units, not a claim to have reconstructed complete
SDK translation units. Byte equality does not establish independent hardware
execution tests.

Validation: `make -j8 check` reproduces the complete retail executable SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`. `make ci` passes all 327 tests,
including the four complete linked-byte regressions and the shared layout checks.

The full report audit passes with 2,406,236 credited bytes and 10,525 functions.
Psy-Q advances from 389/516 and 77.49% code to 393/516 and 81.30%. All four
SDK grid units report 100% matched code. After correcting the output callback
name to PADPORTD, the main target objects were regenerated and both whole-project
progress audits rerun; the verified overlay target objects remained unchanged.
