/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 --stack-return-delay */

typedef int s32;

s32 VSync(s32 mode);
void func_80081268(void);

extern struct { char _[32]; } D_8009B6B0_o __asm__("D_8009B6B0");
#define D_8009B6B0 ((char *)&D_8009B6B0_o)
extern struct { char _[16]; } D_8009B6D0_o __asm__("g_CdReadCompleteCallback");
#define g_CdReadCompleteCallback (*(void (**)(s32, s32))&D_8009B6D0_o)

#define WORD(ptr, ofs) (*(s32 *)((ptr) + (ofs)))

void Render_StepParticleCallback(void) {
    char *state;
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

    func_80081268();
    callback = g_CdReadCompleteCallback;
    if (callback != 0) {
        mode = 2;
        if (WORD(state, 4) < 0) {
            mode = 5;
        }
        callback(mode, WORD(state, 0xC));
    }

done:
}
