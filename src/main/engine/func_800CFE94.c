typedef short s16;

int SquareRoot0(int value);

int func_800CFE94(s16 *from, s16 *to)
{
    int dx = to[0] - from[0];
    int dy = to[1] - from[1];
    int dz = to[2] - from[2];
    int length = SquareRoot0((dx * dx) + (dy * dy) + (dz * dz));

    if (length == 0) {
        length = 1;
    }

    return length;
}
