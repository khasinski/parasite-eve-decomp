#include "common.h"
#include "pe1/akao/voice_masks.h"
#define U16(base, off) (*(u16 *)((char *)(base) + (off)))
#define S16(base, off) (*(s16 *)((char *)(base) + (off)))
#define U32(base, off) (*(u32 *)((char *)(base) + (off)))
#define ADVANCE_ENV_PTR(voice, off)                                  \
    do {                                                            \
        if (table[0] == 0 && table[1] == 0) {                        \
            register s16 *next asm("$2") = table + table[2];        \
            U32((voice), (off)) = (u32)next;                         \
        }                                                           \
        asm volatile("" : : : "memory");                            \
        table = (s16 *)U32((voice), (off));                          \
        U32((voice), (off)) = (u32)(table + 1);                      \
        { register int raw asm("$4") = *table;                      \
          asm volatile("" : : "r"(raw)); sample = raw; }             \
    } while (0)
void Akao_SetVoiceKeyOn(AkaoTrack *voice, u32 voice_mask) {
    register u32 flags asm("$8");
    register int base_volume asm("$7");
    u32 later_flags;
    int value;
    int sample;
    int pan;
    register int mode asm("$4");
    s16 *table;

    base_volume = (S16(voice, 0x46) * (U16(voice, 0x6C) >> 8)) >> 7;
    flags = U32(voice, 0x38);

    if ((flags & 1) != 0) {
        U16(voice, 0x8E)--;
        if (U16(voice, 0x8E) == 0) {
            register int value asm("$3");
            table = (s16 *)U32(voice, 0x1C);
            U16(voice, 0x8E) = U16(voice, 0x8C);
            ADVANCE_ENV_PTR(voice, 0x1C);
            value = (U16(voice, 0x92) * sample) >> 16;
            if (value != S16(voice, 0xE8)) {
                U16(voice, 0xE8) = value;
                U32(voice, 0xF4) |= 0x10;
                if (value >= 0) {
                    register int doubled asm("$2") = value << 1;
                    U16(voice, 0xE8) = doubled;
                }
            }
        }
    }

    if ((flags & 2) != 0) {
        U16(voice, 0xA2)--;
        if (U16(voice, 0xA2) == 0) {
            int volume_product;
            table = (s16 *)U32(voice, 0x20);
            U16(voice, 0xA2) = U16(voice, 0xA0);
            if (table[0] == 0 && table[1] == 0) {
                register s16 *next asm("$2") = table + table[2];
                U32(voice, 0x20) = (u32)next;
            }
            asm volatile("" ::: "memory");
            volume_product = base_volume * (U16(voice, 0xA6) >> 8);
            table = (s16 *)U32(voice, 0x20);
            U32(voice, 0x20) = (u32)(table + 1);
            sample = *table;
            { register int shifted asm("$2") = volume_product << 9;
              register int upper asm("$3") = shifted >> 16;
              value = (upper * sample) >> 15; }
            { register int final_value asm("$3") = value;
              if (final_value != S16(voice, 0xEA)) {
                  U16(voice, 0xEA) = final_value;
                  U32(voice, 0xF4) |= 3;
              }
            }
        }
    }

    later_flags = flags;
    if ((later_flags & 4) != 0) {
        U16(voice, 0xB0)--;
        if (U16(voice, 0xB0) == 0) {
            register int value asm("$3");
            int scale;
            table = (s16 *)U32(voice, 0x24);
            U16(voice, 0xB0) = U16(voice, 0xAE);
            if (table[0] == 0 && table[1] == 0) {
                register s16 *next asm("$2") = table + table[2];
                U32(voice, 0x24) = (u32)next;
            }
            asm volatile("" ::: "memory");
            table = (s16 *)U32(voice, 0x24);
            U32(voice, 0x24) = (u32)(table + 1);
            scale = U16(voice, 0xB4) >> 8;
            { register int raw asm("$4") = *table; asm volatile("" : : "r"(raw)); sample = raw; }
            value = (scale * sample) >> 15;
            if (value != S16(voice, 0xEC)) {
                U16(voice, 0xEC) = value;
                U32(voice, 0xF4) |= 3;
            }
        }
    }

    if ((later_flags & 0x20) != 0) {
        base_volume = (((int)(U16(voice, -0x10) << 17) >> 16) *
                       (U16(voice, 0x6C) >> 8)) >> 7;
        U32(voice, 0xF4) |= 3;
    }

    if ((U32(voice, 0xF4) & 3) != 0) {
        base_volume += S16(voice, 0xEA);
        if ((U32(voice, 0x2C) & 0x02000000) != 0) {
            pan = 0x80;
        } else {
            base_volume = (base_volume * (s8)(U16(voice, 0xD8) >> 8)) >> 7;
            pan = ((U16(voice, 0x76) >> 8) + S16(voice, 0xEC)) & 0xFF;
        }

        mode = D_8009D2C0;
        switch (mode) {
        case 1:
            S16(voice, 0x118) = (base_volume * D_8009B8F8[pan]) >> 15;
            S16(voice, 0x11A) = (base_volume * D_8009B8F8[pan ^ 0xFF]) >> 15;
            break;
        case 4: {
            register int right asm("$3");
            S16(voice, 0x118) = (base_volume * D_8009B8F8[pan]) >> 15;
            right = (base_volume * D_8009B8F8[pan ^ 0xFF]) >> 15;
            S16(voice, 0x11A) = right;
            if ((voice_mask & 0x00AAAAAA) != 0) {
                register int flip asm("$2") = ~right;
                S16(voice, 0x11A) = flip;
            } else {
                S16(voice, 0x118) = ~U16(voice, 0x118);
            }
            break;
        }
        default:
            value = (S16(voice, 0x11A) =
                (u32)(base_volume * D_8009B9F8[0]) >> 15);
            S16(voice, 0x118) = value;
            break;
        }
    }

    if ((later_flags & 0x10) != 0) {
        value = U16(voice, -0x10) + S16(voice, 0xE8) + S16(voice, 0x36);
        if ((U32(voice, 0x2C) & 0x02000000) == 0) {
            int depth = *(u8 *)((char *)voice + 0x3D);
            if (depth != 0) {
                if (depth < 0x80) {
                    value += (value * depth) >> 7;
                } else {
                    value = (value * depth) >> 8;
                }
            }
        }
        U16(voice, 0x10C) = value & 0x3FFF;
        U32(voice, 0xF4) |= 0x10;
    } else if ((U32(voice, 0xF4) & 0x10) != 0) {
        value = U32(voice, 0x30) + S16(voice, 0xE8) + S16(voice, 0x36);
        if ((U32(voice, 0x2C) & 0x02000000) == 0) {
            int depth = *(u8 *)((char *)voice + 0x3D);
            if (depth != 0) {
                if (depth < 0x80) {
                    value += (value * depth) >> 7;
                } else {
                    value = (value * depth) >> 8;
                }
            }
        }
        U16(voice, 0x10C) = value & 0x3FFF;
    }
}
