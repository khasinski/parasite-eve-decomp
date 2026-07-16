typedef unsigned short u16;
typedef short s16;

extern u16 D_800E21C8;

int rand(void);

void func_800CFFAC(s16 *out)
{
    s16 *out_reg;
    int x;
    int phase;
    int temp;
    int y;

    out_reg = out;
    D_800E21C8 = (D_800E21C8 + 1) & 7;
    temp = rand() & 0x1FF;
    phase = D_800E21C8;
    temp += (phase << 9) & 0xC00;
    phase &= 1;
    x = temp + 0x100;
    if ((phase & 1) != 0) {
        y = (rand() & 0x1FF) + 0x100;
    } else {
        y = -(rand() & 0x1FF) - 0x100;
    }

    out_reg[0] = x;
    out_reg[1] = y;
    out_reg[2] = 0;
}
