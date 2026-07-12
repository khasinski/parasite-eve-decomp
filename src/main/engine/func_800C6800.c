typedef signed short s16;

int FieldEng_GetStatus(void *obj);
void *Asset_Find08w(void *asset, void *table, int x, int y, int z);

void *func_800C6800(char *obj, void *asset, char *pos) {
    void *result = 0;

    if (FieldEng_GetStatus(obj) == 3 || FieldEng_GetStatus(obj) == 4) {
        result = Asset_Find08w(asset, *(void **)(*(char **)(*(char **)(obj + 8) + 0) + 8),
                               *(s16 *)(pos + 0), *(s16 *)(pos + 2), *(s16 *)(pos + 4));
    }

    return result;
}
