/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef signed short s16;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned char u8;

#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))

extern char *D_8009D2F0;
extern char *D_8009D254;
extern char D_800B0CEC[];
extern u16 D_800B0D88;
extern u32 D_8009D2E8;
extern int D_8009CE00;
extern char *D_8009D300;

void Anim_SetInterpRate(void *anim, int rate);

int Task_PlayEntityAnim(int **args) {
    char *task;
    u16 flags;

    task = D_8009D300;
    flags = U16_AT(task, 8);

    if ((flags & 0x20) != 0) {
        if ((U16_AT(D_8009D2F0, 0x250) & 4) != 0) {
            goto yield;
        }

        U16_AT(task, 8) = flags & 0xFFDF;
        return 1;
    }

    U32_AT(D_8009D2F0, 0x98) &= ~0x40;
    U16_AT(task, 8) = U16_AT(task, 8) | 0x20;

    Anim_SetInterpRate(D_8009D2F0 + 0x1B4, (s16)*args[0]);
    U16_AT(D_8009D2F0, 0x250) |= 4;

    if (D_8009D2F0 == D_8009D254) {
        D_8009D2E8 &= ~2;
        Anim_SetInterpRate(D_800B0CEC, (s16)*args[0]);
        D_800B0D88 |= 4;
    }

yield:
    D_8009CE00 -= 0x0C;
    U32_AT(D_8009D300, 0x10) = 1;
    return 0;
}
