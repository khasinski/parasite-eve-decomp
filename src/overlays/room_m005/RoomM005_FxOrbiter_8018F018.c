#include "../room_lib/room_lib.h"

typedef struct RoomM005_OrbRec {
    short h0;                     /* 0x00: x */
    short h2;                     /* 0x02: heading */
    short h4;                     /* 0x04: z */
    short h6;                     /* 0x06: size, ramps to 0x88 */
    short h8;                     /* 0x08: orbit phase */
    short hA;                     /* 0x0A: radius */
    short hC;                     /* 0x0C: state */
    short hE;                     /* 0x0E: fade counter */
} RoomM005_OrbRec;

typedef struct RoomM005_Dialog {
    short h0;
    short h2;
    unsigned short h4;            /* 0x04: anchor x */
    unsigned short h6;            /* 0x06: anchor y */
    unsigned short h8;            /* 0x08: anchor z */
    short hA;
    short hC[1];                  /* 0x0C: fade target block */
} RoomM005_Dialog;

extern RoomM005_Dialog RoomM005_DialogAnchor;
extern char RoomM005_Rec2[];
extern int D_800E27EC;
extern unsigned short D_800F336C;
extern short D_800F336A;
extern int D_800F3428;
extern unsigned short D_800E1204[];
extern int func_80077CF4();
extern int func_80077DC4();
extern int func_80077AA4();
extern int func_800783E4();
extern int func_800CF3AC();
extern int func_800CEE20();

int RoomM005_FxOrbiter_8018F018(int mode, RoomM005_OrbRec *r) {
    unsigned short anchor[3];
    short q[4];
    char out[8];

    if (mode == 1) goto tick;
    if (mode == 2) goto burst;
    return 0;

tick:
    if (r->hC == 0) goto init;
    if (r->hC == mode) goto fade;
    return 0;

init:
    anchor[0] = RoomM005_DialogAnchor.h4;
    anchor[1] = RoomM005_DialogAnchor.h6;
    anchor[2] = RoomM005_DialogAnchor.h8;
    r->h2 += func_80077CF4(D_800E27EC << 6) / 512;
    r->h0 = anchor[0] + func_80077DC4(r->h8) * r->hA / 4096;
    r->h4 = anchor[2] + func_80077CF4(r->h8) * r->hA / 4096;
    r->h8 += 0x18;
    if (r->h6 < 0x81) {
        r->h6 += 8;
    }
    if (D_800E2368->h12 == mode) {
        r->hC = 1;
        r->hE = 0;
    }
    return 0;

fade:
    {
        short n = r->hE + 1;
        r->hE = n;
        func_800783E4(r, RoomM005_DialogAnchor.hC, 0x1000 - (n << 7), n << 7, r);
    }
    if (r->hE < 0x20) goto ret0;
    return 1;

burst:
    q[0] = 0;
    q[1] = 0;
    q[2] = r->h8;
    q[3] = 0;
    func_800CF3AC(RoomM005_Rec2, out, (r->h2 + D_800E27EC) & 0x3F);
    {
        int a = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428 != 0) {
            a += 4;
        }
        func_800CEE20(r, q, 0x800, 0x800, D_800F336A + 0xD8,
                      (unsigned short)func_80077AA4(0x20, a), 3, r->h6, out);
    }
    {
        int a = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428 != 0) {
            a += 4;
        }
        func_800CEE20(r, q, 0x1000, 0x1000, (D_800F336A << 1) + 0xD8,
                      (unsigned short)func_80077AA4(0x20, a), 3, r->h6, out);
    }
ret0:
    return 0;
}
