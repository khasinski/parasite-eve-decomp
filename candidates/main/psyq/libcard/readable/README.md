# `_ExitCard`: readable semantic baseline

`exit_card.c` describes the operation without register bindings, empty
barriers, instruction ASM, uninitialized variables or EABI-specific calls.
It is NOT a byte match and is outside the production manifest.

The routine removes the early memory-card IRQ hook: enter the critical
section, obtain the exception handler from C0 table entry 6, replace its
three instructions at byte offset 0x70 with the three-NOP template, flush
cache, then leave the critical section. The offset and template are verified
in retail at 0x8007E514 and 0x8007E584 respectively.

The BIOS operation is documented in
[PSX-SPX, patch_uninstall_early_card_irq_handler](https://psx-spx.consoledev.net/kernelbios/#patch_uninstall_early_card_irq_handler).
This supports names for the handler and hook, rather than an invented struct
whose only member follows an unexplained padding array.

`GetC0Table()` is an explicit external API dependency. It is not implemented
by a hidden ASM macro, and this candidate does not add a retail symbol for
it. In compile probes it is linked at a clearly test-only address. The
function-pointer table is viewed as instruction-word pointers because this
routine patches the pointed-to code instead of calling it.

The candidate delegates return-address preservation to the compiler. It
therefore intentionally does not reproduce the original private global RA
slot write/read; that machine-level requirement remains part of matching.
Do not represent those operations as uninitialized C locals.

## Measurements and proposed transformations

Stock native PSX GCC272/281, O1/O2/O3, indexed/moving/unrolled copies: 18
compiled and linked probes, none exact. The initial indexed baseline (commit ebb23e4a) is 104 bytes
at O2 on both compilers (retail is 112). Smaller size is not a match score.

A nineteenth probe keeps the handler base and uses a moving source pointer,
writing `handler[CARD_IRQ_HOOK_OFFSET / sizeof(unsigned)]` before incrementing
handler. GCC272 emits the retail-style store offset 0x70, without a separate
pointer add before the loop. It produces 100 bytes and remains nonmatching.
This is a useful, behavior-preserving source transformation, not a result
obtained by forcing all register numbers.

Next steps must distinguish three independent requirements:

1. Copy loop: reproduce source/end-symbol addressing, nonempty-loop entry,
   load/store scheduling and register allocation from readable C. Compare
   the loop independently while preserving the full-function baseline.
2. BIOS call: the original sets t1=0x56 and calls through t2=0xB0. An ordinary
   C call to GetC0Table does not produce that sequence. Any proposed adapter
   must be explicit and tested; moving bindings into a macro is not removing
   them. A C wrapper alone will not make the original instructions match.
3. Return/stack convention: stock PSX ordinary calls reserve argument space
   and save ra on the stack; retail uses its private global RA slot and no
   frame. Loop or structure improvements do not establish a solution for
   this boundary. Existing EABI/pinned matches are reference evidence only.

These are open matching requirements, not a promise that changing the loop
alone will obtain 100%. No progress or debt baseline has been updated.

## Validation

The baseline and handler-relative variant each passed 1024 native test cases
with AddressSanitizer and UndefinedBehaviorSanitizer. Tests vary the handler
position and previous instruction words, check exactly three zero writes
and unchanged surrounding memory, and require the order EnterCriticalSection,
GetC0Table, FlushCache (after completed writes), ExitCriticalSection.
These are semantic tests with mocked BIOS services, not instruction-level
or hardware equivalence; they do not test the original private RA slot.

Sources, 19 compiler probes, linked objects, outputs and test harnesses:
`/tmp/psyq-exit-readable/`. `probe.py`, `shape.py`, `test.c` and
`test-relative.c` reproduce the experiments from the repository root.

## Follow-up: scheduling without pins or barriers

181 further PSX compiler probes (168 shape/type/scheduling combinations plus
13 count/bounds variants) produced no complete byte match. The retained
source uses an ordinary moving-pointer for loop and stock flags
`-fno-schedule-insns -fno-schedule-insns2`. It contains no register bindings,
empty ASM barriers, instruction ASM or explicit NOP macro.

The six-operation loop now has the retail sequence:
load word, load-hazard NOP, store at offset 0x70, advance source by four,
branch-not-equal by -5 instructions, advance destination by four in the delay
slot. Opcodes/immediates agree after ignoring register fields; five of six
instruction words still differ because the registers are different. This
is scheduling evidence, not a 24-byte match or a progress percentage.

The whole candidate is 104 bytes, with a 24-byte frame, a direct call to the
explicit GetC0Table dependency, and a pre-loop equality test. Those differ
from the 112-byte retail function. The post-test-loop experiments omit that
entry test but were not selected as the readable baseline. Changing the
counter type or direction did not solve the complete match. Using the real
end-marker address at 0x8007E590 recreates its separate address load, but
still leaves the entry test and register differences.

The retained source was compiled and linked again after formatting: its
bytes equal the selected `272-moving-3-0` probe. It passed 1024 native cases
under ASan/UBSan and 256 differential MIPS cases. The MIPS comparison models
GetC0Table as the same logical BIOS query and checks copied words, external
call order and returned SP/callee-saved registers; the SDK private global RA
slot and private stack traffic are deliberately excluded. This semantic test
is not evidence of instruction or private-state equivalence.

New experiments: `/tmp/psyq-exit-loop/search.py`, `bounds.py`, `results.json`,
`final.{o,elf,bin}`, `verify.py` and `host-test`. Production remains unchanged.
