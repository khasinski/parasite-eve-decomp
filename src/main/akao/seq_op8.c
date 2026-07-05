void sndTrackReadAdsrDecayRate(void *ptr);
void sndTrackReadAdsrSustainLevel(void *ptr, int arg);

void SeqOp_SetTrack34Mask(void *ptr);

void SeqOp_SetMask(void *ptr);

void SeqOp_ClearTrack34Mask(void *ptr);
void SeqOp_ClearTrack3CMask(void *ptr, int arg);
void SeqOp_ClearMask(void *ptr, int arg);

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned short u16;

extern u32 D_800B89D0;
extern u32 D_800B89D4;
extern u32 D_800B89D8;
extern u32 D_800B89DC;
extern u32 D_800B89E0;

void Seq_StartNestedStreams(void *ptr, void *first, void *second);

void SeqOp_ReadAdsrDecayAndSustain(void *ptr, int arg) {
    sndTrackReadAdsrDecayRate(ptr);
    sndTrackReadAdsrSustainLevel(ptr, arg);
}

void SeqOp_SetDurationBAAndMask(void *ptr) {
    unsigned char *cursor = *(unsigned char **)ptr;
    int value;

    *(unsigned char **)ptr = cursor + 1;
    value = *cursor;
    if (value != 0) {
        value++;
    } else {
        value = 0x101;
    }
    *(short *)((char *)ptr + 0xBA) = value;
    SeqOp_SetTrack34Mask(ptr);
}

void SeqOp_SetDuration(unsigned char *arg0) {
    unsigned char *ptr = *(unsigned char **)arg0;
    int value;

    *(unsigned char **)arg0 = ptr + 1;
    value = ptr[0];
    if (value == 0) {
        value = 0x101;
    } else {
        value++;
    }
    *(unsigned short *)(arg0 + 0xBA) = value;
}

void SeqOp_SetRepeatCount(void *ptr) {
    unsigned char *cursor = *(unsigned char **)ptr;
    int value;

    *(unsigned char **)ptr = cursor + 1;
    value = *cursor;
    if (value != 0) {
        value++;
    } else {
        value = 0x101;
    }
    *(short *)((char *)ptr + 0xBC) = value;
    SeqOp_SetMask(ptr);
}

void SeqOp_SetDurationAlt(unsigned char *arg0) {
    unsigned char *ptr = *(unsigned char **)arg0;
    int value;

    *(unsigned char **)arg0 = ptr + 1;
    value = ptr[0];
    if (value == 0) {
        value = 0x101;
    } else {
        value++;
    }
    *(unsigned short *)(arg0 + 0xBC) = value;
}

void SeqOp_StopAndClearTrack(void *ptr, int arg) {
    *(int *)((char *)ptr + 0x38) &= -0x38;
    SeqOp_ClearTrack34Mask(ptr);
    SeqOp_ClearTrack3CMask(ptr, arg);
    SeqOp_ClearMask(ptr, arg);
    *(unsigned short *)((char *)ptr + 0x84) &= 0xFFFA;
}

void SeqOp_SetFlag4(void *ptr) {
    *(int *)((char *)ptr + 0x38) |= 0x10;
}

void SeqOp_ClearFlag10(void *ptr) {
    *(int *)((char *)ptr + 0x38) &= ~0x10;
}

void SeqOp_SetFlag20(void *ptr) {
    *(int *)((char *)ptr + 0x38) |= 0x20;
}

void SeqOp_ClearFlag20(void *ptr) {
    *(int *)((char *)ptr + 0x38) &= ~0x20;
}

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
