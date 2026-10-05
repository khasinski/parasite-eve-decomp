/* Paired ground sprites and the boss ground ring share sway points and pool records. */
#include "room_m273_boss.h"
#include "pe1/gte.h"
#include "room_m273_sway.h"
typedef GteShortVector Vector;
typedef RoomM273PairPoolRecord Record;

/* The emitter owns the eight-byte pool whose entries this callback renders. */

extern short D_800966EE[];
extern unsigned short D_800F336E, D_800F3370;
extern short D_800F3372;
extern unsigned char D_8019AE18[], D_8019AB70[];

int func_80198B1C(int mode,Vector *input) {
    Vector position;
    if(mode==1) {
        if(D_800E27EC>=32) return 1;
    } else if(mode==2) {
        unsigned int phase=(unsigned int)(D_800E27EC-1)<<5;
        int shadeSample=*(short *)((char *)D_800966EE+(((phase+1024)&4095)*4));
        int sizeSample=*(short *)((char *)D_800966EE+((phase&4095)*4));
        unsigned int shade;
        int size,kind,palette;
        unsigned short clut;
        position=*input;
        kind=D_800F336C;
        shade=(unsigned int)(shadeSample+4096)>>5;
        size=sizeSample*2+4096;
        palette=D_800E1204[kind];
        clut=GetClut(0,(kind==4 && D_800F3428) ? palette+8 : palette+4);
        func_800CEE20(&position,0,(short)size,(short)size,100,clut,1,(short)shade,0);
        position.y=D_800942EC.value;
        func_800D004C(&position,384,384,8,&D_8019AB68,4096,4096,
            D_8019AE18,D_8019AB70,(short)shade,1);
    }
    return 0;
}

int func_80198CD4(int mode) {
    switch(mode) {
    case 0:
        return func_800CE560(D_800F33E0->pool,8,4,func_80198B1C);
    case 1: {
        Record *source;
        int i;
        if(D_8019AF74.done) return 2;
        if(D_8019AF74.animation!=15) return 0;
        if(D_8019AF74.frame<4) return 0;
        if(D_8019AF74.frame_1A>=4) break;
        i=0;
        source=(Record *)D_8019AF74.points;
        for(;i<2;++i) {
            Record *output=func_800CE610(D_800F33E0->pool);
            if(!output) break;
            *output=*source;
            ++source;
        }
        break;
    }
    case 2: {
        unsigned int index=D_800E11FA;
        unsigned short palette;
        D_800F3368.parameter00=32;
        D_800F336A=2;
        D_800F3376=32;
        D_800F3378=32;
        D_800F3376=32;
        D_800F3378=32;
        palette=D_800E2850[index];
        asm("" : : "r"(palette) : "memory", "$2");
        D_800F336C=3;
        D_800F336E=1;
        D_800F3372=0;
        D_800F3374=32;
        D_800F3370=palette;
        break;
    }
    }
    return 0;
}

/* Ground ring: while it opens (frames up to 24) it knocks the player back
 * once when the player stands inside the pulsing radius; while drawing it
 * shows the swelling ring model, a short flash and two rotating rings. */
