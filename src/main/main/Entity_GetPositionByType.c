typedef unsigned char u8;

extern void *D_8009D254;
extern void *D_8009D20C;

#define PTR_AT(ptr, off) (*(void **)((u8 *)(ptr) + (off)))
#define U8_AT(ptr, off) (*(u8 *)((u8 *)(ptr) + (off)))
#define S16_AT(ptr, off) (*(short *)((u8 *)(ptr) + (off)))
#define S32_AT(ptr, off) (*(int *)((u8 *)(ptr) + (off)))

int Entity_GetPositionByType(void *cmd) {
    void *entity;
    int type;
    int variant;
    int mode;

    type = S32_AT(PTR_AT(cmd, 4), 0);
    if (type == 0) {
        entity = D_8009D254;
        if (entity == 0) {
            S32_AT(PTR_AT(cmd, 0x18), 0) = -1;
            return 1;
        }
    } else {
        variant = S32_AT(PTR_AT(cmd, 8), 0);
        entity = D_8009D20C;
        while (entity != 0) {
            if (U8_AT(entity, 0xC) == type &&
                U8_AT(entity, 0xD) == variant &&
                ((S32_AT(entity, 0x98) & 0x10) == 0)) {
                break;
            }
            entity = PTR_AT(entity, 4);
        }

        if (entity == 0) {
            S32_AT(PTR_AT(cmd, 0x18), 0) = -1;
            return 1;
        }
    }

    S32_AT(PTR_AT(cmd, 0x18), 0) = 1;
    mode = S32_AT(PTR_AT(cmd, 0), 0);
    switch (mode) {
        case 0:
            S32_AT(PTR_AT(cmd, 0xC), 0) = S32_AT(entity, 0x28);
            S32_AT(PTR_AT(cmd, 0x10), 0) = S32_AT(entity, 0x2C);
            S32_AT(PTR_AT(cmd, 0x14), 0) = S32_AT(entity, 0x30);
            break;
        case 1:
            S32_AT(PTR_AT(cmd, 0xC), 0) = S32_AT(entity, 0x40);
            S32_AT(PTR_AT(cmd, 0x10), 0) = S32_AT(entity, 0x44);
            S32_AT(PTR_AT(cmd, 0x14), 0) = S32_AT(entity, 0x48);
            break;
        case 2:
            S32_AT(PTR_AT(cmd, 0xC), 0) = S32_AT(entity, 0x68);
            S32_AT(PTR_AT(cmd, 0x10), 0) = S32_AT(entity, 0x6C);
            S32_AT(PTR_AT(cmd, 0x14), 0) = S32_AT(entity, 0x70);
            break;
        case 3:
            S32_AT(PTR_AT(cmd, 0xC), 0) = S32_AT(entity, 0x78);
            S32_AT(PTR_AT(cmd, 0x10), 0) = S32_AT(entity, 0x7C);
            S32_AT(PTR_AT(cmd, 0x14), 0) = S32_AT(entity, 0x80);
            break;
        case 4:
            S32_AT(PTR_AT(cmd, 0xC), 0) = S32_AT(entity, 0x88);
            S32_AT(PTR_AT(cmd, 0x10), 0) = S32_AT(entity, 0x8C);
            S32_AT(PTR_AT(cmd, 0x14), 0) = S32_AT(entity, 0x90);
            break;
        case 5:
            S32_AT(PTR_AT(cmd, 0xC), 0) = S16_AT(entity, 0x38);
            S32_AT(PTR_AT(cmd, 0x10), 0) = S16_AT(entity, 0x3A);
            S32_AT(PTR_AT(cmd, 0x14), 0) = S16_AT(entity, 0x3C);
            break;
        case 6:
            S32_AT(PTR_AT(cmd, 0xC), 0) = S32_AT(entity, 0x58);
            S32_AT(PTR_AT(cmd, 0x10), 0) = S32_AT(entity, 0x5C);
            S32_AT(PTR_AT(cmd, 0x14), 0) = S32_AT(entity, 0x60);
            break;
    }

    return 1;
}
