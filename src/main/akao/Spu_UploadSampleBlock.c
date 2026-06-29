/* CC1_FLAGS: -g3 -G0 */
/* MASPSX_FLAGS: -G0 */

typedef signed char s8;
typedef int s32;

void Spu_WaitTransferDone(void);
s32 Spu_ValidateSampleHeader(void *arg0);
s32 Spu_WriteRegChecked(s32 arg0) __asm__("func_80085EB4");
void Spu_UploadWithPrepare(s32 arg0, s32 arg1);

extern s32 g_SpuTransferStatus;
extern s32 g_AkaoInstrumentTable[];

s32 Spu_UploadSampleBlock(void *arg0, s32 arg1) {
    register s8 *cursor asm("$16");
    s8 *src;
    s32 start;
    s32 flush;
    s32 end;
    s32 count;
    s32 dst_offset;
    s32 *dst;

    cursor = arg0;
    flush = arg1;
    Spu_WaitTransferDone();
    if (Spu_ValidateSampleHeader(arg0) == 0) {
        cursor += 0x10;
        Spu_WriteRegChecked(*(s32 *)cursor);
        cursor += 4;
        asm volatile("" : "=r"(cursor) : "0"(cursor));
        count = *(s32 *)cursor;
        cursor += 4;
        asm volatile("" : "=r"(cursor) : "0"(cursor));
        start = *(s32 *)cursor;
        cursor += 4;
        asm volatile("" : "=r"(cursor) : "0"(cursor));
        end = *(s32 *)cursor;
        src = cursor + 0x24;
        if (end == 0) {
            end = 0x100;
        }

        cursor = (s8 *)(end - start);
        Spu_UploadWithPrepare((s32)(src + ((s32)cursor << 6)), count);

        dst_offset = start << 6;
        dst = (s32 *)((s8 *)g_AkaoInstrumentTable + dst_offset);
        count = (s32)cursor << 4;
        if (count != 0) {
            do {
                count--;
                *dst++ = *(s32 *)src;
                src += 4;
            } while (count != 0);
        }

        if (flush != 0) {
            Spu_WaitTransferDone();
        }
        return 0;
    }

    g_SpuTransferStatus = -1;
    return -1;
}
