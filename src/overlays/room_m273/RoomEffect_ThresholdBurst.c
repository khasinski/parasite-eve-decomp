typedef struct { unsigned char unknown[8]; void *pool; } Context;
typedef struct {
    unsigned char unknown[14],kind,unknown15[7];
    short value22; unsigned char unknown24[2]; unsigned short value26;
} State;
typedef struct { unsigned char unknown[8]; State *state; } StateContext;
typedef struct { int unknown[5]; int x,y,z; } Transform;
typedef struct { unsigned char unknown[0x238]; Transform *transform; } Owner;
typedef struct { short x,y,z,pad; } Vector;
extern Context *D_800F33E0;
extern StateContext *D_800F32D0;
extern Owner *g_PlayerEntity;
extern unsigned char D_8019AE9A;
extern short D_8019AE60,D_8019AE64;
extern unsigned short D_800E11FA,D_800E2850[];
extern unsigned short D_800F3368,D_800F336A,D_800F336C,D_800F336E;
extern unsigned short D_800F3370,D_800F3372,D_800F3374;
extern volatile unsigned short D_800F3376,D_800F3378;
extern int func_80194470();
extern int func_800CE560(void *,int,int,int (*)());
extern Vector *func_800CE610(void *);

int func_801945A8(int mode) {
    switch(mode) {
    case 0:
        D_8019AE60=0; D_8019AE64=0;
        return func_800CE560(D_800F33E0->pool,8,4,func_80194470);
    case 1: {
        State *state;
        if(D_8019AE9A) return 2;
        state=D_800F32D0->state;
        if(state->kind==9) {
            unsigned short value=state->value26;
            if(state->value22>=2 && (short)value<2) {
                D_8019AE60=4; D_8019AE64=0;
            }
        }
        if(D_8019AE60) {
            short old=D_8019AE64--;
            if(old<=0) {
                Vector *position=func_800CE610(D_800F33E0->pool);
                if(position) {
                    Transform *transform=g_PlayerEntity->transform;
                    position->x=transform->x;
                    position->y=transform->y;
                    position->z=transform->z;
                    D_8019AE64=2;
                    D_8019AE60--;
                }
            }
        }
        break;
    }
    case 2: {
        int palette=D_800E2850[D_800E11FA];
        D_800F3368=32; D_800F336A=2;
        D_800F3376=32; D_800F3378=32; D_800F3376=32; D_800F3378=32;
        asm("" : : : "memory", "$2");
        D_800F336C=3; D_800F336E=1; D_800F3372=0; D_800F3374=32;
        D_800F3370=palette;
        break;
    }
    }
    return 0;
}
