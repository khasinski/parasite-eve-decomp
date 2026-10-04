# room_m273 func_801981A4 (0x91BC, 0x544 bytes): sway shard callback

Candidate: `RoomEffect_SwayShardCallback.c` (uses `src/overlays/room_m273/room_m273_boss.h`).

Status: mode 1 (player hit / floor hit recorded in `D_8019AF74.hits`, trail
spawn) matches byte for byte. Mode 2 differs only around the two-flare loop.

Cause: loop-invariant hoisting. cc1 `-dL` on the candidate reports a 23-insn
loop where stock loop.c moves `&spin`, `&D_8019AE04` and `D_8019AB70` out of
the loop. Retail moves only `&spin` (its `addiu s5,sp,0x38`) and keeps
`la D_8019AE04` / `la D_8019AB70` inside the loop. With
`threshold * savings * lifetime >= insn_count` (savings 1, lifetime 1) that
puts retail's threshold in [23, 26), while stock uses 58.

The parked room_m273 func_8019665C (queued drop callback) shows the same
symptom and bounds the threshold to [19, 25): retail's effective loop.c
threshold is about 24, not the 58 of our cc1. This is the compiler-config
difference already noted for room_m075 func_8018F3DC; there is no sanctioned
per-file flag for it, so the function is parked.

Secondary difference (would likely settle once the loop matches): retail
reads `D_800F3368.parameter02` for the texture argument into a0 after the
GetClut call.
