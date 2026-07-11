
extern int D_800A8034[];
extern int D_800A8038[];

void *Item_LookupBaseData(unsigned int index)
{
    int *endPtr;
    int base;
    register unsigned int offset;
    int end;

    endPtr = D_800A8038;
    end = endPtr[0];
    base = D_800A8034[0];

    if (index >= ((unsigned int)(end - base) >> 5)) {
        return 0;
    }

    offset = index << 5;
    asm volatile("" : "=r"(offset) : "0"(offset));
    endPtr = (int *)((char *)endPtr - 0x10);
    return (void *)(base + (offset + (int)endPtr));
}
