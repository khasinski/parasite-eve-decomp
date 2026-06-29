typedef struct TrackLike {
    char pad[0x56];
    unsigned short limit;
} TrackLike;

extern TrackLike *g_AkaoCurTrack;

void SeqOp_BranchIfLoopLimitReached(unsigned char **stream)
{
    register unsigned char **cursor asm("$6");
    unsigned char *ptr;
    unsigned char *target;
    int value;
    int lo;
    int offset;

    cursor = stream;
    ptr = *cursor;
    *cursor = ptr + 1;
    value = ptr[0];
    if ((int)g_AkaoCurTrack->limit >= value) {
        *cursor = ptr + 2;
        lo = ptr[1];
        target = ptr + 3;
        *cursor = target;
        offset = lo | (ptr[2] << 8);
        target += (short)offset;
        *cursor = target;
    } else {
        *cursor = ptr + 3;
    }
}
