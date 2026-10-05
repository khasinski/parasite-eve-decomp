#include "room_m350_shared.h"
#include "pe1/render_object.h"

extern int D_800E27EC;
extern short D_800966EE[];
extern RenderColor D_8019A62C;
typedef RoomM350SignedHalfPair SignedHalf;
extern int GetClut(int, int);

int func_80197594(int event, short *position) {
    if (event == 1) {
        if (D_800E27EC >= 16) return 1;
        position[1] -= position[3];
        position[3] += 2;
    } else if (event == 2) {
        int kind = D_800F336C;
        int texture = D_800E1204[kind];
        int handle;
        handle = GetClut(0, (kind == 4 && D_800F3428 != 0) ? texture + 6 : texture + 2);
        func_800CEE20((GteShortVector *)position, 0, 6144, 6144,
            (s16)D_800F3368.parameter02 * (((D_800E27EC - 1) >> 1) & 7),
            (unsigned short)handle, 1,
            ((SignedHalf *)((char *)D_800966EE - 2 +
              (((unsigned int)(D_800E27EC - 1) << 8) & 0x3F00)))->value >> 5,
            &D_8019A62C);
    }
    return 0;
}


extern RoomM350Emitter *D_800F33E0;
extern unsigned char D_8019A86E, D_8019A857;
extern short D_8019A864[3];
extern unsigned short D_800E11E4[];
extern int func_800CE560(void *, int, int, int (*)(int, short *));
extern short *func_800CE610(void *);

int func_801976C8(int event)
{
    if (event == 1) goto update;
    if (event < 2) {
        if (event == 0) goto setup;
        goto done;
    }
    if (event == 2) goto configure;
    goto done;
setup:
    return func_800CE560(D_800F33E0->pool, 8, 4, func_80197594);
update:
    if (D_8019A86E) return 2;
    if (D_8019A857) {
        short *effect = func_800CE610(D_800F33E0->pool);
        if (!effect) return 0;
        do {
            effect[0] = D_8019A864[0];
            effect[1] = D_8019A864[1];
            effect[2] = D_8019A864[2];
            effect[3] = 0;
        } while (0);
        D_8019A857 = 0;
    }
    goto done;
configure:
    D_800F3368.parameter00 = 32;
    D_800F3368.parameter02 = 2;
    D_800F3368.extent_x = 32;
    D_800F3368.extent_y = 32;
    D_800F3368.extent_x = 32;
    D_800F3368.extent_y = 32;
    D_800F3368.tpage = D_800E2850[D_800E11E4[11]];
    D_800F3368.palette = 3;
    D_800F3368.parameter06 = 1;
    D_800F3368.parameter0A = 0;
    D_800F3368.depth = 0;
done:
    return 0;
}
