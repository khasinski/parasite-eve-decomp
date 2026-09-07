typedef struct { short x, y, z, pad; } Vector;
typedef struct {
    Vector position;
    short controls[4];
    short brightness, size, amplitude, reserved;
} Particle;
typedef struct { char reserved[0x200]; int worldY; } Instance;
typedef struct { int reserved[2]; Instance *instance; } Actor;
extern Actor *D_800F32D0;
extern int D_800E27EC, D_800F3428;
extern unsigned short D_800F336C, D_800E1204[];
extern int D_8019A738[], D_8019A734[];
extern int rsin(int);
extern unsigned short GetClut(int, int);
extern void func_800CEE20(void *, void *, int, int, int, unsigned int, int, int, void *);
extern void func_800D004C(void *, int, int, int, void *, int, int, void *, void *, int, int);

int func_801996BC(int event, Particle *object)
{
    Vector position;
    Vector texture;
    if (event == 1) {
        int wave = rsin((unsigned int)D_800E27EC << 4);
        object->size = wave * object->amplitude >> 12;
        if (D_800E27EC >= 8) {
            object->brightness -= 8;
            if (object->brightness < 16) return 1;
        }
    } else if (event == 2) {
        int kind = D_800F336C;
        register int special asm("$4") = 4;
        register int index asm("$3") = kind * 2;
        int palette;
        unsigned short handle;
        palette = *(unsigned short *)((char *)D_800E1204 + index);
        if (kind == special && D_800F3428) palette += 7;
        else palette += 3;
        handle = GetClut(0, palette);
        func_800CEE20(object, object->controls, object->size, object->size,
            128, handle, 1, object->brightness, 0);
        position.x = object->position.x;
        position.y = D_800F32D0->instance->worldY;
        position.z = object->position.z;
        texture.x = 1024;
        texture.y = D_800E27EC * 192;
        texture.z = 0;
        texture.pad = 1;
        func_800D004C(&position, 384, 384, 10, &texture, object->size, object->size,
            D_8019A738, D_8019A734, object->brightness, 1);
    }
    return 0;
}
