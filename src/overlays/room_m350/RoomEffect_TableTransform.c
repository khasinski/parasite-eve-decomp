typedef struct { short position[3]; short size; } Particle;
typedef struct { short rotation[3][3]; int position[3]; } Transform;
typedef struct { char reserved[22]; unsigned short frame; char reserved18[0x220]; Transform *transforms; } Instance;
typedef struct { int reserved[2]; Instance *instance; } Actor;
typedef struct { int reserved[2]; void *pool; } Emitter;
typedef struct { unsigned char index, alternate; } Entry;
extern Actor *D_800F32D0;
extern Instance *D_8019A7F8;
extern Emitter *D_800F33E0;
extern unsigned char D_8019A804;
extern Entry D_8019A4D4[];
extern int D_800E27EC, D_800F3428, D_800966EC[];
extern volatile short D_800F3368,D_800F3376,D_800F3378,D_800F3372,D_800F3374,D_800F336E;
extern short D_800F336A;
extern unsigned short D_800F336C, D_800E1204[];
extern volatile unsigned short D_800E11FA,D_800F3370;
extern unsigned short D_800E2850[];
extern unsigned char D_8019A4EC[];
extern int D_8019A3D0[];
extern int GetClut(int, int);
extern void func_800CEE20(void *, int, int, int, int, unsigned int, int, int, void *);
extern int func_800CE560(void *,int,int,int (*)(int,Particle *));
extern Particle *func_800CE610(void *);

int func_80195564(int event, short *position) {
    if (event == 1) {
        if (D_800E27EC >= 8) return 1;
    } else if (event == 2) {
        int kind = D_800F336C;
        register int specialKind asm("$4");
        int texture;
        int handle;
        asm("" : "=r"(kind) : "0"(kind));
        specialKind = 4;
        texture = D_800E1204[kind];
        if (kind == specialKind && D_800F3428) texture += 8;
        else texture += 4;
        handle = GetClut(0, texture);
        func_800CEE20(position, 0, position[3], position[3],
            D_800F336A * D_8019A4EC[(D_800E27EC - 1) / 2] + 128,
            (unsigned short)handle, 1,
            (short)D_800966EC[((unsigned int)(D_800E27EC - 1) << 7) & 0xF80] >> 5,
            D_8019A3D0);
    }
    return 0;
}

int func_8019569C(int event) {
    if (event==1) goto update;
    if (event<2) { if(event==0) goto setup; goto done; }
    if(event==2) goto configure;
    goto done;
setup:
    return func_800CE560(D_800F33E0->pool,8,10,func_80195564);
update:
    if(D_8019A804) return 2;
    {
        register int frame asm("$3")=D_800F32D0->instance->frame;
        unsigned int relative=frame-7;
        int i=0;
        int frameOffset;
        int *size;
        if(relative>=6) return 0;
        asm("" : "=r"(frame) : "0"(frame), "r"(relative));
        frameOffset=frame*4;
        {
            unsigned int offset=relative<<9;
            int *base=D_800966EC;
            size=(int *)((char *)base+(offset&0x3E00));
        }
        for(;i<2;i++) {
            register int entryOffset asm("$2")=i*2;
            Entry *table=D_8019A4D4;
            Entry *entryBase;
            Entry *entry;
            Particle *particle;
            Instance *instance;
            volatile int *source;
            short *destination;
            int j;
            entryBase=(Entry *)((char *)table+entryOffset);
            entry=(Entry *)((unsigned long)frameOffset+(unsigned long)entryBase);
            if(!entry->index) return 0;
            particle=func_800CE610(D_800F33E0->pool);
            if(!particle) return 0;
            j=0;
            if(entry->alternate) instance=D_8019A7F8;
            else instance=D_800F32D0->instance;
            asm volatile("" : : "r"(instance) : "memory");
            source=instance->transforms[entry->index].position;
            destination=particle->position;
            do { *destination++=*source++; j++; } while(j<3);
            particle->size=*size+2048;
        }
    }
    goto done;
configure:
    {
        int index=D_800E11FA;
        int palette;
        D_800F3368=32; D_800F336A=2;
        D_800F3376=32; D_800F3378=32;
        D_800F3376=32; D_800F3378=32;
        palette=D_800E2850[index];
        asm volatile("" : "=r"(palette) : "0"(palette) : "memory");
        D_800F336C=3; D_800F336E=1;
        D_800F3372=0; D_800F3374=0; D_800F3370=palette;
    }
done:
    return 0;
}
