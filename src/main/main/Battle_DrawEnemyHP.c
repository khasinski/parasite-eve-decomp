typedef signed char s8;
typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern struct { char _[16]; } D_8009CDDC_o __asm__("g_ActiveDrawSlot");
extern struct { char _[16]; } D_8009CE84_o __asm__("D_8009CE84");
extern struct { char _[16]; } D_8009CE86_o __asm__("D_8009CE86");
extern struct { char _[16]; } D_8009D1E8_o __asm__("D_8009D1E8");
extern struct { char _[16]; } D_8009D254_o __asm__("D_8009D254");
extern struct { char _[16]; } D_8009E0F0_o __asm__("D_8009E0F0");
extern struct { char _[16]; } D_8009E0F8_o __asm__("D_8009E0F8");
extern struct { char _[16]; } D_8009E1D0_o __asm__("D_8009E1D0");
extern struct { char _[16]; } D_8009E1D8_o __asm__("D_8009E1D8");
extern struct { char _[16]; } D_8009E2E8_o __asm__("D_8009E2E8");
extern struct { char _[16]; } D_8009E2F0_o __asm__("D_8009E2F0");
extern struct { char _[16]; } D_800B0E38_o __asm__("D_800B0E38");

#define g_ActiveDrawSlot (*(int *)&D_8009CDDC_o)
#define D_8009CE84 (*(u16 *)&D_8009CE84_o)
#define D_8009CE86 (*(u16 *)&D_8009CE86_o)
#define D_8009D1E8 (*(u32 *)&D_8009D1E8_o)
#define D_8009D254 (*(u8 **)&D_8009D254_o)
#define D_8009E0F0 ((u8 *)&D_8009E0F0_o)
#define D_8009E0F8 ((u8 *)&D_8009E0F8_o)
#define D_8009E1D0 ((u8 *)&D_8009E1D0_o)
#define D_8009E1D8 ((u8 *)&D_8009E1D8_o)
#define D_8009E2E8 ((u8 *)&D_8009E2E8_o)
#define D_8009E2F0 ((u8 *)&D_8009E2F0_o)
#define D_800B0E38 ((u8 **)&D_800B0E38_o)

#define U8_AT(ptr, off) (*(u8 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(u8 **)((u8 *)(ptr) + (off)))

void AddPrim(void *ot, void *prim);

static int Battle_WriteReversedDigits(s16 value, u8 *digits) {
    s16 div_value;
    int count;

    count = 0;
    div_value = value / 10;
    digits[0] = value - div_value * 10;
    while (div_value != 0) {
        value = div_value;
        count++;
        div_value = value / 10;
        digits[count] = value - div_value * 10;
    }

    return count;
}

static u8 Battle_GetHpPulse(void) {
    u8 pulse;

    pulse = D_8009D1E8 & 0x1F;
    if (pulse >= 0x11) {
        pulse = 0x20 - pulse;
    }
    return pulse;
}

static u8 *Battle_EnemyHpDigitPrim(u8 *base, int slot, int index) {
    return base + slot * 0x8C + index * 0x1C;
}

static void Battle_SetHpDigitPrim(u8 *prim, int digit, int x, int y, int shade) {
    U8_AT(prim, 0x0C) = digit * 8;
    U8_AT(prim, 0x0D) = 0xE8;
    S16_AT(prim, 0x08) = x;
    S16_AT(prim, 0x0A) = y;
    U8_AT(prim, 0x04) = shade;
    U8_AT(prim, 0x05) = shade;
    U8_AT(prim, 0x06) = shade;
}

void Battle_DrawEnemyHP(s16 hp, s16 max_hp) {
    u8 digits[16];
    u8 *entity;
    u8 *actor;
    u8 *prim;
    int current_count;
    int max_count;
    int slash_index;
    int i;
    int x;
    int y;
    int shade;
    int slot;
    int pulse;
    int x_offset;

    y = D_8009CE86 + 0xB;
    current_count = Battle_WriteReversedDigits(hp, digits);
    slash_index = current_count + 1;
    digits[slash_index] = 10;

    x = D_8009CE84 + 0x38 - slash_index * 6;
    slot = g_ActiveDrawSlot;
    entity = D_8009D254;
    actor = PTR_AT(entity, 0);

    for (i = slash_index; i >= 0; i--) {
        prim = Battle_EnemyHpDigitPrim(D_8009E1D8, slot, i);
        if ((U32_AT(actor, 0x4C) & 0x800) && i != slash_index) {
            pulse = Battle_GetHpPulse();
            shade = 0x20 + pulse * 6;
            U8_AT(prim, 0x05) = pulse + 0x60;
            U8_AT(prim, 0x04) = shade;
            U8_AT(prim, 0x06) = 0x80;
        } else {
            shade = 0x80;
            U8_AT(prim, 0x04) = shade;
            U8_AT(prim, 0x05) = shade;
            U8_AT(prim, 0x06) = shade;
        }
        U8_AT(prim, 0x0C) = digits[i] * 8;
        U8_AT(prim, 0x0D) = 0xE8;
        S16_AT(prim, 0x08) = x;
        S16_AT(prim, 0x0A) = y;
        AddPrim(D_800B0E38[slot] + 0x14, D_8009E1D0 + slot * 0x8C + i * 0x1C);
        x += 6;
    }

    max_count = Battle_WriteReversedDigits(max_hp, digits);
    x = D_8009CE84 + 0x3E - ((slash_index * 2 + 1) * 3);
    x_offset = slash_index - max_count - 1;
    if ((s8)x_offset > 0) {
        x += x_offset * 6;
    }

    for (i = max_count; i >= 0; i--) {
        prim = Battle_EnemyHpDigitPrim(D_8009E0F8, slot, i);
        if (U32_AT(actor, 0x4C) & 0x400) {
            pulse = Battle_GetHpPulse();
            U8_AT(prim, 0x04) = pulse * 8;
            U8_AT(prim, 0x05) = 0x80;
            U8_AT(prim, 0x06) = 0x20 + pulse * 6;
        } else {
            Battle_SetHpDigitPrim(prim, digits[i], x, y, 0x80);
        }

        if (U32_AT(actor, 0x4C) & 0x400) {
            U8_AT(prim, 0x0C) = digits[i] * 8;
            U8_AT(prim, 0x0D) = 0xE8;
            S16_AT(prim, 0x08) = x;
            S16_AT(prim, 0x0A) = y;
        }
        AddPrim(D_800B0E38[slot] + 0x14, D_8009E0F0 + slot * 0x70 + i * 0x1C);
        x += 6;
    }

    prim = D_8009E2F0 + slot * 0x1C;
    S16_AT(prim, 0x08) = D_8009CE84 + 0x44;
    S16_AT(prim, 0x0A) = D_8009CE86 + 0xE;
    AddPrim(D_800B0E38[slot] + 0x10, D_8009E2E8 + slot * 0x1C);
}
