typedef struct { short x,y,z,w; } Vector;
typedef struct { Vector position; unsigned char parameter[4]; } Effect;
extern int D_800E27EC,D_800966EC[];
extern unsigned short D_800942EC;
extern unsigned char D_8019AB70[];
extern void func_800D004C(Vector *,int,int,int,Vector *,int,int,void *,void *,int,int);

int func_8019353C(int mode,Effect *effect) {
    Vector position,rotation;
    /* Layout only: purpose of these eight original frame bytes is unknown. */
    int unknownFrame[2];
    if(mode==1) {
        if(D_800E27EC>=8) return 1;
    } else if(mode==2) {
        short frame=D_800E27EC-1;
        /* The input vector's fourth halfword supplies the initial scale. */
        int product=(*(short *)((char *)D_800966EC+(((unsigned int)frame<<9)&0x3E00))*2+4096)*effect->position.w;
        int sample=*(int *)((char *)D_800966EC+(((unsigned int)frame<<9)&0x3E00));
        short size=product/4096;
        short shade=sample>>21;
        short i;
        rotation.x=1024; rotation.y=0; rotation.z=frame*170; rotation.w=-1;
        position=effect->position;
        i=0;
        do {
            func_800D004C(&position,128,128,12,&rotation,size,size,D_8019AB70,effect->parameter,shade,1);
            shade>>=1;
            size=size*3/2;
            i++;
            position.y=D_800942EC;
        } while(i<2);
    }
    return 0;
}
