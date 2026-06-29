#ifndef PE1_MEMCARD_STATE_H
#define PE1_MEMCARD_STATE_H

/* Memory-card driver state object, pointed to by g_MemCardState (a value-typed
 * instance also lives at D_800A5AC0). Both words are only word-loaded/stored
 * then masked, never sign-extended, so unsigned is codegen-equivalent to the
 * int views used by some files. */
typedef struct MemCardState {
    unsigned int field0;   /* 0x0 */
    unsigned int field4;   /* 0x4 */
} MemCardState;

extern MemCardState * volatile g_MemCardState;

#endif /* PE1_MEMCARD_STATE_H */
