typedef unsigned char u8;

extern u8 *D_80091A28;

int func_80038CE4(int index)
{
    u8 *base = D_80091A28;
    u8 *ptr = base + (u8)index;
    u8 value = ptr[0x1D];

    base += value;
    return base[4];
}
