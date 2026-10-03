#include "room_m273.h"

extern int D_800E27EC, D_800F3428;
extern unsigned short D_800F336C, D_800E1204[];
extern unsigned short GetClut(int, int);

int func_80199568(int mode, RoomM273RisingParticle *particle) {
    if (mode == 1) {
        int frame = D_800E27EC;
        if (frame >= 36) return 1;
        if (frame >= 4) {
            unsigned short y = ((volatile RoomM273RisingParticle *)particle)->y;
            register int speed asm("$3") = ((volatile RoomM273RisingParticle *)particle)->speed;
            int delta = ((volatile RoomM273RisingParticle *)particle)->speed;
            speed += 1;
            asm("" : "=r"(y) : "0"(y), "r"(speed));
            particle->y = y - delta;
            particle->speed = speed;
        }
    } else if (mode == 2) {
        int frame = D_800E27EC - 5;
        int kind, palette, clut;
        if (frame < 0) return 0;
        kind = D_800F336C;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) palette += 4;
        clut = GetClut(16, palette);
        func_800CEE20((GteShortVector *)particle, 0, 12288, 12288,
            D_800F336A * (frame / 4) + 200, clut, 1,
            (short)((int *)D_800966EC)[(((unsigned int)frame << 8) & 0x3F00) / 4] >> 5, 0);
    }
    return 0;
}

int func_801996C4(int mode)
{
    RoomM273TemplatePoint *template;
    RoomM273DelayedBurstRecord *particle;
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
                    particle = (RoomM273DelayedBurstRecord *)func_800CE610(D_800F33E0->pool);
                    if (!particle) break;
                    particle->emitter = *source;
                    angle = ((j << 12) / 6) & 0xFFF;
                    entry = &D_800966EC[angle];
                    particle->emitter.x += (entry->low * 3 * 64) / 4096;
                    particle->emitter.z += (entry->high * 3 * 64) / 4096;
                    particle->emitter.flags = 0;
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
