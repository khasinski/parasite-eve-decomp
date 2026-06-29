typedef unsigned int u32;

typedef struct Unk800A76A0 {
    u32 flags;
    int current;
    int limit;
} Unk800A76A0;

extern u32 g_GameStateFlags;
extern Unk800A76A0 D_800A76A0[4];

int Gpu_CheckDrawStatus(void);

#define LAUNDER(x) asm volatile("" : "=r"(x) : "0"(x))

void Scene_TickTimers(void) {
    register Unk800A76A0 *entry asm("$5");
    int *current_p;
    u32 *flags_p;
    int *limit_p;
    unsigned int i;
    u32 flags;
    int direct;
    int current;

    if ((g_GameStateFlags & 0x41) != 0) {
        return;
    }
    if ((Gpu_CheckDrawStatus() << 24) != 0) {
        return;
    }

    i = 0;
    flags_p = &D_800A76A0[0].flags;
    entry = (Unk800A76A0 *)flags_p;
    limit_p = &entry->limit;
    current_p = &entry->current;

    do {
        flags = *flags_p;
        LAUNDER(flags);
        if ((flags & 1) != 0) {
            if ((flags & 4) == 0) {
                if ((flags & 2) != 0) {
                    if ((flags & 5) != 0) {
                        direct = *current_p;
                        entry->current = direct - 1;
                    } else {
                        current = *current_p;
                        if ((unsigned int)*limit_p < (unsigned int)current) {
                            direct = current - 1;
                            entry->current = direct;
                        } else {
                            direct = flags | 4;
                            *flags_p = direct;
                        }
                    }
                } else {
                    if ((flags & 5) != 0) {
                        direct = *current_p;
                        entry->current = direct + 1;
                    } else {
                        current = *current_p;
                        if ((unsigned int)current < (unsigned int)*limit_p) {
                            direct = current + 1;
                            entry->current = direct;
                        } else {
                            direct = flags | 4;
                            *flags_p = direct;
                        }
                    }
                }
            }
        }
        flags_p = (u32 *)((char *)flags_p + 0xC);
        entry++;
        limit_p = (int *)((char *)limit_p + 0xC);
        i++;
        current_p = (int *)((char *)current_p + 0xC);
    } while (i < 4);
}
