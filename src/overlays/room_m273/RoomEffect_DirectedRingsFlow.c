#include "room_m273_boss.h"
#include "pe1/gte.h"



/* Falling drop: queues its landing position, then knocks the player back on
 * contact; draws two rings before landing and a splash flash after. */
int func_8019665C(int mode, RoomM273Drop *drop) {
    GteShortVector ring;
    GteShortVector unused;
    int i;

    if (mode == 1) {
        if (drop->position.pad == 0) {
            RoomM273BossFloor *floor = &D_800942EC;
            drop->position.x += drop->vx;
            drop->position.y += drop->vy;
            drop->position.z += drop->vz;
            if (drop->position.y >= (s16)floor->value) {
                drop->position.y = floor->value;
                drop->position.pad = 1;
                i = D_8019AF04.count++;
                D_8019AF04.x[i] = drop->position.x;
                D_8019AF04.y[i] = floor->value;
                D_8019AF04.z[i] = drop->position.z;
            }
            if (drop->touched) return 0;
            if (drop->position.y < D_8019AF62) return 0;
            if (Math_IntSqrt((g_PlayerEntity->position[0] - drop->position.x) *
                                 (g_PlayerEntity->position[0] - drop->position.x) +
                             (g_PlayerEntity->position[2] - drop->position.z) *
                                 (g_PlayerEntity->position[2] - drop->position.z)) < 0x80) {
                drop->touched = 1;
                D_8019AF04.hit = drop->position;
                D_8019AF04.hit.pad = 1;
                g_PlayerEntity->actor->flags |= 0x4000;
                if (D_800F32D0->instance->owner)
                    D_800F32D0->instance->owner->flags |= 0x80000000;
            }
        } else {
            if (drop->position.pad++ >= 8) return 1;
        }
    } else if (mode == 2) {
        int shade = D_800966EC[(drop->position.pad << 7) & 0xF80].cosine >> 5;
        RoomM273BossTrig *trig = D_800966EC;
        ring.x = drop->ring.x;
        ring.y = drop->ring.y;
        ring.z = drop->ring.z;
        if (drop->position.pad == 0) {
            for (i = 0; i < 2; i++) {
                func_800D0E88(&drop->position, &ring, 0x100, 0x10, &D_8019AD58,
                              &D_8019AD54, &D_8019AD54, (s16)shade, 1);
                ring.z += 0x400;
            }
        } else {
            int size = trig[(drop->position.pad << 7) & 0xF80].sine * 2 + 0x1000;
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            func_800CEE20(&drop->position, &D_8019AB68, size, size, 0xDC,
                          GetClut(0x30, palette), 1, shade, &D_8019AD5C);
        }
    }
    return 0;
}

/* room_m273, file offset 0x79F0, 1364 retail-matching bytes. */
#include "pe1/gte.h"
typedef GteShortVector Vector;
typedef struct { short m[3][3]; int t[3]; } Matrix;
typedef struct { unsigned char unknown[0x2A]; short x,unknown2C,y,unknown30,z; } Player;
extern unsigned char D_8019AF69,D_8019AF68;
extern unsigned short D_8019AF60;
extern Vector D_8019AD60;
extern unsigned short D_800E11E8,D_800E2850[];
extern int func_8005186C(int),func_80079FB4(int,int);
extern void func_80079754(Vector *,Matrix *);
extern void func_8006DCE4(int,void *,int,int,int);

