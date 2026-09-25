#include "common.h"
#include "pe1/akao/voice_masks.h"
#define U16(base, off) (*(u16 *)((char *)(base) + (off)))
#define S16(base, off) (*(s16 *)((char *)(base) + (off)))
#define U32(base, off) (*(u32 *)((char *)(base) + (off)))
#define ADVANCE_ENV_PTR(voice, off, timer, reset) do { \
    u16 reset_value = U16((voice), (reset)); \
    table = (s16 *)U32((voice), (off)); \
    U16((voice), (timer)) = reset_value; \
    if (table[0] == 0 && table[1] == 0) { \
        register s16 *next asm("$2") = table + table[2]; \
        U32((voice), (off)) = (u32)next; \
    } \
    table = (s16 *)U32((voice), (off)); \
    U32((voice), (off)) = (u32)(table + 1); \
    sample = *table; \
} while (0)
#define MIX_PAN_DISPATCH(voice, volume, pan_value, mask_value, label, mode_value, one_value) do { \
    int left_value; \
    if ((mode_value) == (one_value)) goto label##_case1; \
    if ((mode_value) == 4) goto label##_case4; \
    goto label##_default; \
    label##_case1: \
        S16((voice), 0x118) = ((volume) * D_8009B8F8[(pan_value)]) >> 15; \
        S16((voice), 0x11A) = ((volume) * D_8009B8F8[(pan_value) ^ 0xFF]) >> 15; \
        goto label##_done; \
    label##_case4: { \
        register int right asm("$3"); \
        S16((voice), 0x118) = ((volume) * D_8009B8F8[(pan_value)]) >> 15; \
        right = ((volume) * D_8009B8F8[(pan_value) ^ 0xFF]) >> 15; \
        S16((voice), 0x11A) = right; \
        if (((mask_value) & 0x00AAAAAA) != 0) { \
            register int flip asm("$2") = ~right; \
            S16((voice), 0x11A) = flip; \
            do { } while (0); \
        } else { \
            left_value = ~U16((voice), 0x118); \
            goto label##_store_left; \
        } \
        goto label##_done; \
    } \
    label##_default: { \
        left_value = (u32)((volume) * D_8009B9F8[0]) >> 15; \
        S16((voice), 0x11A) = left_value; \
        label##_store_left: \
        S16((voice), 0x118) = left_value; \
        goto label##_done; \
    } \
    label##_done:; \
} while (0)
#define MIX_PAN_MODE(voice, volume, pan_value, mask_value, label, mode_value) do { \
    int left_value; \
    switch (mode_value) { \
    case 1: \
        S16((voice), 0x118) = ((volume) * D_8009B8F8[(pan_value)]) >> 15; \
        S16((voice), 0x11A) = ((volume) * D_8009B8F8[(pan_value) ^ 0xFF]) >> 15; \
        break; \
    case 4: { \
        register int right asm("$3"); \
        S16((voice), 0x118) = ((volume) * D_8009B8F8[(pan_value)]) >> 15; \
        right = ((volume) * D_8009B8F8[(pan_value) ^ 0xFF]) >> 15; \
        S16((voice), 0x11A) = right; \
        if (((mask_value) & 0x00AAAAAA) != 0) { \
            register int flip asm("$2") = ~right; \
            S16((voice), 0x11A) = flip; \
            do { } while (0); \
        } else { \
            left_value = ~U16((voice), 0x118); \
            goto label##_store_left; \
        } \
        break; \
    } \
    default: { \
        left_value = (u32)((volume) * D_8009B9F8[0]) >> 15; \
        S16((voice), 0x11A) = left_value; \
        label##_store_left: \
        S16((voice), 0x118) = left_value; \
        break; \
    } \
    } \
} while (0)
#define SCALE_GLOBAL_DEPTH(value, depth) do { \
if (depth != 0) { \
    do { \
        if (depth < 0x80) { \
            (value) += ((value) * depth) >> 7; \
            break; \
        } \
        (value) = ((value) * depth) >> 8; \
    } while (0); \
} \
} while (0)

extern int D_8009D2C0;
extern unsigned char D_8009D2CE;
extern void *D_8009D2C8;
extern s16 D_8009B8F8[];
extern s16 D_8009B9F8[];
extern char D_800B8AC0[];

