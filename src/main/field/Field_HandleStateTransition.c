/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern struct { char _[16]; } D_8009D1A0_o __asm__("D_8009D1A0");
extern struct { char _[16]; } D_800B0CD8_o __asm__("D_800B0CD8");
extern struct { char _[16]; } D_800BE9A0_o __asm__("D_800BE9A0");
extern struct { char _[16]; } D_800BE9A2_o __asm__("D_800BE9A2");
extern struct { char _[16]; } D_800BE9A6_o __asm__("D_800BE9A6");
extern struct { char _[16]; } D_800BE9A7_o __asm__("D_800BE9A7");
extern struct { char _[16]; } D_800B0DBF_o __asm__("D_800B0DBF");

#define D_8009D1A0 (*(u32 *)&D_8009D1A0_o)
#define D_800B0CD8 (*(u32 *)&D_800B0CD8_o)
#define D_800BE9A0 (*(u16 *)&D_800BE9A0_o)
#define D_800BE9A2 (*(u16 *)&D_800BE9A2_o)
#define D_800BE9A6 (*(u8 *)&D_800BE9A6_o)
#define D_800BE9A7 (*(u8 *)&D_800BE9A7_o)
#define D_800B0DBF (*(s8 *)&D_800B0DBF_o)

extern u32 D_8009D1E4;
extern u32 D_8009D1F4;
extern u32 D_8009D238;
extern u32 D_8009D26C;
extern u32 D_8009D280;
extern u32 D_8009D2A8;
extern u32 D_8009D2D4;
extern u8 D_8009D1C0[];
extern u8 D_800921F8[];
extern u32 D_80092200[];
extern u32 D_800A76F0[];
extern u32 D_800A7770[];

int CardObj_GetModeClass(void);
int CardObj_GetField(int arg0, int mode, int index);
void CardObj_SetCommandPayload(int arg0, void *payload, int size);
void CardObj_StartCommandWithBytes(int arg0, int byte1, int byte2);
void CardObj_StartCommand4D(int arg0, void *payload);
void *MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base);

void Field_HandleStateTransition(void) {
    u32 flags;
    u32 pad_bits;
    u32 old_pad_bits;
    u32 changed;
    u32 mask;
    u32 sequence_mask;
    u32 menu_open;
    u16 pad;
    u32 inverted;
    int mode;
    int i;
    u8 analog;

    if (CardObj_GetModeClass() == 0) {
        flags = D_8009D1A0;
        if ((flags & 0x4001) == 0) {
            D_8009D1F4 = 4;
            D_8009D26C = 4;
            D_8009D1E4 = 0;
            D_8009D1A0 = flags | 0x4000;
            return;
        }
        D_8009D1A0 = flags | 0x4000;
    }

    mode = D_800BE9A0 & 0xF000;
    if ((mode != 0x4000) && (mode != 0x7000)) {
        D_8009D26C = 0;
        D_8009D1F4 = 0;
        D_8009D1E4 = 0;
        return;
    }

    if (D_8009D1A0 & 0x4000) {
        mode = CardObj_GetModeClass();
        if (mode == 2) {
            D_8009D1A0 &= ~0x4000;
        } else if (mode == 1) {
            CardObj_SetCommandPayload(0, D_8009D1C0, 2);
        } else if (mode == 6) {
            if (CardObj_GetField(0, 2, 0) != 0) {
                if ((D_8009D1A0 & 0x8000) == 0) {
                    CardObj_StartCommandWithBytes(0, 1, 0);
                    D_8009D1A0 |= 0x8000;
                } else {
                    CardObj_StartCommand4D(0, D_800921F8);
                    D_8009D1A0 &= ~0x4000;
                }
            }
        }
    }

    pad = D_800BE9A2;
    inverted = ~pad;
    pad_bits = inverted & 0xFFFF9FFF;
    old_pad_bits = D_8009D26C;
    D_8009D238 = old_pad_bits;
    if (inverted & 0x2000) {
        pad_bits |= 0x4000;
    }
    if (inverted & 0x4000) {
        pad_bits |= 0x2000;
    }

    D_8009D26C = 0;
    for (i = 0; i < 0x20; i++) {
        if (((u16)pad_bits & D_800A76F0[i]) != 0) {
            D_8009D26C |= 1u << i;
            D_800A7770[i]++;
        } else {
            D_800A7770[i] = 0;
        }
    }

    flags = D_8009D1A0;
    if ((flags & 1) != 0) {
        pad_bits = D_8009D26C;
        if ((int)pad_bits < 0) {
            mask = ((pad_bits ^ D_8009D2D4) & pad_bits) & 0x7000007E;
            if (mask != 0) {
                sequence_mask = D_80092200[D_8009D2A8];
                if ((mask & sequence_mask) == sequence_mask) {
                    D_8009D2A8++;
                    if (D_8009D2A8 == 9) {
                        D_8009D280 = 0xAA108448;
                        D_8009D1A0 = flags | 0x12000;
                    }
                } else {
                    D_8009D2A8 = 0;
                }
            }
        } else {
            D_8009D2A8 = 0;
        }
        D_8009D2D4 = D_8009D26C;
    }

    menu_open = D_800B0CD8;
    if (menu_open & 0x400) {
        D_8009D26C &= 0x40FFDC7F;
    }
    if (menu_open & 0x200) {
        D_8009D26C &= 0x60FFDB04;
        if (D_800B0DBF != 1) {
            D_8009D26C &= 0xDFFFFCFF;
        }
    }
    if (D_8009D26C & 0x10000080) {
        D_8009D26C &= 0x7FFFFFFF;
    }
    if (D_8009D26C & 0x40000401) {
        D_8009D26C &= 0xEFFFFF7F;
        D_8009D26C &= 0x7FFFFFFF;
    }
    if (D_8009D26C & 0x20000300) {
        D_8009D26C &= 0xBFFFFBFE;
        D_8009D26C &= 0xEFFFFF7F;
        D_8009D26C &= 0x7FFFFFFF;
    }

    if ((D_800BE9A0 & 0xF000) == 0x7000) {
        D_8009D26C &= ~0x79;
        menu_open = MenuWidget_FindByModeAndSelectedBase(1, 0) != 0;

        analog = D_800BE9A7;
        if (menu_open) {
            if (analog < 0x14) {
                D_8009D26C |= 0x8;
            } else if (analog >= 0xE7) {
                D_8009D26C |= 0x20;
            }

            analog = D_800BE9A6;
            if (analog < 0x14) {
                D_8009D26C |= 0x40;
            } else if (analog >= 0xE7) {
                D_8009D26C |= 0x10;
            }
        } else {
            if (analog < 0x5A) {
                D_8009D26C |= 0x8;
                if (analog < 0x14) {
                    D_8009D26C |= 0x1;
                }
            } else if (analog >= 0xA1) {
                D_8009D26C |= 0x20;
                if (analog >= 0xE7) {
                    D_8009D26C |= 0x1;
                }
            }

            analog = D_800BE9A6;
            if (analog < 0x5A) {
                D_8009D26C |= 0x40;
                if (analog < 0x14) {
                    D_8009D26C |= 0x1;
                }
            } else if (analog >= 0xA1) {
                D_8009D26C |= 0x10;
                if (analog >= 0xE7) {
                    D_8009D26C |= 0x1;
                }
            }
        }
    }

    pad_bits = D_8009D26C;
    old_pad_bits = D_8009D238;
    changed = pad_bits ^ old_pad_bits;
    D_8009D1F4 = changed & pad_bits;
    D_8009D1E4 = changed & old_pad_bits;
}