int func_801969D8(int mode) {
    Vector playerPosition;
    Matrix matrix;
    switch(mode) {
    case 0:
        return func_800CE560(D_800F33E0->pool,24,40,func_8019665C);
    case 1: {
        register unsigned char *stopped asm("$17")=&D_8019AF69;
        Player *player;
        int dx,dy,dz,x,y,z; short spread; short angle;
        if(*stopped) return 2;
        player=(Player *)g_PlayerEntity;
        playerPosition.x=x=player->x;
        dx=x-D_8019AEFC.x;
        playerPosition.y=y=player->y;
        playerPosition.z=z=player->z;
        dz=z-D_8019AEFC.z;
        dy=y-D_8019AEFC.y;
        angle=func_80079FB4(dy,func_8005186C((unsigned int)dx*dx+(unsigned int)dz*dz))&4095;
        if(angle>768) angle=768;
        if(angle<384) angle=384;
        /* Pitch is clamped to [384, 768]; the second ring halves its spread. */
        spread=angle-256;
        if(D_8019AF68) {
            short i=0;
            int width=(short)spread;
            /* Retail address window: heading 0x8019AF60, origin 0x8019AEFC. */
            register unsigned short *heading asm("$19")=(unsigned short *)(stopped-9);
            register Matrix *transform asm("$16")=&matrix;
            matrix.t[0]=0; matrix.t[1]=0; matrix.t[2]=0;
            do {
                RoomM273Drop *particle=func_800CE610(D_800F33E0->pool);
                short *wave;
                if(!particle) break;
                wave=(short *)&((int *)D_800966EC)[(i*4096/6)&4095];
                particle->ring.x=angle+wave[1]*160/4096;
                particle->ring.y=wave[0]*width/4096+(*heading+2048);
                particle->ring.z=0; particle->ring.pad=1;
                func_80079754(&particle->ring,transform);
                gte_ldrotmatrix(transform);
                gte_ldtransmatrix(transform);
                { Vector *input=&D_8019AD60; gte_ldv0(input); }
                gte_cop2_hazard_slot();
                gte_cop2_hazard_slot();
                gte_mvmva_rotation_v0_translation_sf12();
                gte_stsv((GteShortVector *)&particle->vx);
                particle->position.x=heading[-50];
                particle->position.y=heading[-49];
                particle->position.z=heading[-48];
                particle->position.pad=0; particle->touched=0;
                i++;
            } while(i<6);
            /* Failure in the six-particle ring does not skip this ring. */
            i=0; width=(short)spread/2; heading=&D_8019AF60;
            do {
                RoomM273Drop *particle=func_800CE610(D_800F33E0->pool);
                short *wave;
                if(!particle) break;
                {
                    register int *table asm("$3")=(int *)D_800966EC;
                    wave=(short *)&table[((unsigned int)i<<10)&3072];
                }
                particle->ring.x=angle+wave[1]*96/4096;
                particle->ring.y=wave[0]*width/4096+(*heading+2048);
                particle->ring.z=0; particle->ring.pad=1;
                asm("" : : "r"(&particle->ring));
                transform=&matrix;
                func_80079754(&particle->ring,transform);
                gte_ldrotmatrix(transform);
                gte_ldtransmatrix(transform);
                { Vector *input=&D_8019AD60; gte_ldv0(input); }
                gte_cop2_hazard_slot();
                gte_cop2_hazard_slot();
                gte_mvmva_rotation_v0_translation_sf12();
                gte_stsv((GteShortVector *)&particle->vx);
                particle->position.x=heading[-50];
                particle->position.y=heading[-49];
                particle->position.z=heading[-48];
                particle->position.pad=0; particle->touched=0;
                i++;
            } while(i<4);
            func_8006DCE4(0x5D1,((RoomM273EffectStateContext *)D_800F32D0)->state.directed_rings->owner->value,D_8019AEFC.x,D_8019AEFC.y,D_8019AEFC.z);
        }
        break;
    }
    case 2: {
        unsigned int index=D_800E11E8;
        unsigned short palette;
        D_800F3368.parameter00=16; D_800F3368.parameter02=1;
        D_800F3368.extent_x=16; D_800F3368.extent_y=16;
        D_800F3368.extent_x=16; D_800F3368.extent_y=16;
        palette=D_800E2850[index];
        D_800F3368.palette=2; D_800F3368.parameter06=0; D_800F3368.parameter0A=0; D_800F3368.depth=0;
        D_800F3368.tpage=palette;
        break;
    }
    }
    return 0;
}
