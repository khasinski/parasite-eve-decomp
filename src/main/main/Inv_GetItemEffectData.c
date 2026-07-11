typedef unsigned char u8;
typedef int s32;

extern u8 D_800A1B50[];
extern s32 D_8009D218;

u8 *Str_LookupTable4(int index);
u8 *Str_LookupTable8(int index);

#define COPY_EFFECT_TEXT(src_)                    \
    do {                                          \
        u8 *src = (src_);                         \
        u8 *dst = D_800A1B50;                     \
        u8 ch = *src++;                           \
        *dst++ = ch;                              \
        if (ch != 0xFF) {                         \
            do {                                  \
                ch = *src++;                      \
                *dst++ = ch;                      \
            } while (ch != 0xFF);                 \
        }                                         \
    } while (0)

#define APPEND_EFFECT_TEXT(src_)                  \
    do {                                          \
        u8 *dst = D_800A1B50;                     \
        u8 *src = (src_);                         \
        u8 ch;                                    \
        while (*dst != 0xFF) {                    \
            dst++;                                \
        }                                         \
        do {                                      \
            ch = *src++;                          \
            *dst++ = ch;                          \
        } while (ch != 0xFF);                     \
    } while (0)

u8 *Inv_GetItemEffectData(int item_id, int effect_type) {
    if (item_id == 0) {
        COPY_EFFECT_TEXT(Str_LookupTable4(0x13));
    } else if (effect_type >= 2) {
        COPY_EFFECT_TEXT(Str_LookupTable4(0x12));
    } else if (D_8009D218 == 0) {
        COPY_EFFECT_TEXT(Str_LookupTable8(item_id - 1));
        APPEND_EFFECT_TEXT(Str_LookupTable4(effect_type + 0x10));
    } else {
        COPY_EFFECT_TEXT(Str_LookupTable4(effect_type + 0x10));
        APPEND_EFFECT_TEXT(Str_LookupTable8(item_id - 1));
    }

    return D_800A1B50;
}
