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
#define D_800A76E4 (*(u32 *)&D_800A76E4_o)
#define D_800A76E8 (*(u32 *)&D_800A76E8_o)
#define D_800A76EA (*(u16 *)&D_800A76EA_o)

void Battle_CopyPadStateToRecord(void) {
    u8 *record = *(u8 **)(D_8009D278 + 0x68);
    u32 word_c;
    u32 word_10;

    *(u16 *)(record + 0x00) = D_800A76D8;
    *(u16 *)(record + 0x02) = D_800A76DA;
    *(u16 *)(record + 0x04) = D_800A76DC;
    *(u16 *)(record + 0x06) = D_800A76DE;
    *(u32 *)(record + 0x08) = D_800A76E0;

    word_c = *(u32 *)(record + 0x0C);
    word_c = (word_c & ~0x003FFFFFU) | (D_800A76E4 & 0x003FFFFFU);
    *(u32 *)(record + 0x0C) = word_c;

    word_10 = *(u32 *)(record + 0x10);
    word_10 &= ~0x0003FFFFU;
    word_10 |= D_800A76E8 & 0x0002FFFFU;
    word_10 |= (D_800A76EA & 1) << 16;
    *(u32 *)(record + 0x10) = word_10;
}
