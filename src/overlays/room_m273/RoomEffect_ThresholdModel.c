typedef struct { short x,y,z,phase,scaleXZ,scaleY; unsigned short parameter12,unknown14; } Effect;
typedef struct { short m[3][3],pad; int t[3]; } Matrix;
typedef struct { int x,y,z,pad; } Vector;
extern int D_800E27EC,D_800F3428,D_800966EC[];
extern unsigned short D_800E11FA,D_800E2850[],D_800F336C,D_800E1204[];
extern void *D_8019AE8C;
extern int GetTPage(int,int,int,int),rcos(int),rsin(int);
extern unsigned short GetClut(int,int);
extern void GsSetOrign(int,int),func_800C6ED8(int);
extern void func_800C6EF8(void *),func_800C6FA0(void *,int);
extern void *memset(void *,int,unsigned long);
extern void Gte_ScaleMatrix(Matrix *,Vector *),func_800C71E4(void *,Matrix *),func_800C6F4C(void *);

int func_80193CB8(int mode,Effect *effect) {
    Matrix matrix;
    Vector copy,scale;
    if(mode==1) {
        int frame=D_800E27EC;
        int *wave;
        register int *shifted asm("$4");
        register int *table asm("$6");
        unsigned int index;
        if(frame>=32) return 1;
        frame*=32;
        index=frame&0xFFF;
        asm("" : "=r"(index) : "0"(index) : "$6");
        table=D_800966EC;
        wave=&table[index];
        shifted=&table[(frame+1024)&0xFFF];
        {
            register int sample asm("$2")=*wave;
            effect->scaleXZ=(short)sample/2;
        }
        effect->scaleY=(((short *)shifted)[1]+4096)*2;
        asm("" : : : "memory");
        {
            unsigned int phase=(unsigned short)effect->phase;
            int parameter=((short *)wave)[1];
            asm("" : "=r"(phase), "=r"(parameter) : "0"(phase), "1"(parameter));
            effect->phase=phase+256;
            effect->parameter12=parameter>>6;
        }
    } else if(mode==2) {
        int flags=GetTPage(0,1,0,0);
        int page=(unsigned short)(D_800E2850[D_800E11FA]|flags);
        int kind=D_800F336C,palette;
        int cosine,sine;
        register void **asset asm("$17");
        register int specialKind asm("$3")=4;
        asm("" : : "r"(page) : "memory");
        palette=D_800E1204[kind];
        if(kind==specialKind && D_800F3428) palette+=11;
        else palette+=7;
        GsSetOrign(page,GetClut(0,palette));
        func_800C6ED8(1);
        asset=&D_8019AE8C;
        func_800C6EF8(*asset);
        func_800C6FA0(*asset,effect->parameter12);
        cosine=rcos(effect->phase);
        sine=rsin(effect->phase);
        matrix.m[0][2]=sine; matrix.m[2][0]=-sine;
        matrix.m[0][0]=cosine; matrix.m[2][2]=cosine;
        matrix.t[0]=matrix.t[1]=matrix.t[2]=0;
        matrix.m[0][1]=matrix.m[1][0]=matrix.m[1][2]=matrix.m[2][1]=0;
        matrix.m[1][1]=4096;
        memset(&scale,0,16);
        scale.x=effect->scaleXZ; scale.y=effect->scaleY; scale.z=effect->scaleXZ;
        copy=scale;
        Gte_ScaleMatrix(&matrix,&copy);
        {
            int x=effect->x;
            void *model=*asset;
            matrix.t[0]=x; matrix.t[1]=effect->y; matrix.t[2]=effect->z;
            func_800C71E4(model,&matrix);
        }
        func_800C6F4C(*asset);
    }
    return 0;
}
