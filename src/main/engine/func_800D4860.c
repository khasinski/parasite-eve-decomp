extern void *D_800B0E20;
extern int D_800E22D4;
extern int D_800F32D4;
extern int D_800F32D8;
extern int D_800F3418;
extern int D_800F342C;
extern int D_800F3474;

int Asset_FindTable08ByU32Key(void *arg0, int key);
void func_800CECAC(void);
void func_800CED3C(int arg0);

int func_800D4860(void) {
    void **table;

    func_800CECAC();

    table = &D_800B0E20;
    D_800F32D4 = Asset_FindTable08ByU32Key(*table, 0x6DEAE684);
    D_800F32D8 = Asset_FindTable08ByU32Key(*table, 0x8AB5B684);
    D_800F3418 = Asset_FindTable08ByU32Key(*table, 0x5AB32504);
    D_800F342C = Asset_FindTable08ByU32Key(*table, 0x5AB32104);
    D_800F3474 = Asset_FindTable08ByU32Key(*table, 0x83ACD504);
    D_800E22D4 = Asset_FindTable08ByU32Key(*table, 0x57E8EB04);
    func_800CED3C(0);

    return 0;
}
