typedef signed short s16;

int Math_IntSqrt(int value);

int Battle_CalcDistToPlayer(void *arg0, void *arg1)
{
    int x;
    int y;

    x = *(s16 *)((char *)arg0 + 0x268) - *(s16 *)((char *)arg1 + 0x2A);
    y = *(s16 *)((char *)arg0 + 0x26C) - *(s16 *)((char *)arg1 + 0x32);
    return Math_IntSqrt((x * x) + (y * y));
}
