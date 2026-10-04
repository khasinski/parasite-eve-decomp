# scene_e08 func_80191E78 (0x2E90, 0x350 bytes): particle slots draw

Parked at 78 real diffs (sc.sh); the two loops match apart from register
choice.

Files: `Scene_DrawParticleSlots_80191E78.c` (instance), `scene_particle_slot_draw.h`
(include/pe1/), and `scene_particle_slots.h.patch` (offset[] becomes
GteShortVector so the slot offset can be struct-copied and passed to
func_800CEE20).

Remaining:
1. Prologue block: retail emits the nine D_800F3368 parameter stores after
   the three stack vector copies and the D_800E2850[D_800E11EA] load; stock
   sched1 hoists them above the copies (struct-field or scalar-global form,
   stores written before or after the copies in source: same result).
2. Second loop: retail hoists both `&D_800E1204` (s7) and the constant 4
   (s6) out of the loop and keeps a separate `li v1, 4` for the
   parameter02 store. Without help only the table base is hoisted; with
   `special = 4;` inside the loop both are hoisted but s6/s7 are swapped and
   the 0x40/4 constant sharing differs.
`offset.z <<= 1` gives retail's lhu/sll (the `*= 2` form gives lh).
