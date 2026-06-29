typedef signed short s16;

int Gte_Atan2(int arg0, int arg1);

int Battle_CalcRelativeAngle(void *arg0, void *arg1)
{
    register int angle asm("$3");
    int wrapped;

    angle = 0x800 - Gte_Atan2(
        *(int *)((char *)arg0 + 0x28) - *(int *)((char *)arg1 + 0x28),
        *(int *)((char *)arg0 + 0x30) - *(int *)((char *)arg1 + 0x30));
    angle = (angle << 16) >> 16;
    angle += *(s16 *)((char *)arg1 + 0x3A);

    if (angle >= 0) {
        wrapped = angle;
    } else {
        wrapped = angle + 0xFFF;
    }

    wrapped >>= 12;
    wrapped <<= 12;
    return (s16)(angle - wrapped);
}
