typedef struct { short x,y,z,w; } Vector;
typedef struct { unsigned char unknown[8]; void *pool; } Context;
typedef struct { unsigned char unknown[14],kind,unknown15[7]; short timer; } State;
typedef struct { unsigned char unknown[8]; State *state; } StateContext;
extern Context *D_800F33E0;
extern StateContext *D_800F32D0;
extern unsigned char D_8019AEF8;
extern Vector D_8019AE9C[];
extern void *D_8019AE80;
extern unsigned short D_800E11FA,D_800E2850[];
extern unsigned short D_800F3368,D_800F336A,D_800F336C,D_800F336E;
extern unsigned short D_800F3370,D_800F3372,D_800F3374;
extern volatile unsigned short D_800F3376,D_800F3378;
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
        state=D_800F32D0->state;
        if(state->kind==11) {
            int count=state->timer;
            unsigned short value=*(volatile unsigned short *)&state->timer;
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
