#ifndef PE1_MEMCARD_H
#define PE1_MEMCARD_H

typedef struct MemCardPortState {
    unsigned char present;
    unsigned char pad01[0x417];
} MemCardPortState;

extern MemCardPortState g_MemCardPortStates[];
extern int g_MemCardActivePortOneBased;
extern int g_MemCardActiveState;
extern int g_MemCardActivePromptPending;
extern int g_MemCardLoadSucceeded;

#endif
