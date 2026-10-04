# Akao_StepSampleLoader (main 0x7F0D0, 0x800 bytes): parked at lev 8

Typed draft of the sequencer note step. To build it, copy `note_step.h` to
include/pe1/akao/ and the .c to src/main/main/ (plain -G0 file with the
`--expand-div` maspsx marker, like the other akao units). There is no
flags-word problem here: every absolute access is a plain RMW at -G0.

lev 8, equal size (512 words). What is left is register choice in two
multiplies and one store order:
1. Drum-kit volume: `expression_value = (volume * (sample[2] +
   (sample[3] << 8))) << 2`. Retail takes the product in a1, the volume
   register (a tie); mine uses v1 or a3, depending on which variable holds
   the volume.
2. Pitch LFO depth: retail `mflo a3; srl v1,a3,7; lhu v0,selector`, then
   stores counter before phase. Mine takes the product in a1, puts the
   selector load before the srl, and stores phase before counter.

What brought it from lev 117 to 8 (all plain C, keep these):
- Command loop as `do { AkaoCommandHandler handler; ... } while (op >= 0xA0
  && op != 0xA0)`. The block-scope declaration stops jump.c from copying
  the exit test (no loop rotation), and calling through the 0xFC table in
  its own branch puts `move a0,s0` where retail has it. The extended
  command byte reuses the `period` variable (retail keeps it in s2).
- The sustain test is bit 0x200000 (AKAO_TRACK_FLAG_SUSTAIN in track.h is
  1 << 20, which does not match this use).
- The note index reuses `opcode` (`opcode /= 11`), and the LFO flags read
  reuses it too. Retail keeps all three in s1.
- `length = track->field_56 = D_8009B8DC[opcode % 11];` (u16) gives the
  load-then-copy.
- The drum sample pointer is computed in two steps: `sample =
  track->branch_target; sample += note % 12 * 6;`.
- Instrument selection is a static inline helper with early returns. The
  id goes through a `u8` parameter into a `u16` local, which gives
  retail's `andi 0xff`.
- Portamento block: the order `vibrato_duration`, then `vibrato_delta =
  expression + note - counter - delta`, then `field_E2`, then `note = counter
  + delta`. The 16-bit stores narrow the arithmetic, so retail reloads
  with lhu.
- LFO restarts: read the table entry before the counter and phase stores.
- Vibrato slide: keep the step count in a u16 local and store field_7A
  from it after clearing vibrato_delta.
- The drum volume read goes into a multi-set function-scope variable
  (`value`, shared with the LFO depth product), so it is scheduled first.
  A single-set temporary gets launch priority and is placed late.

The permuter (darwine scratch/a5akao*, base.c reformatted through pycparser
with AkaoTrackUpdateSlot's pad written as 0x118) found nothing at lev < 8.
