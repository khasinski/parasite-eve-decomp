#include "common.h"

extern u8 *D_8009D278;
extern u16 D_800A76D8;
extern u16 D_800A76DA;
extern u16 D_800A76DC;
extern u16 D_800A76DE;
extern u32 D_800A76E0;
extern volatile u32 D_800A76E4;
extern volatile u32 D_800A76E8;
extern volatile u16 D_800A76EA;

void Battle_CopyPadStateToRecord(void) {
    volatile u8 *record = *(u8 **)(D_8009D278 + 0x68);
    register u32 word_c asm("$4");
    register u32 word_10 asm("$3");
    u32 middle_mask = 0x000FFC00;
    u32 middle_keep = ~0x000FFC00U;
    u32 high_keep = ~0x00300000U;
    u32 bit15_keep = ~0x00008000U;
    u32 bit16_keep = ~0x00010000U;
    u32 bit17_keep = ~0x00020000U;

    word_c = *(volatile u32 *)(record + 0x0C);
    word_10 = *(volatile u32 *)(record + 0x10);
    *(volatile u16 *)(record + 0x00) = D_800A76D8;
    *(volatile u16 *)(record + 0x02) = D_800A76DA;
    *(volatile u16 *)(record + 0x04) = D_800A76DC;
    *(volatile u16 *)(record + 0x06) = D_800A76DE;
    *(volatile u32 *)(record + 0x08) = D_800A76E0;

    word_c = (word_c & ~0x000003FFU) | (D_800A76E4 & 0x000003FFU);
    *(volatile u32 *)(record + 0x0C) = word_c;
    word_c = (word_c & middle_keep) | (D_800A76E4 & middle_mask);
    high_keep = word_c & high_keep;
    *(volatile u32 *)(record + 0x0C) = word_c;
    high_keep |= D_800A76E4 & 0x00300000U;
    *(volatile u32 *)(record + 0x0C) = high_keep;

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
    word_10 = (word_10 & bit15_keep) | (D_800A76E8 & 0x00008000U);
    *(volatile u32 *)(record + 0x10) = word_10;
    word_10 = (word_10 & bit16_keep) | ((D_800A76EA & 1) << 16);
    bit17_keep = word_10 & bit17_keep;
    *(volatile u32 *)(record + 0x10) = word_10;
    bit17_keep |= D_800A76E8 & 0x00020000U;
    *(u32 *)(record + 0x10) = bit17_keep;
}
