# scene_e20 func_8018F028 — matched

Promoted to `src/overlays/scene_e20/RoomEffect_FlareParticle_8018F028.c`.
The 1832-byte function has score 0 and matches retail byte for byte under
stock native GCC 2.7.2 and stock MASPSX. The complete `scene_e20.bin` also
matches SHA-1 `cb847aba4aa80d900b766aa03ca9e306edc140e8`.

Two register pins resolve the former near-miss: the first palette comparison's
constant and the streak comparison's palette index independently use v1.
No empty barriers or CPU instruction ASM were added. GTE transfers use the
shared GTE macros.

The callback and spawning controller now share `SceneE20Particle` and the
floor-height record in `include/pe1/scene_e20_flare.h`. Earlier candidate
variants remain available in Git history.
