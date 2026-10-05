/* The rising-sprite callback and its poll/reset controller share one TU. */
#include "room_m273_particles.h"

typedef RoomM273RisingParticle Particle;
typedef struct { unsigned char r, g, b, unknown; } Color;
extern Color D_8018F1E4;
extern int D_800E27EC, D_800F3428, D_800966EC[];
extern unsigned short D_800F336C, D_800E1204[];
extern unsigned short GetClut(int, int);
extern void func_800CEE20(Particle *, void *, int, int, int, int, int, int, Color *);

int func_80195E10(int mode, Particle *particle) {
    Color color = D_8018F1E4;
    if (mode == 1) {
        if (D_800E27EC >= 16) return 1;
        else {
            unsigned short y = particle->y;
            register int speed asm("$3") = ((volatile Particle *)particle)->speed;
            int delta = ((Particle *)particle)->speed;
            speed += 1;
            particle->y = y - delta;
            particle->speed = speed;
        }
    } else if (mode == 2) {
        int frame = D_800E27EC - 1;
        int sample = *(short *)((char *)D_800966EC + (((unsigned int)frame << 8) & 0x3F00));
        int size, kind, palette;
        register int specialKind asm("$3") = 4;
        unsigned short clut;
        kind = D_800F336C;
        size = sample * 2;
        asm("" : : "r"(size) : "memory");
        palette = D_800E1204[kind];
        if (kind == specialKind && D_800F3428) palette += 9;
        else palette += 5;
        clut = GetClut(0, palette);
        func_800CEE20(particle, 0, size, size, 102, clut, 1,
            (short)D_800966EC[(((unsigned int)frame << 9) & 0x3E00) / 4] >> 6, &color);
    }
    return 0;
}

#include "room_m273_effects.h"

#define ROOMLIB_POLL_RESET_POOL_CONTEXT_DECL
#define ROOMLIB_POLL_RESET_POOL_EXPR D_800F33E0->pool
#define ROOMLIB_POLL_RESET_FUNC func_80195F78
#define ROOMLIB_POLL_RESET_CALLBACK func_80195E10
#define ROOMLIB_POLL_RESET_FLAG D_8019AEF8
#define ROOMLIB_POLL_RESET_COUNTER D_8019AEB2
#define ROOMLIB_POLL_RESET_SEED D_8019AEAC
#include "../room_lib/RoomLib_PollAndResetActor.inc"

#include "room_m273_effects.h"
typedef GteShortVector Vector;
typedef RoomM273EffectModeState State;
extern RoomM273EffectStateContext *D_800F32D0;
extern unsigned char D_8019AEF8;
extern Vector D_8019AE9C[];
extern void *D_8019AE80;
extern unsigned short D_800E11FA,D_800E2850[];
extern unsigned short D_800F3368,D_800F336A,D_800F336C,D_800F336E;
extern unsigned short D_800F3370,D_800F3372,D_800F3374;
extern unsigned short D_800F3376,D_800F3378;
extern int func_8019A398(),func_8019A4CC();
extern int func_800CE560(void *,int,int,int (*)());
extern int func_800CE5AC(void **,int,int,int,int (*)());
extern void *func_800CE610(void *);
extern void func_800CE688(void *),func_800CE78C(void *);

int func_801960F4(int mode) {
    /* Layout only: purpose of these eight original frame bytes is unknown. */
    int unknownFrame[2];
    switch(mode) {
    case 0: {
        int size=func_800CE560(D_800F33E0->pool,4,6,func_8019A398);
        asm("" : : "r"(size));
        return size+func_800CE5AC(&D_8019AE80,size,4,6,func_8019A4CC);
    }
    case 1: {
        unsigned char *stopped=&D_8019AEF8;
        State *state;
        if(*stopped) return 2;
        state=D_800F32D0->state.mode;
        if(state->kind==11) {
            int count=state->value22;
            unsigned short value=*(volatile unsigned short *)&state->value22;
            if(count<35 && (value&7)==0) {
                short i=0;
                /* The shared positions start 0x5C bytes before the stop symbol. */
                Vector *positions=(Vector *)(stopped-92);
                do {
                    Vector **out=func_800CE610(D_800F33E0->pool);
                    unsigned long address;
                    if(!out) break;
                    address=i*8;
                    address+=(unsigned long)positions;
                    *out=(Vector *)address;
                    i++;
                } while(i<2);
                i=0;
                positions=D_8019AE9C;
                do {
                    Vector **out=func_800CE610(D_8019AE80);
                    unsigned long address;
                    if(!out) break;
                    address=i*8;
                    address+=(unsigned long)positions;
                    *out=(Vector *)address;
                    i++;
                } while(i<2);
            }
            func_800CE688(D_8019AE80);
        }
        break;
    }
    case 2: {
        unsigned int index;
        unsigned short palette;
        func_800CE78C(D_8019AE80);
        index=D_800E11FA;
        D_800F3368=32; D_800F336A=2;
        D_800F3376=32; D_800F3378=32;
        D_800F3376=32; D_800F3378=32;
        palette=D_800E2850[index];
        asm("" : : : "memory", "$2");
        D_800F336C=3; D_800F336E=1; D_800F3372=0; D_800F3374=0;
        D_800F3370=palette;
        break;
    }
    }
    return 0;
}
