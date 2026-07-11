/* MASPSX_FLAGS: --stack-return-delay */
typedef unsigned char u8;
typedef unsigned int u32;

extern int D_8009B75C;
extern int D_8009B770;
extern void *D_8009B724;
extern void *D_8009B728;
extern void *D_8009B72C;
extern void *D_8009B730;
extern void *D_8009B734;
extern void *D_8009B738;
extern void *D_8009B748;
extern u8 *D_8009B758;
extern u8 D_800A5AE0[];
extern u8 D_800A5B28[];
extern u8 D_800A5B70[];

void MemCard_SetCallbackVtable(void);
void MemCard_DmaCompleteCallback(int status);
void CardObj_ResetFields(void *obj);
int CardObj_ReadPayloadByte(void *obj);
void Render_AccumParticleAlpha(void *obj);
int CardObj_GetChannelId(u8 *entry);
u8 *CardObj_LookupByChannelId(int value);
void CardObj_SwapByteField(void *obj);
void bzero(void *dst, int len);
void MemCard_InitTimerCallbacks(void);

#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))

void MemCard_InitTransferState(u8 *arg0, u8 *arg1) {
    register u8 *obj asm("$16");
    register u8 *first_arg asm("$17");
    register u8 *second_arg asm("$18");
    register u8 *cmd asm("$4");
    register u8 *payload_a asm("$6");
    register u8 *payload_b asm("$7");
    register int i asm("$8");
    register int ff asm("$9");
    register int j asm("$3");
    void *swap_fn;

    first_arg = arg0;
    second_arg = arg1;
    D_8009B75C = 0;
    D_8009B770 = 0;
    MemCard_SetCallbackVtable();

    obj = D_800A5B70;
    D_8009B724 = MemCard_DmaCompleteCallback;
    D_8009B728 = CardObj_ResetFields;
    D_8009B72C = CardObj_ReadPayloadByte;
    D_8009B730 = Render_AccumParticleAlpha;
    D_8009B734 = CardObj_GetChannelId;
    D_8009B738 = CardObj_LookupByChannelId;
    swap_fn = CardObj_SwapByteField;
    /* Keep the bzero argument move after the callback pointer loads. */
    asm volatile("" ::: "$4");
    D_8009B758 = obj;
    D_8009B748 = swap_fn;

    bzero(obj, 0x1E0);

    /* Keep later stores based from obj/s0 instead of absolute D_800A5B70 offsets. */
    asm volatile("" : "=r"(obj) : "0"(obj));
    i = 0;
    ff = 0xFF;
    cmd = obj + 0x40;
    payload_b = D_800A5B28;
    payload_a = D_800A5AE0;
    U32_AT(obj, 0x30) = (u32)first_arg;
    U32_AT(obj, 0x120) = (u32)second_arg;

    do {
        register u8 *out asm("$2");
        u8 *bytes;

        out = (u8 *)U32_AT(cmd, -0x10);
        U32_AT(cmd, -0x34) = 0;
        U32_AT(cmd, -0x30) = (u32)obj;
        out[0] = ff;

        out = (u8 *)U32_AT(cmd, -0x10);
        out[1] = 0;
        U32_AT(cmd, -0x04) = (u32)payload_a;
        U32_AT(cmd, 0x00) = (u32)payload_b;

        bytes = obj + 0x5D;
        j = 5;
        do {
            *bytes = ff;
            j--;
            bytes++;
        } while (j >= 0);

        payload_b += 0x23;
        payload_a += 0x23;
        i++;
        cmd += 0xF0;
        obj += 0xF0;
    } while (i < 2);

    MemCard_InitTimerCallbacks();
    D_8009B75C = 1;
}
