typedef struct { short x,y,z,phase; unsigned char unknown[8]; } Effect;
typedef struct { unsigned char unknown[0x594]; int x,y,z; } Transform;
typedef struct {
    unsigned char unknown[14],kind,unknown15[7];
    short value22; unsigned char unknown24[2]; unsigned short value26;
    unsigned char unknown28[0x21C]; Transform *transform;
} State;
typedef struct { unsigned char unknown[8]; State *state; } StateContext;
typedef struct { unsigned char unknown[8]; void *pool; } Context;
typedef struct { void *asset,*pool; } AssetState;
extern Context *D_800F33E0;
extern StateContext *D_800F32D0;
extern AssetState D_8019AE8C;
extern void *D_8019AE90,*D_800B0E64;
extern unsigned char D_8019AE9A;
extern unsigned short D_800942EC;
extern void *Asset_FindTable08ByU32Key(void *,unsigned int);
extern void func_800C6D5C(void *,int,int);
extern int func_80193CB8(),func_80193B5C();
extern int func_800CE560(void *,int,int,int (*)());
extern int func_800CE5AC(void **,int,int,int,int (*)());
extern void *func_800CE610(void *);
extern void func_800CE688(void *),func_800CE78C(void *);

int func_80193F30(int mode) {
    switch(mode) {
    case 0: {
        int size;
        D_8019AE8C.asset=Asset_FindTable08ByU32Key(D_800B0E64,0xC5941704);
        func_800C6D5C(D_8019AE8C.asset,0,0);
        size=func_800CE560(D_800F33E0->pool,16,6,func_80193CB8);
        asm("" : : "r"(size));
        return size+func_800CE5AC(&D_8019AE8C.pool,size,4,2,func_80193B5C);
    }
    case 1: {
        State *state;
        if(D_8019AE9A) return 2;
        state=D_800F32D0->state;
        if(state->kind==9) {
            unsigned short value=state->value26;
            if(state->value22>0 && (short)value<=0) {
                short i=0;
                do {
                    Effect *effect=func_800CE610(D_800F33E0->pool);
                    if(!effect) break;
                    effect->x=state->transform->x;
                    asm("" : : : "memory");
                    effect->y=D_800942EC;
                    effect->z=state->transform->z;
                    effect->phase=i*384;
                    if(i==0) *(Effect **)func_800CE610(D_8019AE90)=effect;
                    i++;
                } while(i<3);
            }
        }
        func_800CE688(D_8019AE90);
        break;
    }
    case 2:
        func_800CE78C(D_8019AE90);
        break;
    }
    return 0;
}
