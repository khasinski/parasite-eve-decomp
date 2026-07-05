typedef unsigned short u16;
typedef unsigned int u32;

extern u32 g_TimerTimeoutStart;
extern u32 g_TimerTimeoutLimit;

extern int g_SpuTransferStatus;
void SpuSetTransferCallback(void (*callback)(void));

int Spu_CheckTimerElapsed(void) {
    register u32 current asm("$4");
    register u32 raw asm("$3");
    u32 base;
    register u32 diff asm("$2");
    u32 limit;

    raw = *(volatile u16 *)0x1F801120;
    base = g_TimerTimeoutStart;
    asm volatile("" : "=r"(raw) : "0"(raw));
    current = raw & 0xFFFF;
    if (current < base) {
        if (*(volatile u16 *)0x1F801128 != 0) {
            current += *(volatile u16 *)0x1F801128;
        } else {
            current += 0x10000;
        }
    }

    if ((*(volatile u16 *)0x1F801124 & 0x200) != 0) {
        limit = g_TimerTimeoutLimit;
        diff = current - g_TimerTimeoutStart;
        return !(diff < limit);
    } else {
        limit = g_TimerTimeoutLimit;
        diff = (current - g_TimerTimeoutStart) >> 3;
        return !(diff < limit);
    }
}

int Spu_ValidateSampleHeader(void *arg0)
{
    return *(int *)arg0 + 0xB0BEB4BF;
}

void Spu_ClearTransferCallback(void) {
    SpuSetTransferCallback(0);
    g_SpuTransferStatus = 0;
}
