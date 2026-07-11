typedef signed short s16;
typedef unsigned char u8;
typedef unsigned int u32;

#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U8_AT(ptr, off) (*(u8 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))

extern u8 *D_8009D20C;
extern u8 *D_8009D254;
extern u8 *D_8009D2F0;
extern u32 D_800B89F8[];

void Render_InitObjectFromTable(void *obj, void *owner, int index);
void Render_TransformSkinnedVertices(void *actor, u32 *view_matrix);

int Task_CopyEntityRenderObject(int **args) {
    u8 *src;

    if (*args[0] == 0) {
        src = D_8009D254;
        if (src == 0) {
            goto done;
        }
    } else {
        src = D_8009D20C;
        if (src == 0) {
            goto done;
        }

        do {
            if ((U8_AT(src, 0x0C) == *args[0]) &&
                (U8_AT(src, 0x0D) == *args[1]) &&
                ((U32_AT(src, 0x98) & 0x10) == 0)) {
                break;
            }

            src = *(u8 **)(src + 4);
        } while (src != 0);

        if (src == 0) {
            goto done;
        }
    }

    Render_InitObjectFromTable(D_8009D2F0 + 0x1B4, src + 0x1B4, (s16)*args[2]);
    Render_TransformSkinnedVertices(D_8009D2F0 + 0x1B4, D_800B89F8);

    *(u8 **)(D_8009D2F0 + 0x18C) = src;
    U32_AT(D_8009D2F0, 0x28) = S16_AT(D_8009D2F0, 0x254) << 16;
    U32_AT(D_8009D2F0, 0x2C) = S16_AT(D_8009D2F0, 0x256) << 16;
    U32_AT(D_8009D2F0, 0x30) = S16_AT(D_8009D2F0, 0x258) << 16;
    U32_AT(D_8009D2F0, 0x98) |= 0x2000;

done:
    return 1;
}
