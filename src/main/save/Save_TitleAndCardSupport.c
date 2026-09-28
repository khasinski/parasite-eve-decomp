#include "common.h"
#include "pe1/save.h"


void Save_SprintfSjis(unsigned char *dst, unsigned char *fmt)
{
    SaveSjisFormatArgument *args;
    int ch;
    unsigned char *out;
    int value;
    unsigned int code;
    unsigned char *text;
    int byte;

    out = dst;
    ch = *fmt;
    args = (SaveSjisFormatArgument *)&g_SaveSjisFormatArguments;
    while (ch != 0) {
        /* lbu already zero-extends; this keeps the retail andi on v1. */
        asm volatile("" : "=r"(ch) : "0"(ch));
        ch &= 0xFF;
        fmt++;
        if (ch == '%') {
            ch = *fmt++;
            switch (ch) {
            case 'd':
                value = args++->number;
                code = (value / 10) % 10 + 0x824Fu;
                *out++ = code >> 8;
                *out++ = (unsigned char)code;
                code = value % 10 + 0x824Fu;
                *out++ = code >> 8;
                *out++ = (unsigned char)code;
                break;
            case 'D':
                value = args++->number;
                code = value % 10 + 0x824Fu;
                *out++ = code >> 8;
                *out++ = (unsigned char)code;
                break;
            case 's':
                text = args++->text;
                if (text == 0) break;
                byte = *text;
                if (byte == 0) break;
                do {
                    text++;
                    *out++ = byte;
                    byte = *text;
                } while (byte != 0);
                break;
            default:
                break;
            }
        } else {
            *out++ = *(fmt - 1);
        }
        ch = *fmt;
    }
    *out = 0;
}

extern u8 g_SaveTitleBuffer[];
extern int g_SaveTitleStyleFlag;
extern int g_PlayTimeFrameCounter;
extern u8 *g_SaveTitleFormatLongPtr;
extern u8 *g_SaveTitleFormatShortPtr;

void bzero(void *dst, int len);
int Save_ClassifyPlaytime(int arg0);
int Save_GetCurrentMapNumber(void);
u8 *Tbl_FindNthNonEmpty(int arg0);

typedef unsigned char u8_1;

extern int g_MemCardEventPort1Spec0004;
extern int g_MemCardEventPort1Spec8000;
extern int g_MemCardEventPort1Spec0100;
extern int g_MemCardEventPort1Spec2000;

extern int g_MemCardEventF400Spec0004Flag;
extern int g_MemCardRemovedEventPending;
extern int g_MemCardEventF400Spec2000Flag;

int TestEvent(int event);

extern int g_MemCardEventF000Spec0004;
extern int g_MemCardEventF000Spec8000;
extern int g_MemCardEventF000Spec0100;
extern int g_MemCardEventF000Spec2000;

extern int g_MemCardEventF000Spec0004Flag;
extern int g_MemCardEventF000Spec8000Flag;
extern int g_MemCardEventF000Spec2000Flag;

extern unsigned char D_800A0EDC[];
extern unsigned char D_800A12F4;
extern int g_MemCardConnectDebounce;
extern int D_800A1848[];

u8 *Save_FormatTitle(int chapter, int playtime_seconds) {
    int minutes_total;
    int hours;
    int minutes;
    int seconds;
    u8 *value;

    bzero(g_SaveTitleBuffer, 0x44);

    hours = playtime_seconds / 3600;
    minutes_total = playtime_seconds / 60;
    minutes = minutes_total - (hours * 60);
    seconds = playtime_seconds - (minutes_total * 60);

    g_SaveSjisFormatArguments.chapter = chapter;
    g_SaveSjisFormatArguments.hours = hours;
    g_SaveSjisFormatArguments.minutes = minutes;
    g_SaveSjisFormatArguments.seconds = seconds;

    if (g_SaveTitleStyleFlag != 0) {
        value = Tbl_FindNthNonEmpty(-1);
        g_SaveSjisFormatArguments.primary.text = value;
        Save_SprintfSjis(g_SaveTitleBuffer, g_SaveTitleFormatShortPtr);
    } else {
        g_SaveSjisFormatArguments.primary.number =
            Save_ClassifyPlaytime(g_PlayTimeFrameCounter);
        value = Tbl_FindNthNonEmpty(Save_GetCurrentMapNumber());
        g_SaveSjisFormatArguments.secondary.text = value;
        Save_SprintfSjis(g_SaveTitleBuffer, g_SaveTitleFormatLongPtr);
    }

    return g_SaveTitleBuffer;
}

u32 Save_CalcCrc16(u32 size, u8_1 *data) {
    u32 crc = 0xFFFF;
    u32 i = 0;

    size &= 0xFFFF;
    if (size != 0) {
        do {
            u32 bit = 0;

            crc ^= data[i & 0xFFFF] << 8;
            do {
                if (crc & 0x8000) {
                    crc = (crc << 1) ^ 0x1021;
                } else {
                    crc <<= 1;
                }
                bit++;
            } while ((bit & 0xFFFF) < 8);
            i++;
        } while ((i & 0xFFFF) < size);
    }

    return ~crc & 0xFFFF;
}

void MemCard_ClearPort1Events(void) {
    TestEvent(g_MemCardEventPort1Spec0004);
    TestEvent(g_MemCardEventPort1Spec8000);
    TestEvent(g_MemCardEventPort1Spec0100);
    TestEvent(g_MemCardEventPort1Spec2000);
    g_MemCardEventF400Spec2000Flag = 0;
    g_MemCardRemovedEventPending = 0;
    g_MemCardEventF400Spec0004Flag = 0;
}

void Evt_ClearEvents(void) {
    TestEvent(g_MemCardEventF000Spec0004);
    TestEvent(g_MemCardEventF000Spec8000);
    TestEvent(g_MemCardEventF000Spec0100);
    TestEvent(g_MemCardEventF000Spec2000);
    g_MemCardEventF000Spec2000Flag = 0;
    g_MemCardEventF000Spec8000Flag = 0;
    g_MemCardEventF000Spec0004Flag = 0;
}

int MemCard_PollTransferDelay(void) {
    int offset;
    int *pending;
    int count;
    int one;
    int reset;
    int value;
    int clamped;
    register int final_value asm("$4");

    count = 0;
    reset = 12;
    one = 1;
    pending = D_800A1848;
    value = g_MemCardConnectDebounce;
    offset = 0;
    g_MemCardConnectDebounce = value - (value > 0);

    do {
        if ((unsigned int)(D_800A0EDC[offset] - 2) < 2) {
            g_MemCardConnectDebounce = reset;
            *pending = one;
        }
        pending++;
        count++;
        offset += 0x418;
    } while (count < 2);

    pending = D_800A1848;
    __asm__ volatile("" : "=r"(pending) : "0"(pending));
    if (pending[0] != 0) {
        if (D_800A0EDC[0] != 4) {
            goto done;
        }
    }
    if (*(pending + 1) != 0) {
        if (D_800A12F4 != 4) {
            goto done;
        }
    }

    final_value = g_MemCardConnectDebounce;
    clamped = 4;
    if (final_value < 5) {
        clamped = final_value;
    }
    g_MemCardConnectDebounce = clamped;
    *(pending + 1) = 0;
    pending[0] = 0;
done:
    return g_MemCardConnectDebounce > 0;
}
