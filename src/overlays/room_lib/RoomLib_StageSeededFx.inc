#include "common.h"
#include "pe1/room_fx.h"

extern char ROOMLIB_STAGE_SEED_CALLBACK[];
extern char ROOMLIB_STAGE_SEED_DATA[];
extern char *func_800C2B50(void);
extern char *func_800C2B90(void *, int, char *, char *);

void ROOMLIB_STAGE_SEED_NAME(void *effect, char *state, RoomFxSeed8 *seed) {
    char *owner = func_800C2B50();
    char *slot;

    if (*(s16 *)(state + 2) == 0) {
        slot = func_800C2B90(effect, 4, ROOMLIB_STAGE_SEED_CALLBACK, ROOMLIB_STAGE_SEED_DATA);
        if (slot != 0) {
            *(RoomFxSeed8 *)slot = *seed;
        }
        if (*(s16 *)(state + 2) == 0) {
            slot = func_800C2B90(effect, 3, ROOMLIB_STAGE_SEED_CALLBACK, ROOMLIB_STAGE_SEED_DATA);
            if (slot != 0) {
                *(RoomFxSeed8 *)slot = *seed;
            }
            if (*(s16 *)(state + 2) == 0) {
                slot = func_800C2B90(effect, 2, ROOMLIB_STAGE_SEED_CALLBACK, ROOMLIB_STAGE_SEED_DATA);
                if (slot != 0) {
                    *(s16 *)(slot + 0x14) = 1;
                    *(RoomFxSeed8 *)slot = *seed;
                }
            }
        }
    }
    if (*(s16 *)(state + 2) == 4) {
        slot = func_800C2B90(effect, 2, ROOMLIB_STAGE_SEED_CALLBACK, ROOMLIB_STAGE_SEED_DATA);
        if (slot != 0) {
            *(s16 *)(slot + 0x14) = 0;
            *(RoomFxSeed8 *)slot = *seed;
        }
    }
    if (*(s16 *)(state + 2) == 6) {
        slot = func_800C2B90(effect, 5, ROOMLIB_STAGE_SEED_CALLBACK, ROOMLIB_STAGE_SEED_DATA);
        if (slot != 0) {
            int i;

            *(RoomFxSeed8 *)slot = *seed;
            if (*(s16 *)(owner + 0x4E) == 0) {
                *(s16 *)(slot + 0xE8) = 8;
                *(s16 *)(slot + 0xEC) = 8;
                /* The original loop compares the signed halfword count as unsigned. */
                for (i = 0; i < (u32)*(s16 *)(slot + 0xE8); i++) {
                    *(s16 *)(slot + 0x98 + i * 2) = i << 9;
                }
            } else {
                *(s16 *)(slot + 0xE8) = 6;
                *(s16 *)(slot + 0xEC) = 6;
                for (i = 0; i < (u32)*(s16 *)(slot + 0xE8); i++) {
                    *(s16 *)(slot + 0x98 + i * 2) = i * 0x2A8;
                }
            }
        }
    }
    if (*(s16 *)(state + 2) >= 0x3D) {
        state[1] = 2;
    }
}
