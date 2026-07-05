typedef signed short s16;
typedef int s32;

typedef struct {
    char pad0[0x10];
    s16 random_mod;
    char pad12[0x2];
    s16 half14;
    char pad16[0xC];
    s16 half22;
    s16 resource2;
    s16 half26;
    s16 resource1_again;
    s16 half2A;
    s16 half2C;
    s16 half2E;
    s32 word30;
    s16 resource1;
} Overlay024Object;

s32 *func_800C2B10(s32 slot);
s32 func_80071A54(void);

void func_80102CAC(void *arg0, void *arg1, Overlay024Object *obj) {
    s32 random;

    obj->half2E = 0xD;
    obj->resource1 = *func_800C2B10(1);
    obj->resource2 = *func_800C2B10(2);
    obj->half26 = 1;
    obj->half2A = 0x80;
    obj->resource1_again = *func_800C2B10(1);
    obj->half2C = 0;
    obj->word30 = 0;
    obj->half22 = 0;
    obj->half14 = 0;
    random = func_80071A54();
    obj->random_mod = random % 40;
}
