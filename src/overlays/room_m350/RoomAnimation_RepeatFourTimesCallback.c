typedef struct {
    char reserved[24];
    unsigned char *state;
} Actor;
typedef struct {
    Actor *actor;
    char reserved04[10];
    unsigned char animation;
    unsigned char frameCount;
    int reserved10;
    union { int word; short halves[2]; } frame;
} Animation;
extern Animation **D_800F32D0;
extern short D_8019A828;
extern unsigned char D_8019A82E, D_8019A82F, D_8019A830;

int func_80196E04(int event) {
    Animation *animation = D_800F32D0[2];
    if (event == 0) {
        D_8019A828 = 4;
        D_8019A82E = 0;
        D_8019A82F = 0;
        D_8019A830 = 0;
    } else if (event == 1) {
        register short frame;
        if (animation->actor) {
            unsigned char *state = animation->actor->state;
            if (*state == 1) *state = 2;
        }
        frame = animation->frame.halves[1];
        if (animation->animation == 9) {
            short *remaining = &D_8019A828;
            if (*remaining > 0) {
                if (frame >= 22) {
                    animation->frame.word = 0x30000;
                    *remaining = *(volatile unsigned short *)remaining - 1;
                }
            } else if (frame >= animation->frameCount - 1) {
                if (animation->actor) *animation->actor->state = 4;
                D_8019A82E = 1;
                return 1;
            }
        }
    }
    return 0;
}
