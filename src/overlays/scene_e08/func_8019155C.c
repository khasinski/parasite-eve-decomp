#include "common.h"
#include "../room_lib/room_lib.h"

typedef struct {
    s32 words[8];
} SceneE08MatrixWords;

typedef struct {
    RoomLink *link;
    SceneE08MatrixWords matrix;
    void *asset;
} SceneE08InitState;

extern void func_800C2B40(void *);
extern void *func_8006DC18(int);
extern int func_8006E498(void *, int);
extern void func_8006DF50(void *, int, int, int, int);
extern void *volatile D_800B0E64;
extern RoomLink *g_PlayerEntity;
extern int D_80199528, D_8019956C, D_8019957C;
extern volatile u8 D_80199680, D_80199681, D_80199682, D_80199684, D_80199685, D_80199686;
extern volatile s16 D_80199688, D_8019968A;
extern volatile u8 D_801996A0, D_801996A1, D_801996A2, D_801996A4, D_801996A5, D_801996A6;
extern volatile s16 D_801996A8, D_801996AA;
extern volatile u8 D_801996B0, D_801996B1, D_801996B2, D_801996B4, D_801996B5, D_801996B6;
extern volatile s16 D_801996B8, D_801996BA;

void func_8019155C(RoomEnt *ent, void *unused, SceneE08InitState *state) {
    SceneE08MatrixWords *matrix;
    void *asset;
    int half;
    RoomLink *player;
    register int intensity asm("$3");
    register int mode asm("$2");
    register int offset asm("$5");
    void *volatile *sound = &D_800B0E64;
    void *handle;
    int playerX;
    register int playerZ asm("$3");

    func_800C2B40(state);
    D_80199528 = func_8006E498(*sound, 0x10D8704);
    state->link = ent->link;
    matrix = (SceneE08MatrixWords *)state->link->p238;
    state->matrix = *matrix;
    asset = func_8006DC18(0x2E);
    intensity = 0x80;
    state->asset = asset;
    mode = 8;
    offset = -400;

    D_80199688 = offset;
    D_801996A8 = offset;
    /* Empty compiler barriers retain the retail register lifetimes. */
    asm volatile ("" : "=r"(player) : "0"(g_PlayerEntity));
    asm volatile ("" : "=r"(half) : "0"(0x80));
    D_80199685 = mode;
    D_801996A4 = 0x44;
    D_801996A5 = 2;
    D_801996B4 = 0x60;
    D_801996B5 = 0x40;
    D_80199684 = intensity;
    D_8019968A = half;
    D_80199680 = intensity;
    D_80199681 = intensity;
    D_80199682 = intensity;
    D_80199686 = 0;
    D_801996AA = half;
    D_801996A0 = intensity;
    D_801996A1 = intensity;
    D_801996A2 = intensity;
    D_801996A6 = 0;
    D_801996B8 = -500;
    D_801996BA = half;
    D_801996B0 = intensity;
    D_801996B1 = intensity;
    D_801996B2 = intensity;
    D_801996B6 = 0;

    handle = *sound;
    /* Keep the handle load ahead of the two player-coordinate loads. */
    asm volatile("" : : "r"(handle));
    playerZ = ((s16 *)player->pos)[5];
    playerX = ((s16 *)player->pos)[1];
    D_8019956C = playerX;
    D_8019957C = playerZ;
    if (handle != 0) {
        func_8006DF50(*sound, 0x603, 0, 0x80, 0x7F);
    }
}
