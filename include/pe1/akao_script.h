#ifndef PE1_AKAO_SCRIPT_H
#define PE1_AKAO_SCRIPT_H

/* Canonical Akao sequencer per-track script state.
 *
 * Union of the partial `ScriptState` views previously redefined inside each
 * SeqOp_*.c file. Offsets and access widths are preserved exactly so every
 * field access compiles to the same load/store (byte-match holds). Still
 * intentionally partial; gaps are explicit pads. Total 0xBA is a lower bound,
 * not the true track-struct size, but nothing does pointer arithmetic on
 * AkaoScriptState* so the trailing size is not load-bearing.
 */

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef signed short   s16;

typedef struct AkaoScriptState {
    /* 0x00 */ u8 *pc;                /* script program counter */
    /* 0x04 */ u8 pad04[0x10];
    /* 0x14 */ u8 *branch_target;     /* computed jump/loop target */
    /* 0x18 */ u8 pad18[0x20];
    /* 0x38 */ u32 flags;             /* track status flags (bit 3 = SetBranchTarget) */
    /* 0x3C */ u8 pad3C[0xA];
    /* 0x46 */ s16 loop_counter;
    /* 0x48 */ u8 pad48[0x22];
    /* 0x6A */ s16 loop_counter_saved;
    /* 0x6C */ u8 pad6C[0x28];
    /* 0x94 */ u16 pan_current;
    /* 0x96 */ u16 pan_duration;
    /* 0x98 */ s16 pan_delta;
    /* 0x9A */ u8 pad9A[0xC];
    /* 0xA6 */ u16 expr_current;
    /* 0xA8 */ u16 expr_duration;
    /* 0xAA */ s16 expr_delta;
    /* 0xAC */ u8 padAC[0x8];
    /* 0xB4 */ u16 pitch_current;
    /* 0xB6 */ u16 pitch_duration;
    /* 0xB8 */ s16 pitch_delta;
} AkaoScriptState;

#endif /* PE1_AKAO_SCRIPT_H */
