/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned short u16;

extern struct { char _[16]; } D_8009D278_o __asm__("D_8009D278");
extern struct { char _[16]; } D_800A76D8_o __asm__("D_800A76D8");
extern struct { char _[16]; } D_800A76DA_o __asm__("D_800A76DA");
extern struct { char _[16]; } D_800A76DC_o __asm__("D_800A76DC");
extern struct { char _[16]; } D_800A76DE_o __asm__("D_800A76DE");
extern struct { char _[16]; } D_800A76E0_o __asm__("D_800A76E0");
extern struct { char _[16]; } D_800A76E4_o __asm__("D_800A76E4");
extern struct { char _[16]; } D_800A76E8_o __asm__("D_800A76E8");
extern struct { char _[16]; } D_800A76EA_o __asm__("D_800A76EA");

#define D_8009D278 (*(u8 **)&D_8009D278_o)
#define D_800A76D8 (*(u16 *)&D_800A76D8_o)
#define D_800A76DA (*(u16 *)&D_800A76DA_o)
#define D_800A76DC (*(u16 *)&D_800A76DC_o)
#define D_800A76DE (*(u16 *)&D_800A76DE_o)
#define D_800A76E0 (*(u32 *)&D_800A76E0_o)
#define D_800A76E4 (*(volatile u32 *)&D_800A76E4_o)
#define D_800A76E8 (*(volatile u32 *)&D_800A76E8_o)
#define D_800A76EA (*(volatile u16 *)&D_800A76EA_o)

void Battle_CopyPadStateToRecord(void) {
    volatile u8 *record = *(u8 **)(D_8009D278 + 0x68);
    u32 word_c;
    u32 word_10;

    *(volatile u16 *)(record + 0x00) = D_800A76D8;
    *(volatile u16 *)(record + 0x02) = D_800A76DA;
    *(volatile u16 *)(record + 0x04) = D_800A76DC;
    *(volatile u16 *)(record + 0x06) = D_800A76DE;
    *(volatile u32 *)(record + 0x08) = D_800A76E0;

    word_c = *(volatile u32 *)(record + 0x0C);
    word_c = (word_c & ~0x000003FFU) | (D_800A76E4 & 0x000003FFU);
    *(volatile u32 *)(record + 0x0C) = word_c;
    word_c = (word_c & ~0x000FFC00U) | (D_800A76E4 & 0x000FFC00U);
    *(volatile u32 *)(record + 0x0C) = word_c;
    word_c = (word_c & ~0x00300000U) | (D_800A76E4 & 0x00300000U);
    *(volatile u32 *)(record + 0x0C) = word_c;

    word_10 = *(volatile u32 *)(record + 0x10);
    word_10 = (word_10 & ~0x0000000FU) | (D_800A76E8 & 0x0000000FU);
    *(volatile u32 *)(record + 0x10) = word_10;
    word_10 = (word_10 & ~0x00000030U) | (D_800A76E8 & 0x00000030U);
    *(volatile u32 *)(record + 0x10) = word_10;
    word_10 = (word_10 & ~0x000000C0U) | (D_800A76E8 & 0x000000C0U);
    *(volatile u32 *)(record + 0x10) = word_10;
    word_10 = (word_10 & ~0x00000100U) | (D_800A76E8 & 0x00000100U);
    *(volatile u32 *)(record + 0x10) = word_10;
    word_10 = (word_10 & ~0x00000200U) | (D_800A76E8 & 0x00000200U);
    *(volatile u32 *)(record + 0x10) = word_10;
    word_10 = (word_10 & ~0x00000400U) | (D_800A76E8 & 0x00000400U);
    *(volatile u32 *)(record + 0x10) = word_10;
    word_10 = (word_10 & ~0x00000800U) | (D_800A76E8 & 0x00000800U);
    *(volatile u32 *)(record + 0x10) = word_10;
    word_10 = (word_10 & ~0x00001000U) | (D_800A76E8 & 0x00001000U);
    *(volatile u32 *)(record + 0x10) = word_10;
    word_10 = (word_10 & ~0x00006000U) | (D_800A76E8 & 0x00006000U);
    *(volatile u32 *)(record + 0x10) = word_10;
    word_10 = (word_10 & ~0x00008000U) | (D_800A76E8 & 0x00008000U);
    *(volatile u32 *)(record + 0x10) = word_10;
    word_10 = (word_10 & ~0x00010000U) | ((D_800A76EA & 1) << 16);
    *(volatile u32 *)(record + 0x10) = word_10;
    word_10 = (word_10 & ~0x00020000U) | (D_800A76E8 & 0x00020000U);
    *(volatile u32 *)(record + 0x10) = word_10;
}
