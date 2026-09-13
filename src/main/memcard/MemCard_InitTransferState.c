/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"
#include "pe1/card_obj.h"

extern CardObj D_800A5B70[];
extern u8 D_800A5AE0[];
extern u8 D_800A5B28[];

extern int g_MemCardServiceReady;
extern int g_MemCardCommandByte;
extern CardObj *g_MemCardObjArray;
extern int (*D_8009B724)(int);
extern void (*g_MemCardObjResetFn)(CardObj *);
extern int (*D_8009B72C)(CardObj *);
extern void (*D_8009B730)(CardObj *);
extern int (*D_8009B734)(CardObj *);
extern CardObj *(*g_MemCardObjLookupFn)(int);
extern void (*D_8009B748)(CardObj *);

void MemCard_SetCallbackVtable(void);
int MemCard_DmaCompleteCallback(int result);
void CardObj_ResetFields(CardObj *obj);
int CardObj_ReadPayloadByte(CardObj *obj);
void LIBPAD_PADPORTD_text_388(CardObj *);
int CardObj_GetChannelId(CardObj *obj);
CardObj *CardObj_LookupByChannelId(int channel);
void CardObj_SwapByteField(CardObj *obj);
void bzero(void *ptr, int size);
void MemCard_InitTimerCallbacks(void);

int MemCard_InitTransferState(u8 *output0, u8 *output1) {
    u8 *savedOutput0 = output0;
    u8 *savedOutput1 = output1;
    register CardObj *obj asm("$16");
    u8 *table3c;
    u8 *table40;
    int i;
    int fill;
    register u8 *field40 asm("$4");
    u8 *cursor;
    void (*swapCommand)(CardObj *);
    CardObj *clearObj;
    int count;

    g_MemCardServiceReady = 0;
    g_MemCardCommandByte = 0;
    MemCard_SetCallbackVtable();

    asm("" : "=r"(obj) : "0"(&D_800A5B70[0]));
    D_8009B724 = MemCard_DmaCompleteCallback;
    g_MemCardObjResetFn = CardObj_ResetFields;
    D_8009B72C = CardObj_ReadPayloadByte;
    D_8009B730 = LIBPAD_PADPORTD_text_388;
    D_8009B734 = CardObj_GetChannelId;
    g_MemCardObjLookupFn = CardObj_LookupByChannelId;
    swapCommand = CardObj_SwapByteField;
    asm volatile("" : "=r"(clearObj) : "0"(obj));
    g_MemCardObjArray = obj;
    D_8009B748 = swapCommand;

    bzero(clearObj, sizeof(CardObj) * 2);
    i = 0;
    fill = 0xFF;
    field40 = (u8 *)&obj->field_40;
    table40 = D_800A5B28;
    table3c = D_800A5AE0;
    obj[0].output_30 = savedOutput0;
    obj[1].output_30 = savedOutput1;
    do {
        *(void **)(field40 - 0x34) = 0;
        *(CardObj **)(field40 - 0x30) = obj;
        (*(u8 **)(field40 - 0x10))[0] = fill;
        (*(u8 **)(field40 - 0x10))[1] = 0;
        *(u8 **)(field40 - 4) = table3c;
        *(u8 **)field40 = table40;
        cursor = obj->field_5d;
        count = 5;
        do {
            *cursor = fill;
            count--;
            cursor++;
        } while (count >= 0);
        table40 += 0x23;
        table3c += 0x23;
        i++;
        field40 += sizeof(CardObj);
        obj++;
    } while (i < 2);

    MemCard_InitTimerCallbacks();
    g_MemCardServiceReady = 1;
    asm volatile("" : : : "memory");
    return 1;
}
