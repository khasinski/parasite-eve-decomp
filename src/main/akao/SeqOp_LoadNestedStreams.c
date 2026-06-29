typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned short u16;

extern u32 D_800B89D0;
extern u32 D_800B89D4;
extern u32 D_800B89D8;
extern u32 D_800B89DC;
extern u32 D_800B89E0;

void Seq_StartNestedStreams(void *ptr, void *first, void *second);

void SeqOp_LoadNestedStreams(void *ptr) {
    u8 *cursor;
    u32 offset;
    u8 *first;
    u8 *second;

    cursor = *(u8 **)ptr;
    offset = (cursor[1] << 8) | cursor[0];
    if (offset != 0) {
        first = cursor + offset + 2;
    } else {
        first = 0;
    }

    cursor += 2;
    offset = (cursor[1] << 8) | cursor[0];
    if (offset != 0) {
        second = cursor + offset + 2;
    } else {
        second = 0;
    }

    D_800B89D4 = 0;
    D_800B89D8 = 0;
    D_800B89DC = *(u16 *)((char *)ptr + 0x76) >> 8;
    D_800B89E0 = *(int *)((char *)ptr + 0x44) >> 23;
    Seq_StartNestedStreams(&D_800B89D0, first, second);
    *(u8 **)ptr += 4;
}
