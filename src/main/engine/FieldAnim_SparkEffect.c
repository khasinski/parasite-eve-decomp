#include "common.h"
void **FieldEng_GetSlot(void);
extern int D_800E0FFC;
extern int D_800E22B8;
extern int D_800E22BC;
extern int D_800E22C0;
extern short D_800E22B0;
extern short D_800E22B2;
extern short D_800E22B4;
extern u8 D_800E22C8;
extern u8 D_800E22C9;
extern u8 D_800E22CA;
extern u8 D_800E22CD;
extern short D_800E22CE;
int func_800CE084(void)
{
  void **slot = FieldEng_GetSlot();
  register int value;
  register void *slotData;
  slotData = &D_800E0FFC;
 do { *slot = slotData; value = 0x300; D_800E22B8 = value; D_800E22BC = value; } while (0);
  D_800E22C0 = value;
  value = 5;
  D_800E22CD = value;
  value = -0x64;
  D_800E22CE = value;
  value = 0x80;
  D_800E22B0 = 0;
  D_800E22B2 = 0;
  D_800E22B4 = 0;
  D_800E22C8 = value;
  D_800E22C9 = value;
  D_800E22CA = value;
  return 0;
}


extern void FieldEng_Spawn6(int a, int b, int c, int d, int e, int f);

int func_800CE118(int a, int b, int c, int d, int e, int f) {
    FieldEng_Spawn6(a, b, c, d, e, f);
    return 0;
}


extern char D_800E0FC0[];
extern void FieldEng_Register(void *o, void *table);

int func_800CE144(void *o) {
    FieldEng_Register(o, D_800E0FC0);
    return 0;
}


extern char D_800E0FCC[];
extern char D_800E0FB4[];
extern char D_800E0FD8[];
extern int func_800C251C(void *obj, void *table);
extern int func_800C2758(void *obj, void *table, void *extra);
extern int func_800CE1DC(char *arg0);

int func_800CE16C(void *obj) {
    int first;
    int second;
    int status;

    first = func_800C251C(obj, D_800E0FCC);
    second = func_800C2758(obj, D_800E0FB4, D_800E0FD8);
    status = first | second;
    if (status == -1) {
        func_800CE1DC(obj);
    }
    return 0;
}


int func_800CE1DC(char *arg0) {
    *arg0 = 4;
    return 0;
}


int func_800CE1EC(void) {
    return 0;
}


int func_800CE1F4(void) {
    return 0;
}


#include "common.h"
int *func_800C2B10(int index);
int *func_800C2B28(int index);
void func_800CEDA8(int arg0);

extern int D_800E2848;
extern u16 D_800E2808;
extern u16 D_800E280A;
extern u16 D_800E280C;

int func_800CE1FC(void) {
    int index;
    int *table;
    int *entry;
    int *next;
    u8 *data;

    index = *func_800C2B10(0xD);
    table = (int *)*func_800C2B28(1);
    entry = (int *)table[index];
    index++;
    next = (int *)table[index];
    D_800E2848 = (int)entry;

    if (next == 0) {
        *func_800C2B10(0xE) = 1;
    }

    *func_800C2B10(0xD) = index;

    data = (u8 *)D_800E2848;
    D_800E2808 = *(u16 *)(data + 0x268);
    D_800E280A = *(u16 *)(data + 0x26A);
    D_800E280C = *(u16 *)(data + 0x26C);

    func_800CEDA8(0);
}


#include "common.h"
int rand(void);

extern u16 D_800E2808;
extern u16 D_800E280A;
extern u16 D_800E280C;

int func_800CE2B4(void *arg0, void *arg1, u8 *anim) {
    register int base asm("$5");
    register int r asm("$2");

    r = rand();
    base = D_800E2808 - 0x28;
    *(u16 *)(anim + 0x6) = base + (r % 80);

    r = rand();
    base = D_800E280A - 0x28;
    *(u16 *)(anim + 0x8) = base + (r % 80);

    r = rand();
    base = D_800E280C;

    anim[1] = 0x7F;
    *(u16 *)(anim + 0x4) = 0;
    anim[3] = 0;
    base -= 0x28;
    *(u16 *)(anim + 0xA) = base + (r % 80);
}


void func_800CE3AC(void) {
}


#include "common.h"
void func_800C2EAC(u8 arg0);
void func_800C3098(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3238(int arg0);
void func_800C3B04(void);

extern u16 D_800E22A8;
extern u16 D_800E22AA;
extern u16 D_800E22AC;
extern short D_800E22D0;
extern signed char D_800E22CC;

int func_800CE3B4(void *arg0, void *arg1, u8 *anim) {
    u16 value_v0;
    register u16 *output asm("$4");
    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    value_v0 = *(u16 *)(anim + 0x6);
    asm("" : : "r"(value_v0) : "$4");
    output = &D_800E22A8;
    asm("" : "=r"(output) : "0"(output));
    *output = value_v0;
    D_800E22AA = *(u16 *)(anim + 0x8);
    D_800E22AC = *(u16 *)(anim + 0xA);
    D_800E22D0 = (signed char)anim[1];
    D_800E22CC = (anim[3] * 2) - 0x60;

    func_800C3B04();
    func_800C3098(0x10);
}


int func_800CE464(void *arg0, char *arg1) {
    int value;

    value = 2;
    arg1[1] = value;
    return value;
}


void func_800CE470(void *arg0, char *arg1, unsigned char *arg2) {
    arg2[3]++;
    if ((signed char)arg2[3] == 6) {
        arg1[1] = 2;
    }
}
