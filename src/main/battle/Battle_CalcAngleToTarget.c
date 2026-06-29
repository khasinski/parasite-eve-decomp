typedef signed short s16;

int Gte_Atan2(int arg0, int arg1);

int Battle_CalcAngleToTarget(void *arg0, void *arg1)
{
    int x;
    int y;
    int angle;

    x = *(s16 *)((char *)arg0 + 0xB4) << 16;
    y = *(s16 *)((char *)arg0 + 0xB8) << 16;
    angle = Gte_Atan2(x - *(int *)((char *)arg1 + 0), y - *(int *)((char *)arg1 + 8));
    return (s16)(angle + 0x800);
}
