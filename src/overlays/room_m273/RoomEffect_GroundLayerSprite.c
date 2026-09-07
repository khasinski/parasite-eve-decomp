typedef struct { short x,y,z,pad; } Vector;
extern int D_800E27EC,D_800F3428;
extern short D_800966EE[];
extern unsigned short D_800F336C,D_800E1204[],D_800942EC;
extern Vector D_8019AB68;
extern unsigned char D_8019AE18[],D_8019AB70[];
extern unsigned short GetClut(int,int);
extern void func_800CEE20(Vector *,void *,int,int,int,int,int,int,void *);
extern void func_800D004C(Vector *,int,int,int,Vector *,int,int,void *,void *,int,int);

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
        register int specialKind asm("$3");
        unsigned short clut;
        position=*input;
        kind=D_800F336C;
        shade=(unsigned int)(shadeSample+4096)>>5;
        size=sizeSample*2+4096;
        specialKind=4;
        asm("" : : "r"(shade), "r"(size) : "memory");
        palette=D_800E1204[kind];
        if(kind==specialKind && D_800F3428) palette+=8;
        else palette+=4;
        clut=GetClut(0,palette);
        func_800CEE20(&position,0,(short)size,(short)size,100,clut,1,(short)shade,0);
        position.y=D_800942EC;
        func_800D004C(&position,384,384,8,&D_8019AB68,4096,4096,
            D_8019AE18,D_8019AB70,(short)shade,1);
    }
    return 0;
}