void Akao_SetVoiceKeyOff(AkaoTrack *cursor, u32 voice_mask, int index) {
    register void *voice asm("$6") = cursor;
    u32 flags;
    register int base_volume asm("$4");
    int value;
    int sample;
    int pan;
    void *target_voice;
    register s16 *table asm("$3");

    base_volume = (S16(voice, 0x46) * (U16(voice, 0x6C) >> 8)) >> 7;
    flags = U32(voice, 0x38);

    if ((flags & 1) != 0 && U16(voice, 0x8A) == 0) {
        U16(voice, 0x8E)--;
        if (U16(voice, 0x8E) == 0) {
            ADVANCE_ENV_PTR(voice, 0x1C, 0x8E, 0x8C);
            { register int final_value asm("$7") = (U16(voice, 0x92) * sample) >> 16;
              if (final_value != S16(voice, 0xE8)) {
                  U16(voice, 0xE8) = final_value;
                  U32(voice, 0xF4) |= 0x10;
                  if (final_value >= 0) {
                      register int doubled asm("$2") = final_value << 1;
                      U16(voice, 0xE8) = doubled;
                  }
              }
            }
        }
    }

    if ((flags & 2) != 0 && U16(voice, 0x9E) == 0) {
        U16(voice, 0xA2)--;
        if (U16(voice, 0xA2) == 0) {
            { u16 reset_value = U16(voice, 0xA0);
              table = (s16 *)U32(voice, 0x20);
              U16(voice, 0xA2) = reset_value;
              if (table[0] == 0 && table[1] == 0) {
                  { register s16 *next asm("$2") = table + table[2]; U32(voice, 0x20) = (u32)next; }
              }
            }
            value = base_volume * (U16(voice, 0xA6) >> 8);
            table = (s16 *)U32(voice, 0x20);
            U32(voice, 0x20) = (u32)(table + 1);
            sample = *table;
            { register int shifted asm("$2") = value << 9; register int upper asm("$7") = shifted >> 16;
              register int final_value asm("$7") = (upper * sample) >> 15;
              if (final_value != S16(voice, 0xEA)) {
                  U16(voice, 0xEA) = final_value;
                  U32(voice, 0xF4) |= 3;
              }
            }
        }
    }

    if ((flags & 4) != 0) {
        U16(voice, 0xB0)--;
        if (U16(voice, 0xB0) == 0) {
            { u16 reset_value = U16(voice, 0xAE);
              table = (s16 *)U32(voice, 0x24);
              U16(voice, 0xB0) = reset_value;
              if (table[0] == 0 && table[1] == 0) {
                  register s16 *next asm("$2") = table + table[2];
                  U32(voice, 0x24) = (u32)next;
              }
            }
            table = (s16 *)U32(voice, 0x24);
            U32(voice, 0x24) = (u32)(table + 1);
            { register int scale asm("$2") = U16(voice, 0xB4) >> 8;
              sample = *table;
              { register int final_value asm("$7") = (scale * sample) >> 15;
                if (final_value != S16(voice, 0xEC)) {
                    U16(voice, 0xEC) = final_value;
                    U32(voice, 0xF4) |= 3;
                }
              }
            }
        }
    }

    if ((flags & 0x20) != 0) {
        base_volume = (((int)(U16(voice, -0x10) << 17) >> 16) *
                       (U16(voice, 0x6C) >> 8)) >> 7;
        U32(voice, 0xF4) |= 3;
    }

    if ((U32(voice, 0xF4) & 3) != 0) {
        { int product; int mode; register int one asm("$2");
          base_volume += S16(voice, 0xEA);
          product = base_volume * (U16(D_8009D2C8, 0x4A) & 0x7F);
          { register int raw_pan asm("$2") = U16(voice, 0x76); register int bias asm("$3") = S16(voice, 0xEC); register int shifted asm("$2") = raw_pan >> 8; register int sum asm("$2") = shifted + bias; pan = sum & 0xFF; }
          one = 1;
          mode = D_8009D2C0;
          asm volatile("" : : "r"(mode));
          base_volume = product >> 7;
          MIX_PAN_DISPATCH(voice, base_volume, pan, voice_mask, primary, mode, one);
        }

        if ((flags & 0x800) != 0) {
            target_voice = D_800B8AC0 + U16(voice, 0x5C) * 0x11C;
            S16(target_voice, 0x118) = U16(voice, 0x118);
            S16(target_voice, 0x11A) = U16(voice, 0x11A);

            { register int raw asm("$2") = U16(voice, 0x5E);
              int mode = D_8009D2C0;
              base_volume = raw >> 1;
              MIX_PAN_MODE(voice, base_volume, pan, voice_mask, secondary, mode);
            }
            U16(target_voice, 0x118) -= U16(voice, 0x118);
            U16(target_voice, 0x11A) -= U16(voice, 0x11A);
        }
    }

    if ((flags & 0x10) != 0) {
        register int depth asm("$7") = D_8009D2CE;
        int pitch = U16(voice, -0x10) + S16(voice, 0xE8) + S16(voice, 0x36);
        SCALE_GLOBAL_DEPTH(pitch, depth);
        U16(voice, 0x10C) = pitch & 0x3FFF;
        U32(voice, 0xF4) |= 0x10;
    } else if ((U32(voice, 0xF4) & 0x10) != 0) {
        register int depth asm("$7") = D_8009D2CE;
        register int pitch asm("$3") = U32(voice, 0x30) + S16(voice, 0xE8) + S16(voice, 0x36);
        SCALE_GLOBAL_DEPTH(pitch, depth);
        { register int masked asm("$2") = pitch & 0x3FFF; U16(voice, 0x10C) = masked; }
    }
}