int func_80198E94(int mode, GteShortVector *position) {
    GteRotation rotation;
    GteMatrix matrix;
    GteVector scale;
    s16 unused[4];
    int frame;
    int size;
    int fade;
    int value;
    int i;

    if (mode == 1) {
        int now = D_800E27EC;
        if (now >= 0x28) return 1;
        if (now >= 0x19) return 0;
        if (position->pad != 0) return 0;
        if (D_8019AF74.cooldown != 0) return 0;
        {
            int radius = D_800966EC[((now << 11) / 40) & 0xFFF].sine / 16 + 0x80;
            int dx = g_PlayerEntity->position[0] - position->x;
            int dz = g_PlayerEntity->position[2] - position->z;
            if (radius < func_8005186C(dx * dx + dz * dz)) return 0;
        }
        position->pad = 1;
        g_PlayerEntity->actor->flags |= 0x4000;
        if (D_800F32D0->instance->owner) {
            D_800F32D0->instance->owner->flags |= 0x80000000;
            D_8019AF74.cooldown = 0x28;
        }
        return 0;
    }
    if (mode != 2) return 0;
    frame = D_800E27EC - 1;
    if (D_800E27EC >= 5 && D_800E27EC < 0x25) {
        int page;
        int kind;
        int palette;
        int cosine;
        int sine;
        i = D_800E27EC - 5;
        if (i < 8) value = D_800966EC[(i << 7) & 0xF80].sine;
        else value = D_800966EC[(((D_800E27EC - 13) << 10) / 24) & 0xFFF].cosine;
        page = (D_800E2850[D_800E11FA] | GetTPage(0, 1, 0, 0)) & 0xFFFF;
        fade = value >> 6;
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        size = value >> 2;
        GsSetOrign(page, GetClut(0, (kind == 4 && D_800F3428 != 0) ? palette + 6 : palette + 2));
        func_800C6ED8(1);
        func_800C6EF8(D_8019AF04.model);
        func_800C6FA0(D_8019AF04.model, (u16)fade);
        cosine = rcos(frame << 8);
        sine = rsin(frame << 8);
        matrix.m[0][2] = sine;
        matrix.m[2][0] = -sine;
        matrix.m[0][0] = cosine;
        matrix.m[2][2] = cosine;
        matrix.t[2] = 0;
        matrix.t[1] = 0;
        matrix.t[0] = 0;
        matrix.m[2][1] = 0;
        matrix.m[1][2] = 0;
        matrix.m[1][0] = 0;
        matrix.m[0][1] = 0;
        matrix.m[1][1] = 0x1000;
        memset(&scale, 0, sizeof(scale));
        scale.x = size;
        scale.y = size;
        scale.z = size;
        Gte_ScaleMatrix(&matrix, &scale);
        matrix.t[0] = position->x;
        matrix.t[1] = position->y;
        matrix.t[2] = position->z;
        func_800C71E4(D_8019AF04.model, &matrix);
        func_800C6F4C(D_8019AF04.model);
    }
    D_800F3368.parameter00 = 0x20;
    D_800F3368.parameter02 = 2;
    D_800F3368.extent_x = 0x20;
    D_800F3368.extent_y = 0x20;
    D_800F3368.extent_x = 0x20;
    D_800F3368.extent_y = 0x20;
    D_800F3368.tpage = D_800E2850[D_800E11EA.index];
    D_800F3368.palette = 3;
    D_800F3368.parameter06 = 0;
    if (frame < 8) {
        int kind = D_800F3368.palette;
        int palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428 != 0) palette += 4;
        size = D_800966EC[(frame << 7) & 0xF80].sine + 0x1800;
        func_800CEE20(position, 0, size, size, (s16)D_800F3368.parameter02 * frame + 0x80,
                      (u16)GetClut(0x10, palette), 1, 0x80, 0);
    }
    rotation.x = 0x400;
    rotation.y = -frame << 9;
    rotation.flags = 1;
    rotation.z = 0;
    fade = D_800966EC[(frame * 25) & 0xFFF].cosine >> 5;
    value = D_800966EC[(frame * 51) & 0xFFF].sine / 16;
    for (i = 0; i < 2; i++) {
        func_800D004C(position, value, value, 8, &rotation, 0x1000, 0x1000,
                      &D_8019AE1C[0], &D_8019AE1C[1], fade, 1);
        value += 0x100;
    }
    return 0;
}

/* Partial storage views; the fourth halfword carries the emitter flag. */
typedef RoomM273PairPoolRecord Entry;
typedef struct { Entry entries[2]; unsigned char unknown[14]; unsigned char stopped; } EmitterState;
extern void *D_800B0E64, *D_8019AF6C;
extern EmitterState D_8019AF84;
extern short D_800F3374;
extern void *Asset_FindTable08ByU32Key(void *, unsigned int);
extern void func_800C6D5C(void *, int, int);

int func_801993F0(int mode) {
    switch (mode) {
    case 0:
        D_8019AF6C = Asset_FindTable08ByU32Key(D_800B0E64, 0xC5541704);
        func_800C6D5C(D_8019AF6C, 0, 0);
        return func_800CE560(D_800F33E0->pool, 8, 4, func_80198E94);
    case 1: {
        int i;
        unsigned char *flags;
        Entry *source;
        unsigned char *anchor = (unsigned char *)&D_8019AF84 + 30;
        if (*anchor) return 2;
        i = 0;
        flags = anchor - 24;
        source = (Entry *)(anchor - 30);
        for (; i < 2; ++i) {
            asm("" : "=r"(source) : "0"(source));
            if (*(unsigned short *)flags & 1) {
                Entry *entry = func_800CE610(D_800F33E0->pool);
                if (!entry) return 0;
                entry->emitter.x = source->emitter.x;
                entry->emitter.y = source->emitter.y;
                entry->emitter.z = source->emitter.z;
                entry->emitter.flags = 0;
                *(unsigned short *)flags &= 0xFFFE;
            }
            flags += 8;
            ++source;
        }
        break;
    }
    case 2:
        D_800F3372 = 0;
        D_800F3374 = 0;
        break;
    }
    return 0;
}
