#include "common.h"
#include "pe1/psyq_nop.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern u32 field_flags_read_0[4] __asm__("g_GameStateFlags");
extern u32 field_flags_read_1[4] __asm__("g_GameStateFlags");
extern u32 field_flags_read_3[4] __asm__("g_GameStateFlags");
extern u32 field_flags_read_4[4] __asm__("g_GameStateFlags");
extern u32 field_flags_read_5[4] __asm__("g_GameStateFlags");
extern u32 field_flags_read_6[4] __asm__("g_GameStateFlags");
extern u32 field_flags_write[4] __asm__("g_GameStateFlags");
extern struct { char _[16]; } D_800B0CD8_o __asm__("g_GameState");
extern struct { char _[16]; } D_800BE9A0_o __asm__("g_AnalogStickState");
extern struct { char _[16]; } D_800BE9A2_o __asm__("D_800BE9A2");
extern struct { char _[16]; } D_800BE9A6_o __asm__("D_800BE9A6");
extern struct { char _[16]; } D_800BE9A7_o __asm__("D_800BE9A7");
extern struct { char _[16]; } D_800B0DBF_o __asm__("D_800B0DBF");


#define D_800B0CD8 (*(u32 *)&D_800B0CD8_o)
#define D_800BE9A0 (*(u16 *)&D_800BE9A0_o)
#define D_800BE9A2 (*(u16 *)&D_800BE9A2_o)
#define D_800BE9A6 (*(u8 *)&D_800BE9A6_o)
#define D_800BE9A7 (*(u8 *)&D_800BE9A7_o)
#define D_800B0DBF (*(s8 *)&D_800B0DBF_o)

extern u32 D_8009D1E4;
extern u32 D_8009D1F4;
extern u32 D_8009D238;
extern u32 D_8009D26C __asm__("g_FieldPadBits");
extern u32 D_8009D280[] __asm__("g_SceneDispatchToken");
extern u32 D_8009D2A8;
extern u32 D_8009D2D4;
extern u8 D_8009D1C0[];
extern u8 D_800921F8[];
extern u32 D_80092200[];
extern u32 D_800A76F0[];
extern u32 D_800A7770[];

int CardObj_GetModeClass();
int CardObj_GetField(int arg0, int mode, int index);
void CardObj_SetCommandPayload(int arg0, void *payload, int size);
void CardObj_StartCommandWithBytes(int arg0, int byte1, int byte2);
void CardObj_StartCommand4D(int arg0, void *payload);
void *MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base);

void Field_HandleStateTransition(void) {
    u32 flags;
    u32 pad_bits;
    register u32 base_bits asm("$3");
    register u32 old_pad_bits asm("$2");
    register u32 pad_mask asm("$2");
    register u32 active_pad asm("$4");
    register u32 save_pad asm("$2");
    register u32 final_pad asm("$2");
    register u32 final_old asm("$4");
    register u32 changed asm("$3");
    u32 mask;
    u32 sequence_mask;
    u32 menu_open;
    register u32 pad asm("$4");
    u32 inverted;
    int mode;
    u16 i;
    u8 analog;

    if (CardObj_GetModeClass(0) == 0) {
        u32 initial_flags = field_flags_read_0[0];
        if ((initial_flags & 0x4001) == 0) {
            D_8009D1F4 = 4;
            D_8009D26C = 4;
            D_8009D1E4 = 0;
            field_flags_write[0] = initial_flags | 0x4000;
            return;
        }
        field_flags_write[0] = initial_flags | 0x4000;
    }

    mode = D_800BE9A0 & 0xF000;
    if ((mode != 0x4000) && (mode != 0x7000)) {
        goto clear_input;
    }

    if (field_flags_read_1[0] & 0x4000) {
        mode = CardObj_GetModeClass(0);
        if (mode == 2) {
            goto clear_4000;
        }
        if (mode == 1) {
            CardObj_SetCommandPayload(0, D_8009D1C0, 2);
            goto after_mode;
        }
        if (mode != 6) {
            goto after_mode;
        }
        if (CardObj_GetField(0, 2, 0) == 0) {
            goto after_mode;
        }
        if ((field_flags_read_3[0] & 0x8000) == 0) {
            CardObj_StartCommandWithBytes(0, 1, 0);
            field_flags_write[0] = field_flags_read_4[0] | 0x8000;
            goto after_mode;
        }
        CardObj_StartCommand4D(0, D_800921F8);
clear_4000:
        field_flags_write[0] = field_flags_read_5[0] & ~0x4000;
after_mode:
        ;

    }

    pad = D_800BE9A2;
    asm volatile("" : : "r"(pad));
    pad_mask = 0xFFFF9FFF;
    asm volatile("" : : "r"(pad_mask));
    base_bits = (~pad) & pad_mask;
    asm volatile("" : "=r"(pad) : "0"(pad));
    old_pad_bits = D_8009D26C;
    D_8009D238 = old_pad_bits;
    inverted = ~pad;
    pad_bits = base_bits;
    if (inverted & 0x2000) {
        pad_bits = base_bits | 0x4000;
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

    flags = field_flags_read_6[0];
    if ((flags & 1) != 0) {
        active_pad = D_8009D26C;
        if ((int)active_pad < 0) {
            mask = ((active_pad ^ D_8009D2D4) & active_pad) & 0x7000007E;
            if (mask != 0) {
                sequence_mask = D_80092200[D_8009D2A8];
                if ((mask & sequence_mask) == sequence_mask) {
                    D_8009D2A8++;
                    if (D_8009D2A8 == 9) {
                        D_8009D280[0] = 0xAA108448;
                        field_flags_write[0] = flags | 0x12000;
                    }
                } else {
                    D_8009D2A8 = 0;
                }
            }
        } else {
            D_8009D2A8 = 0;
        }
        save_pad = D_8009D26C;
        PE1_NOP();
        D_8009D2D4 = save_pad;
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

        if (menu_open) {
            analog = D_800BE9A7;
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
            analog = D_800BE9A7;
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

    final_pad = D_8009D26C;
    final_old = D_8009D238;
    changed = final_pad ^ final_old;
    D_8009D1F4 = changed & final_pad;
    D_8009D1E4 = changed & final_old;
    return;

clear_input:
    D_8009D26C = 0;
    D_8009D1F4 = 0;
    D_8009D1E4 = 0;
}
