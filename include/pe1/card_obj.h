#ifndef PE1_CARD_OBJ_H
#define PE1_CARD_OBJ_H

/* Memory-card command object (CardObj_*). field_46 is unsigned char: stored as 1
 * by BeginCommand4D and compared ==0xFF (lbu) by IsTransferActive. */
typedef struct CardObj {
    unsigned char pad_00[0x14];
    void (*fn_14)(void *);            /* 0x14 */
    void (*fn_18)(void *);            /* 0x18 */
    unsigned char pad_1C[0x04];
    int field_20;                     /* 0x20 */
    unsigned char pad_24[0x22];
    unsigned char field_46;           /* 0x46 */
    unsigned char pad_47[0x9F];
    unsigned short field_e6;          /* 0xE6 */
} CardObj;

#endif /* PE1_CARD_OBJ_H */
