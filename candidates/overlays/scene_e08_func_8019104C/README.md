# scene_e08 func_8019104C (0x2064, 0x2FC bytes): ring sprites draw

Parked at 10 real diffs (sc.sh), all register naming.

Files: `Scene_DrawRingSprites_8019104C.c` (instance for src/overlays/scene_e08/)
and `scene_ring_sprites.h` (goes to include/pe1/).

Logic, frame layout, stores and calls all match. Retail keeps the state
pointer in two pseudos: the incoming `$a2` copy in s1 is used by the
`ticks < 12` flash-sprite block, and a second copy `s2 = s1` (made in the
delay slot of `jal func_800C2EAC`) is used by everything after that block;
s1 is then reused for `&matrix`. Stock cse always merges the copy
(`ring = state` in every combination of uses in the condition / body / tail
was tried; cse1 or cse2 canonicalises one away and flow deletes the copy),
so the build uses one register (s2) throughout: prologue save order of s1/s2
and the base register of the ten state loads differ.

Not tried: inline helper functions, permuter.
Also note `(ticks % 12 & 0xFE) - 0x5C` (not `+ 0xA4`) gives retail's addiu.
