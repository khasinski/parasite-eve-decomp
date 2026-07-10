typedef signed short s16;
typedef unsigned short u16;
typedef unsigned int u32;

#define U16(base, off) (*(u16 *)((char *)(base) + (off)))
#define S16(base, off) (*(s16 *)((char *)(base) + (off)))
#define U32(base, off) (*(u32 *)((char *)(base) + (off)))
#define ADVANCE_ENV_PTR(voice, off)                           \
    do {                                                     \
        table = (s16 *)U32((voice), (off));                   \
        if (table[0] == 0 && table[1] == 0) {                 \
            table += table[2];                                \
            U32((voice), (off)) = (u32)table;                 \
        }                                                     \
        sample = *table++;                                    \
        U32((voice), (off)) = (u32)table;                     \
    } while (0)
#define MIX_PAN(out_l, out_r, volume, pan_value, mask_value)          \
    do {                                                             \
        switch (D_8009D2C0) {                                        \
        case 1:                                                      \
            (out_l) = ((volume) * D_8009B8F8[(pan_value)]) >> 15;     \
            (out_r) = ((volume) * D_8009B8F8[(pan_value) ^ 0xFF]) >> 15; \
            break;                                                   \
        case 4:                                                      \
            (out_l) = ((volume) * D_8009B8F8[(pan_value)]) >> 15;     \
            (out_r) = ((volume) * D_8009B8F8[(pan_value) ^ 0xFF]) >> 15; \
            if (((mask_value) & 0x00AAAAAA) != 0) {                  \
                (out_r) = ~(out_r);                                  \
            } else {                                                 \
                (out_l) = ~(out_l);                                  \
            }                                                        \
            break;                                                   \
        default:                                                     \
            (out_r) = (u32)((volume) * D_8009B9F8[0]) >> 15;         \
            (out_l) = (out_r);                                       \
            break;                                                   \
        }                                                           \
    } while (0)
#define SCALE_GLOBAL_DEPTH(value)                                    \
    do {                                                            \
        int depth = D_8009D2CE;                                      \
        if (depth != 0) {                                            \
            if (depth < 0x80) {                                      \
                (value) += ((value) * depth) >> 7;                   \
            } else {                                                 \
                (value) = ((value) * depth) >> 8;                    \
            }                                                        \
        }                                                           \
    } while (0)

extern int D_8009D2C0;
extern unsigned char D_8009D2CE;
extern void *D_8009D2C8;
extern s16 D_8009B8F8[];
extern s16 D_8009B9F8[];
extern char D_800B8AC0[];

void Akao_SetVoiceKeyOff(void *voice, u32 voice_mask) {
    u32 flags;
    int base_volume;
    int value;
    int sample;
    int pan;
    int left;
    int right;
    void *target_voice;
    s16 *table;

    base_volume = (S16(voice, 0x46) * (U16(voice, 0x6C) >> 8)) >> 7;
    flags = U32(voice, 0x38);

    if ((flags & 1) != 0 && U16(voice, 0x8A) == 0) {
        U16(voice, 0x8E)--;
        if (U16(voice, 0x8E) == 0) {
            U16(voice, 0x8E) = U16(voice, 0x8C);
            ADVANCE_ENV_PTR(voice, 0x1C);
            value = (U16(voice, 0x92) * sample) >> 16;
            if (value != S16(voice, 0xE8)) {
                U16(voice, 0xE8) = value;
                U32(voice, 0xF4) |= 0x10;
                if (value >= 0) {
                    U16(voice, 0xE8) = value << 1;
                }
            }
        }
    }

    if ((flags & 2) != 0 && U16(voice, 0x9E) == 0) {
        U16(voice, 0xA2)--;
        if (U16(voice, 0xA2) == 0) {
            U16(voice, 0xA2) = U16(voice, 0xA0);
            ADVANCE_ENV_PTR(voice, 0x20);
            value = base_volume * (U16(voice, 0xA6) >> 8);
            value = ((value << 9) >> 16) * sample;
            value >>= 15;
            if (value != S16(voice, 0xEA)) {
                U16(voice, 0xEA) = value;
                U32(voice, 0xF4) |= 3;
            }
        }
    }

    if ((flags & 4) != 0) {
        U16(voice, 0xB0)--;
        if (U16(voice, 0xB0) == 0) {
            U16(voice, 0xB0) = U16(voice, 0xAE);
            ADVANCE_ENV_PTR(voice, 0x24);
            value = ((U16(voice, 0xB4) >> 8) * sample) >> 15;
            if (value != S16(voice, 0xEC)) {
                U16(voice, 0xEC) = value;
                U32(voice, 0xF4) |= 3;
            }
        }
    }

    if ((flags & 0x20) != 0) {
        base_volume = (((int)(U16(voice, -0x10) << 17) >> 16) *
                       (U16(voice, 0x6C) >> 8)) >> 7;
        U32(voice, 0xF4) |= 3;
    }

    if ((U32(voice, 0xF4) & 3) != 0) {
        base_volume += S16(voice, 0xEA);
        base_volume = (base_volume * (U16(D_8009D2C8, 0x4A) & 0x7F)) >> 7;
        pan = ((U16(voice, 0x76) >> 8) + S16(voice, 0xEC)) & 0xFF;
        MIX_PAN(left, right, base_volume, pan, voice_mask);
        S16(voice, 0x118) = left;
        S16(voice, 0x11A) = right;

        if ((flags & 0x800) != 0) {
            target_voice = D_800B8AC0 + U16(voice, 0x5C) * 0x11C;
            S16(target_voice, 0x118) = U16(voice, 0x118);
            S16(target_voice, 0x11A) = U16(voice, 0x11A);

            base_volume = U16(voice, 0x5E) >> 1;
            MIX_PAN(left, right, base_volume, pan, voice_mask);
            S16(voice, 0x118) = left;
            S16(voice, 0x11A) = right;
            U16(target_voice, 0x118) -= U16(voice, 0x118);
            U16(target_voice, 0x11A) -= U16(voice, 0x11A);
        }
    }

    if ((flags & 0x10) != 0) {
        value = U16(voice, -0x10) + S16(voice, 0xE8) + S16(voice, 0x36);
        SCALE_GLOBAL_DEPTH(value);
        U16(voice, 0x10C) = value & 0x3FFF;
        U32(voice, 0xF4) |= 0x10;
    } else if ((U32(voice, 0xF4) & 0x10) != 0) {
        value = U32(voice, 0x30) + S16(voice, 0xE8) + S16(voice, 0x36);
        SCALE_GLOBAL_DEPTH(value);
        U16(voice, 0x10C) = value & 0x3FFF;
    }
}
