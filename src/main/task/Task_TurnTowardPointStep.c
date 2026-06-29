/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned short u16;

extern int g_SceneDataTable0;
extern char *g_TaskNodePool;
extern char *g_CurrentEntity;

int Gte_Atan2(int arg0, int arg1);

#define LOAD_FIELD_STATE(reg)                                         \
    asm volatile(                                                     \
        "lui\t%0, %%hi(g_CurrentEntity)\n"                                 \
        "lw\t%0, %%lo(g_CurrentEntity)(%0)"                                \
        : "=r"(reg))

int Task_TurnTowardPointStep(int **arg0) {
    char *node = g_TaskNodePool;
    int flags = *(u16 *)(node + 8);
    int x;
    register int y asm("$4");
    int step;
    register int angle asm("$3");
    register int stepped asm("$6");
    int original;

    {
        int active = flags & 0x20;
        if (active != 0) {
            goto cached_args;
        }
    }
    {
        int new_flags;

        x = *arg0[0];
        y = *arg0[1];
        step = *arg0[2];
        new_flags = flags | 0x20;
        *(u16 *)(node + 8) = new_flags;
        *(int *)(node + 0x14) = x;
        *(int *)(node + 0x1C) = step;
        *(int *)(node + 0x18) = y;
        goto have_args;
    }

cached_args:
    {
        x = *(int *)(node + 0x14);
        y = *(int *)(node + 0x18);
        step = *(int *)(node + 0x1C);
    }

have_args:
    {
        char *state;
        register int dx asm("$6");
        int dy;
        register int tmp asm("$2");

        LOAD_FIELD_STATE(state);
        tmp = *(int *)(state + 0x28);
        dx = (tmp - x) >> 16;
        dy = (*(int *)(state + 0x30) - y) >> 16;
        if ((dx | dy) == 0) {
            return 1;
        }
        angle = 0x1400 - Gte_Atan2(dy, dx);
    }

    {
        char *state;
        LOAD_FIELD_STATE(state);
        angle &= 0xFFF;
        asm volatile("lh\t%0,0x3A(%1)" : "=r"(original) : "r"(state));
    }
    stepped = angle;
    if (angle == original) {
        goto finish;
    }

    {
        int current = original;
        register int desired asm("$3") = angle;
        register int out asm("$6") = stepped;
        int delta;

        if (current < desired) {
            delta = desired - current;
            if (delta < 0x800) {
                if (step < delta) {
                    out = current + step;
                }
            } else if (step < delta) {
                out = current - step;
                if (out < 0) {
                    int wrap = current + 0x1000;
                    wrap = wrap - desired;
                    if (wrap < step) {
                        out = desired;
                    }
                }
            }
        } else {
            delta = current - desired;
            if (delta < 0x800) {
                if (step < delta) {
                    out = current - step;
                }
            } else if (step < delta) {
                out = current + step;
                if (out >= 0x1001) {
                    int wrap = desired + 0x1000;
                    wrap = wrap - current;
                    if (wrap < step) {
                        out = desired;
                    }
                }
            }
        }
        stepped = out;
    }
    {
        char *state;
        LOAD_FIELD_STATE(state);
        stepped &= 0xFFF;
        *(u16 *)(state + 0x3A) = stepped;
    }
    if (stepped != angle) {
        int cursor = g_SceneDataTable0;
        char *active = g_TaskNodePool;
        cursor -= 0x14;
        g_SceneDataTable0 = cursor;
        *(int *)(active + 0x10) = 1;
        return 0;
    }

finish:
    {
        char *active = g_TaskNodePool;
        int f = *(u16 *)(active + 8);
        f &= 0xFFDF;
        *(u16 *)(active + 8) = f;
        return 1;
    }
}
