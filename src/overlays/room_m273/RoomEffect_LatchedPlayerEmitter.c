typedef struct { int x,y,z; } Position;
typedef struct { unsigned char unknown[20]; Position position; } Transform;
typedef struct { unsigned char unknown[14],kind,unknown15[0x229]; Transform *transform; } Owner;
typedef struct { unsigned char unknown[8]; void *pool; } Context;
typedef struct { Position *position; } Effect;
extern Context *D_800F33E0;
extern Owner *g_PlayerEntity;
extern int D_800E27EC;
extern unsigned char D_8019AE5C;
extern unsigned short D_800E11FA,D_800E2850[];
extern unsigned short D_800F3368,D_800F336A,D_800F336C,D_800F336E;
extern unsigned short D_800F3370,D_800F3372,D_800F3374;
extern volatile unsigned short D_800F3376,D_800F3378;
extern int func_80193870();
extern int func_800CE560(void *,int,int,int (*)());
extern Effect *func_800CE610(void *);

int func_801939B4(int mode) {
    switch(mode) {
    case 0:
        D_8019AE5C=0;
        return func_800CE560(D_800F33E0->pool,4,1,func_80193870);
    case 1:
        if(D_800E27EC>=20) return 2;
        if(D_8019AE5C) return 2;
        if(g_PlayerEntity->kind<4) {
            Effect *effect=func_800CE610(D_800F33E0->pool);
            if(effect) {
                Transform *transform=g_PlayerEntity->transform;
                D_8019AE5C=1;
                effect->position=&transform->position;
            }
        }
        break;
    case 2:
        if(D_8019AE5C) {
            int palette=D_800E2850[D_800E11FA];
            D_800F3368=32; D_800F336A=2;
            D_800F3376=32; D_800F3378=32; D_800F3376=32; D_800F3378=32;
            asm("" : : : "memory", "$2");
            D_800F336C=3; D_800F336E=1; D_800F3372=0; D_800F3374=0;
            D_800F3370=palette;
        }
        break;
    }
    return 0;
}
