/* func_80193488 */
void func_80193488(void) {
}

/* func_80193490 */
void func_80193490(void) {
}

/* func_80193498 */
void func_80193498(void) {
}

/* func_801934A0 */
void func_801934A0(void* arg0, void* arg1, unsigned char* arg2) {
    unsigned int i;

    *(int*)arg2 = 0;

    for (i = 0; i < 8; i++) {
        *(arg2 + i + 0xB4) = 0;
    }
}

/* RoomLib_DrawEightScaledParticles_801934C8 */
#define ROOMLIB_DRAW_EIGHT_SCALED_NAME func_801934C8
#define ROOMLIB_DRAW_EIGHT_SCALED_TABLE D_80196F0C
#define ROOMLIB_DRAW_EIGHT_SCALED_DEPTH D_8019742A
#include "../room_lib/RoomLib_DrawEightScaledParticles.inc"

/* RoomLib_UpdateEightParticles_80193654 */
#define ROOMLIB_UPDATE_EIGHT_NAME func_80193654
#define ROOMLIB_UPDATE_EIGHT_SEED0 D_8018F064
#define ROOMLIB_UPDATE_EIGHT_SEED1 D_8018F06C
#define ROOMLIB_UPDATE_EIGHT_SEED2 D_8018F074
#include "../room_lib/RoomLib_UpdateEightParticles.inc"
