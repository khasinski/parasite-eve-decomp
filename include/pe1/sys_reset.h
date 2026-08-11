#ifndef PE1_SYS_RESET_H
#define PE1_SYS_RESET_H

#include "common.h"

extern s32 D_8010DA0C[];
extern s32 D_8010DA10[16];
extern s32 D_8010DA50[16];
extern s32 D_8010DA90[];
extern char D_8010BD18[];
extern char D_8010BD28[];
extern char D_8010BCFC[];
extern s32 *volatile *D_8010DB1C;
extern volatile u32 *D_8010DB20;
extern volatile u32 *D_8010DB24;
extern s32 *volatile *D_8010DB28;
extern volatile u32 *D_8010DB2C;
extern volatile u32 *D_8010DB30;
extern volatile u32 *D_8010DB4C;
extern volatile u32 *D_8010DB50;
extern volatile u32 *D_8010DB54;

void func_8010C1EC(s32 *data, u32 size);
int func_8010C308(void);
int func_8010C39C(void);
void func_8010C448(char *message);
void func_80071A74(void *message, int mode);

#endif
