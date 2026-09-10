# Reconstructed LIBCD interrupt, command and wait routines

`candidate.c` compiles getintr, CD_sync, CD_ready, CD_cw, CD_vol, CD_flush, CD_initvol, CD_initintr, CD_init, CD_datasync and CD_getsector in retail
order, using one stock GCC 2.8.1 configuration:
`-mno-split-addresses -fno-expensive-optimizations`. It includes their current
candidate sources, so standalone and combined experiments share definitions.
This covers 5500 retail bytes and is still only part of BIOS_1: other driver
routines and data ownership remain outside this file. The first four functions remain assembly in production; CD_vol and CD_flush
already use matching C. The new ordinary-C CD_initvol replaces the legacy
stack/register model only in this combined reconstruction, pending CD_initintr
recovery under the same compiler configuration. The combined candidate includes their production
sources directly.

| Function | Retail bytes | Combined objdiff match |
| --- | ---: | ---: |
| getintr | 1372 | 94.33527% |
| CD_sync | 640 | 91.79375% |
| CD_ready | 712 | 89.15169% |
| CD_cw | 1036 | 92.22394% |
| CD_vol | 136 | 100% |
| CD_flush | 212 | 100% |
| CD_initvol | 240 | 100% |
| CD_initintr | 76 | 84.47369% |
| CD_init | 480 | 95.958336% |
| CD_datasync | 360 | 93.577774% |
| CD_getsector | 236 | 100% |

The common configuration retains the previous three-function combined scores.
getintr alone reaches 95.997086% without `-fno-expensive-optimizations`; its
standalone configuration is retained. Thus the combined build is not yet a
configuration that gives each function its best individual score. The first four routines use no pins, barriers or instruction ASM. Imported
CD_flush retains its existing one register pin and one empty barrier; CD_vol
has neither. No new constraints were introduced. The timeout, dispatcher
and response-copy helpers are shared inline C.

```sh
tools/scripts/cc.sh proposals/libcd_commands/candidate.c /tmp/libcd_commands.o
python proposals/CD_datasync/verify_behavior.py /tmp/libcd_commands.o
python proposals/CD_init/verify_behavior.py /tmp/libcd_commands.o
python proposals/libcd_commands/verify_init_chain.py /tmp/libcd_commands.o
python proposals/CD_initintr/verify_behavior.py /tmp/libcd_commands.o
python proposals/CD_initvol/verify_behavior.py /tmp/libcd_commands.o
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

Shared hardware-pointer declarations in `include/pe1/psyq_cd.h` resolve the
previous conflicting declarations between CD_vol, CD_flush, CD_initvol and
CD_getsector. Indexed byte ports point to volatile bytes. The word-control
address uses u32; CD_getsector qualifies its access through its existing
volatile union pointer, while CD_flush retains the original single ordinary
word store in its return delay slot. CD_initvol retains volatile byte-port
accesses through its address temporary; its separate SPU halfword views remain
unchanged. All affected production bytes retain the retail main SHA, all 191
overlays match, and 290 repository tests pass. Six-function compilation keeps
both pre-existing CD_vol/CD_flush matches at 100%.

Behavior verifiers resolve the additional production symbol aliases from the
repository linker definitions. The individual 8448/840/1008 suites and the
3072-case CD_cw/CD_sync/getintr interaction suite pass on the six-function
object. CD_flush remains modeled in that interaction suite: an exploratory
attempt to execute it encountered a Unicorn CPU exception after its return
also in unmodified retail code, so it supplies no additional behavior claim.
The exact instruction/relocation match is the evidence for the combined
CD_flush body.

The seventh function, CD_initvol, matches all 240 bytes using a real CdlATV
local and SpuRegs fields, with no pins or artificial stack frame. Its 100-case
behavior suite also passes on this combined object. Current master-volume
fields at offsets 0x1B8/0x1BA replace padding in the shared register structure.
See `../CD_initvol/README.md` for the evidence and remaining production-TU
constraint with CD_initintr.

CD_initintr is now also expressed directly in C, using the two callback globals
and two shared status words instead of a pinned synthetic page. Its 24-case
suite passes on the combined object, but it does not yet match byte-for-byte.
The canonical u32 CD_status1 declaration is shared with getintr and production
initialization; clearing just its low byte is rejected by the negative control.

CD_init is the ninth reconstructed function. Its word-control access uses the
same u32 pointee type as CD_flush, allowing its single initialization store
to occupy the original CD_cw call delay slot (94.625% → 95.958336%). It remains
assembly in production. Its existing 240-case suite passes on the combined
object. `verify_init_chain.py` adds 24 comparisons executing the actual
CD_init → CD_cw → CD_sync → getintr bodies on both sides. It covers 0/1/7
initial pending interrupts, a first NOP with or without lid-open status, and
command 1/10/12 failures or success. Command-1 errors are ignored; command
10/12 errors abort initialization. The model supplies eight-byte FIFO
responses, VSync, callback context, registration and diagnostics. It compares
MMIO/API traces, status/event words, response buffers, return and stack.
Timeouts and asynchronous callback delivery are outside this particular suite;
CD_flush is not invoked. Omitting the command-10 failure branch is rejected.

CD_datasync extends the reconstruction to ten functions and 5264 retail bytes.
It uses the shared typed event state and a local parameterized timeout helper,
with no pins or barriers. Its 93.577774% score is unchanged by combining;
all previous nine scores also remain unchanged. Its 45-case DMA-wait suite,
the 24-case initialization chain and the 3072-case real-sync/getintr suite pass
on the ten-function object. SDK BIOS_1 offsets 0x1330..0x1498 independently
prove its full 360-byte retail range, with 36 relocation-field differences.
The driver TU remains incomplete, and this function is still ASM in production.

CD_getsector now uses ordinary C with volatile MMIO, a local index pointer
and a real volatile readback local. This removes its synthetic frame type,
global SP/v0 variables, union-based address reinterpretation and all four
register pins. Stock GCC281 unsplit matches all 236 bytes, including beside
CD_getsector2 in the shared production `sector_read.c` unit.
Main retail SHA, all 191 overlay SHA checks and source/debt gates pass.
This removes four production pins (773 → 769) without adding barriers.

The collected source is not a contiguous SDK 4.6 text range: retail inserts
CD_getsector2 and CD_getsector between CD_datasync and the final setter and
dispatcher, while SDK BIOS_1 directly follows CD_datasync with its setter.
The twelve-function object is not a completed original TU;
its 5500 represented retail bytes must not be equated with SDK text size.
