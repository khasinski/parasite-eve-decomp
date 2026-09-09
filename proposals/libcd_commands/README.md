# Reconstructed LIBCD interrupt, command and wait routines

`candidate.c` compiles getintr, CD_sync, CD_ready and CD_cw together in retail
order, using one stock GCC 2.8.1 configuration:
`-mno-split-addresses -fno-expensive-optimizations`. It includes their current
candidate sources, so standalone and combined experiments share definitions.
This covers 3760 retail bytes and is still only part of BIOS_1: other driver
routines and data ownership remain outside this file. These four functions
remain assembly in production.

| Function | Retail bytes | Combined objdiff match |
| --- | ---: | ---: |
| getintr | 1372 | 94.33527% |
| CD_sync | 640 | 91.79375% |
| CD_ready | 712 | 89.15169% |
| CD_cw | 1036 | 92.22394% |

The common configuration retains the previous three-function combined scores.
getintr alone reaches 95.997086% without `-fno-expensive-optimizations`; its
standalone configuration is retained. Thus the combined build is not yet a
configuration that gives each function its best individual score. Neither
configuration uses pins, barriers or instruction ASM. The timeout, dispatcher
and response-copy helpers are shared inline C.

```sh
tools/scripts/cc.sh proposals/libcd_commands/candidate.c /tmp/libcd_commands.o
python proposals/getintr/verify_behavior.py /tmp/libcd_commands.o
python proposals/CD_sync/verify_behavior.py /tmp/libcd_commands.o
python proposals/CD_ready/verify_behavior.py /tmp/libcd_commands.o
python proposals/CD_cw/verify_behavior.py /tmp/libcd_commands.o
python proposals/CD_cw/verify_behavior.py /tmp/libcd_commands.o --real-sync
python proposals/CD_cw/verify_behavior.py /tmp/libcd_commands.o --real-intr
```

The four individual suites run 8448, 840, 1008 and 3072 cases. Candidate text
is linked into a separate emulated RAM region (0x80150000), and entry points
and modeled internal calls are resolved from ELF symbols. This avoids large
combined objects overlapping the original external-function hook addresses.

`--real-sync` runs another 3072 cases in which CD_cw calls the real CD_sync:
retail runs the original functions and the candidate runs the C bodies. The
initial error status is consumed before issuing a command; CD_cw ignores that
synchronization result. The verifier asserts that the two initial VSync calls
and callback-context check occur before command issue. Omitting the CD_sync
call is rejected as a negative control.

`--real-intr` also executes getintr, for another 3072 cases. The model supplies
controller index/status bytes, an eight-byte response FIFO and ready/complete
or error interrupts. The original/C routines themselves acknowledge interrupts,
consume the FIFO, update status words/event bytes, copy response buffers and
dispatch callbacks. Redirecting the ready-response copy into the sync buffer
is rejected. This mode covers successful command completion, disk error,
callback combinations, debug levels, null parameters/results, both timeout
mechanisms and their boundaries.

These are finite comparisons, not physical-hardware or whole-driver proofs.
VSync, callback context, callbacks, diagnostics and CD_flush remain modeled;
getintr is also modeled unless `--real-intr` is selected. The SDK verifier in
`../CD_cw/verify_sdk.py` independently establishes the three exported routines'
contiguous ranges; it does not assert whole-BIOS_1 equivalence or an exact SDK
match for the private getintr routine.
