typedef struct { short x,y,z,pad; } Vector;
extern int D_800E27EC,D_800F3428,D_800966EC[];
extern unsigned short D_800F336C,D_800E1204[],D_800942EC;
extern unsigned char D_8019AB70[],D_8019AD74[],D_8019AD78[];
extern unsigned short GetClut(int,int);
extern void func_800CEE20(Vector *,Vector *,int,int,int,int,int,int,void *);
extern void func_800D004C(Vector *,int,int,int,Vector *,int,int,void *,void *,int,int);

int func_801977F8(int mode,Vector *input) {
    Vector rotation,position;
    if(mode==1) {
        if(D_800E27EC>=16) return 1;
    } else if(mode==2) {
        register int counter asm("$2")=D_800E27EC;
        register int frame asm("$22")=counter-1;
        int kind=D_800F336C;
        register int sample asm("$3")=*(int *)((char *)D_800966EC+(((unsigned int)frame<<8)&0x3F00));
        int shade,size,palette;
        unsigned short clut;
        rotation.x=0; rotation.y=0; rotation.z=(unsigned int)frame<<8; rotation.pad=0;
        position=*input;
        palette=D_800E1204[kind];
        shade=sample>>21;
        size=(short)sample*3+2048;
        if(kind==4 && D_800F3428) palette+=4;
        clut=GetClut(80,palette);
        func_800CEE20(&position,&rotation,size,size,74,clut,1,shade,0);
        rotation.z>>=1;
        sample=*(int *)((char *)D_800966EC+(((unsigned int)frame<<8)&0x3F00));
        shade=sample>>21;
        size=(short)sample+1024;
        func_800D004C(&position,512,512,16,&rotation,size,size,D_8019AB70,D_8019AD74,shade,1);
        rotation.x=1024; rotation.y=0; rotation.z=(unsigned int)frame<<8; rotation.pad=1;
        position.y=D_800942EC;
        asm("" : : "r"(frame) : "memory");
        func_800D004C(&position,768,768,8,&rotation,4096,4096,D_8019AD78,D_8019AB70,shade,1);
    }
    return 0;
}
