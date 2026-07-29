typedef int s32;
typedef unsigned int size_t;

typedef struct DecDCTEnv {
    unsigned char iqY[64];
    unsigned char iqC[64];
    short dct[64];
} DecDCTEnv;

extern s32 D_8010DA0C[];
extern s32 D_8010DA90[];

static inline void CopyWords(long *dst, long *src, size_t count) {
    while (count--) {
        *dst++ = *src++;
    }
}

s32 *func_8010BE70(DecDCTEnv *env) {
    CopyWords((long *)&env->iqY, (long *)&D_8010DA0C[1], 16);
    CopyWords((long *)&env->iqC, (long *)&D_8010DA0C[17], 16);
    CopyWords((long *)&env->dct, (long *)&D_8010DA90[1], 32);
    return (s32 *)env;
}
