typedef unsigned int u32;

typedef struct {
    u32 flags;
} SceneE02RenderTarget;

typedef struct {
    SceneE02RenderTarget *target;
} SceneE02Link;

typedef struct {
    unsigned char state;
    char pad1[0x7];
    SceneE02Link *link;
} SceneE02Obj;

extern int func_800C6CE0(void);

int func_8018F368(SceneE02Obj *arg0) {
    arg0->state = 4;

    if ((u32)func_800C6CE0() >= 2) {
        SceneE02RenderTarget *target = arg0->link->target;

        target->flags &= 0xC0FFFFFF;
    }

    return 0;
}
