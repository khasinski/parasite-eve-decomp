typedef struct { unsigned short x, y, z, speed; } Particle;
typedef struct { short x, y, z, pad; } Vector;
extern int D_800E27EC, D_800F3428;
extern unsigned short D_800F336C, D_800E1204[];
extern char D_8019948C[];
extern void func_800CF3AC(void *, void *, int);
extern unsigned short GetClut(int, int);
extern void func_800CEE20(Vector *, void *, int, int, int, int, int, int, void *);

int func_80192DA0(int mode, volatile Particle *particle) {
    int color[2];
    Vector position;
    switch (mode) {
    case 1: {
        int speed = particle->speed - 1;
        register int y asm("$2") = particle->y;
        register int delta asm("$5") = particle->speed;
        int frame = D_800E27EC;
        /* Preserve the retail read order and register lifetimes. */
        asm("" : : "r"(delta), "r"(frame), "r"(speed), "r"(y));
        ((Particle *)particle)->y = y - delta;
        ((Particle *)particle)->speed = speed;
        if (frame >= 24) return 1;
        break;
    }
    case 2: {
        int palette;
        int kind;
        register int specialKind asm("$3");
        int z;
        unsigned short clut;
        func_800CF3AC(D_8019948C, color, D_800E27EC);
        kind = D_800F336C;
        asm("" : : "r"(kind));
        position.x = particle->x;
        position.y = particle->y;
        z = particle->z;
        specialKind = 4;
        position.z = z;
        palette = D_800E1204[kind];
        if (kind == specialKind && D_800F3428) palette += 10;
        else palette += 6;
        clut = GetClut(0, palette);
        func_800CEE20(&position, 0, 2048, 2048, 6, clut, 1, 128, color);
        break;
    }
    }
    return 0;
}
