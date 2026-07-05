typedef unsigned char u8;

#include "pe1/akao.h"

extern u8 D_800B290C[];

void Akao_ResetReleaseRate(AkaoTrack *track) {
    track->flags &= ~0x200;
    track->adsr_release_rate = D_800B290C[track->note_pitch << 6];
    track->update_flags |= 0x4400;
}

void SeqOp_PushLoopPoint(void *ptr) {
    unsigned short index = (*(unsigned short *)((char *)ptr + 0xCE) + 1) & 3;

    *(volatile short *)((char *)ptr + 0xCE) = index;
    *(void **)((char *)ptr + 4 + index * 4) = *(void **)ptr;
    index = *(unsigned short *)((char *)ptr + 0xCE);
    *(short *)((char *)ptr + 0x62 + index * 2) = 0;
}

void SeqOp_LoopCounter(void *ptr) {
    unsigned char *cursor = *(unsigned char **)ptr;
    int value;
    unsigned short index;
    unsigned short counter;

    *(unsigned char **)ptr = cursor + 1;
    value = *cursor;
    if (value == 0) {
        value = 0x100;
    }

    index = *(unsigned short *)((char *)ptr + 0xCE);
    counter = *(unsigned short *)((char *)ptr + 0x62 + index * 2) + 1;
    *(short *)((char *)ptr + 0x62 + index * 2) = counter;

    if (counter != value) {
        index = *(unsigned short *)((char *)ptr + 0xCE);
        *(void **)ptr = *(void **)((char *)ptr + 4 + index * 4);
    } else {
        index = *(unsigned short *)((char *)ptr + 0xCE);
        index = (index - 1) & 3;
        *(short *)((char *)ptr + 0xCE) = index;
    }
}
