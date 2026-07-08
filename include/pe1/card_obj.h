#ifndef PE1_CARD_OBJ_H
#define PE1_CARD_OBJ_H

/* Memory-card command object (CardObj_*). field_46 is unsigned char: stored as 1
 * by BeginCommand4D and compared ==0xFF (lbu) by IsTransferActive. */
typedef struct CardObj {
    unsigned char *field_00;           /* 0x00 */
    unsigned char *field_04;           /* 0x04 */
    unsigned char *field_08;           /* 0x08 */
    unsigned char *field_0c;           /* 0x0C */
    struct CardObj *field_10;          /* 0x10 */
    void (*fn_14)(void *);            /* 0x14 */
    void (*fn_18)(void *);            /* 0x18 */
    unsigned char pad_1C[0x04];
    int field_20;                     /* 0x20 */
    unsigned char pad_24[0x04];
    unsigned char *payload_28;         /* 0x28 */
    unsigned char *payload_2c;         /* 0x2C */
    unsigned char *output_30;          /* 0x30 */
    unsigned char payload_28_len;      /* 0x34 */
    unsigned char payload_2c_len;      /* 0x35 */
    unsigned char command;             /* 0x36 */
    unsigned char saved_command;       /* 0x37 */
    unsigned char pad_38[0x04];
    unsigned char *response_3c;        /* 0x3C */
    unsigned char pad_40[0x05];
    unsigned char payload_index;       /* 0x45 */
    unsigned char field_46;           /* 0x46 */
    unsigned char field_47;           /* 0x47 */
    unsigned char field_48;           /* 0x48 */
    unsigned char field_49;           /* 0x49 */
    unsigned char field_4a;           /* 0x4A */
    unsigned char pad_4B[0x01];
    int field_4c;                     /* 0x4C */
    unsigned char pad_50[0x07];
    unsigned char field_57[0x06];      /* 0x57 */
    unsigned char pad_5D[0x86];
    unsigned char field_e3;           /* 0xE3 */
    unsigned char field_e4;           /* 0xE4 */
    unsigned char pad_e5[0x01];
    unsigned short field_e6;          /* 0xE6 */
    unsigned char field_e8;           /* 0xE8 */
    unsigned char field_e9;           /* 0xE9 */
    unsigned char field_ea;           /* 0xEA */
    unsigned char pad_eb[0x01];
    int field_ec;                     /* 0xEC */
} CardObj;

#endif /* PE1_CARD_OBJ_H */
