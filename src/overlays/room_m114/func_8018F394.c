#include "../room_lib/room_lib.h"

typedef struct RoomM114FxSoundState {
    RoomLink *link;
    RoomLibFxMatrixWords matrix;
    void *asset;
    short h28;
    short h2A;
    short h2C;
} RoomM114FxSoundState;

extern unsigned char *D_800B0E64;
extern unsigned char D_80193720;
extern unsigned char D_80193721;
extern unsigned char D_80193722;
extern unsigned char D_80193724;
extern unsigned char D_80193725;
extern unsigned char D_80193726;
extern short D_80193728;
extern short D_8019372A;
extern int D_8019372C;

int func_8006E498(void *a0, int a1);

void func_8018F394(RoomEnt *ent, void *unused, RoomM114FxSoundState *state) {
    RoomLibFxMatrixWords *matrix;
    void *asset;
    register unsigned char *soundBank asm("$4");
    register int soundId asm("$5");
    register int color asm("$2");

    func_800C2B40(state);
    state->link = ent->link;
    matrix = (RoomLibFxMatrixWords *)state->link->p238;
    state->matrix = *matrix;
    state->h2A = 0;
    state->h2C = 0;
    state->h28 = 0x1E;
    asset = func_8006DC18(0x23);
    soundId = 0xCB0000;
    asm volatile("" : : "r"(soundId));
    state->asset = asset;
    asm volatile("" : : : "memory");
    soundBank = D_800B0E64;
    asm volatile("" : : "r"(soundBank) : "memory");
    D_80193728 = -0x12C;
    D_8019372A = 0x80;
    asm volatile("" : : : "memory");
    color = 0x80;
    D_80193724 = 0;
    D_80193725 = 0;
    D_80193720 = color;
    D_80193721 = color;
    D_80193722 = color;
    D_80193726 = 0;
    D_8019372C = func_8006E498(soundBank, soundId | 0x8704);
}
