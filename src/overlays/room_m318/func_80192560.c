typedef unsigned char u8;

extern char *D_8009D20C;
extern char *D_8009D254;
extern char D_8018F1CC;

void func_80071A74(char *message, int arg1, int arg2);
void func_800CE870(char *entity, int arg1);

char *func_80192560(int arg0, int arg1) {
    register int entity_kind asm("$7");
    char *entity;

    entity_kind = arg1;
    entity = D_8009D20C;
    if (entity != 0) {
        do {
            if (entity != D_8009D254 && *(char **)entity != 0 && *(int *)(*(char **)entity + 0x10) > 0 &&
                *(u8 *)(entity + 0xD) == arg0 && *(u8 *)(entity + 0xC) == entity_kind) {
                func_800CE870(entity, 0);
                return entity;
            }
            entity = *(char **)(entity + 4);
        } while (entity != 0);
    }

    func_80071A74(&D_8018F1CC, arg0, entity_kind);
    return 0;
}
