#include "room_m273.h"

int func_801996C4(int mode)
{
    RoomM273TemplatePoint *template;
    RoomM273TemplatePoint *particle;
    RoomM273TrigEntry *entry;
    int i;
    int j;
    int angle;
    int offset;
    int flagOffset;
    int sixteen;
    u16 paletteIndex;
    u8 *stop;
    u16 *flag;
    RoomM273TemplatePoint *source;
    u16 palette;

    switch (mode) {
    case 0:
        return func_800CE560(D_800F33E0->pool, 8, 24, (int (*)())func_80199568);
    case 1:
        stop = &D_8019AFA2;
        if (*stop) return 2;
        i = 0;
        template = (RoomM273TemplatePoint *)(stop - 0x1E);
        offset = 0;
        do {
            flag = (u16 *)(D_8019AF8A + offset);
            if (*flag & 2) {
                j = 0;
                flagOffset = offset;
                source = template;
                do {
                    particle = (RoomM273TemplatePoint *)func_800CE610(D_800F33E0->pool);
                    if (!particle) break;
                    *particle = *source;
                    angle = ((j << 12) / 6) & 0xFFF;
                    entry = &D_800966EC[angle];
                    particle->x += (entry->low * 3 * 64) / 4096;
                    particle->z += (entry->high * 3 * 64) / 4096;
                    particle->flags = 0;
                    *(u16 *)(D_8019AF8A + flagOffset) &= ~2;
                    j++;
                } while (j < 6);
            }
            template++;
            i++;
            offset += 8;
        } while (i < 2);
        break;
    case 2:
        paletteIndex = D_800E11E8;
        sixteen = 16;
        D_800F3368.parameter00 = sixteen;
        D_800F3368.parameter02 = 1;
        D_800F3376 = sixteen;
        D_800F3378 = sixteen;
        D_800F3376 = sixteen;
        D_800F3378 = sixteen;
        palette = D_800E2850[paletteIndex];
        D_800F3368.palette = 2;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 0;
        D_800F3368.tpage = palette;
        break;
    }
    return 0;
}
