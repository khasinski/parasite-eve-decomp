#include "room_m273.h"
#include "pe1/psyq_gpu.h"

int func_80195984(int mode, RoomM273PulseInput *input)
{
    GteShortVector position;
    int frame;
    int palette;
    int size;
    int sample;
    u16 clut;

    if (mode == 1) {
        if (D_800E27EC >= 16) return 1;
        input->position.pad += input->velocity;
        input->velocity += 2;
    } else if (mode == 2) {
        frame = D_800E27EC - 1;
        sample = D_800966EC[(((unsigned int)frame << 8) & 0x3F00) / sizeof(RoomM273TrigEntry)].low;
        position = input->position;
        palette = D_800E1204[D_800F336C];
        size = sample * 2 + 2048;
        if (D_800F336C == 4 && D_800F3428) palette += 4;
        clut = GetClut(16, palette);
        func_800CEE20(&position, 0, (s16)size, (s16)size,
                       D_800F336A * (frame / 2) + 128, clut, 1,
                       D_800966EC[(((unsigned int)frame << 8) & 0x3F00) / sizeof(RoomM273TrigEntry)].high >> 5, 0);
        position.y -= input->position.pad;
        {
            int clut2;
            palette = D_800E1204[D_800F336C];
            if (D_800F336C == 4 && D_800F3428) palette += 4;
            clut2 = GetClut(64, palette);
            func_800CEE20(&position, 0, 8192, 8192,
                           D_800F336A * D_8019ACC0[frame / 4], clut2, 1,
                           (((s32 *)D_800966EC)[(((unsigned int)frame << 9) & 0x3E00) / 4] << 16) >> 21, 0);
        }
    }
    return 0;
}
