/* MASPSX_FLAGS: --stack-return-delay */

typedef int s32;

s32 VSync(s32 mode);
void Save_ProcessDataCallback(void);

extern char D_8009B6B0[];
extern void (*D_8009B6D0)(s32, s32);

#define WORD(ptr, ofs) (*(s32 *)((ptr) + (ofs)))

void Render_StepParticleCallback(void) {
    register char *state asm("$16");
    s32 now;
    s32 limit;
    void (*callback)(s32, s32);
    register s32 mode asm("$4");

    state = D_8009B6B0;
    mode = -1;
    asm volatile("" : "=r"(state) : "0"(state));
    WORD(state, 0) += WORD(state, -4) << 2;
    WORD(state, 4) -= 1;
    now = VSync(mode);

    limit = WORD(state, 0x14) + 0x4B0;
    limit = limit < now;
    if (limit != 0) {
        WORD(state, 4) = -1;
    }

    if (WORD(state, 4) != 0) {
        now = VSync(-1);
        limit = WORD(state, 0x14) + 0x4B0;
        limit = limit < now;
        if (limit == 0) {
            goto done;
        }
    }

    Save_ProcessDataCallback();
    callback = D_8009B6D0;
    if (callback != 0) {
        mode = 2;
        if (WORD(state, 4) < 0) {
            mode = 5;
        }
        callback(mode, WORD(state, 0xC));
    }

done:
}
