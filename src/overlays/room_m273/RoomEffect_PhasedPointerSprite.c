typedef struct { void *position; } Effect;
typedef struct { unsigned char unknown[58]; unsigned short value; } Object;
typedef struct { int unknown[2]; Object *object; } Context;
typedef struct { short zero; unsigned short value,phase,one; } Parameters;
extern int D_800E27EC,D_800966EC[];
extern Context *D_800F32D0;
extern unsigned char D_8019ACDC[],D_8019ACE0[];
extern void func_800D0728(void *,int,int,int,Parameters *,int,int,void *,void *,int,int);

int func_8019A4CC(int mode,Effect *effect) {
    Parameters parameters;
    /* Layout only: purpose of these eight original frame bytes is unknown. */
    int stack_pad[2];
    if(mode==1) {
        if(D_800E27EC>=16) return 1;
    } else if(mode==2) {
        register int frame asm("$4")=D_800E27EC-1;
        unsigned int sizeOffset=((unsigned int)frame<<8)&0x3F00;
        unsigned int shadeOffset=((unsigned int)frame<<9)&0x3E00;
        int size=*(short *)((char *)D_800966EC+sizeOffset)*2+4096;
        int shade=(short)*(int *)((char *)D_800966EC+shadeOffset)>>5;
        parameters.zero=0;
        parameters.value=D_800F32D0->object->value;
        parameters.phase=(unsigned int)frame<<8;
        parameters.one=1;
        func_800D0728(effect->position,32,96,8,&parameters,size,size,
            D_8019ACDC,D_8019ACE0,shade,1);
    }
    return 0;
}
