extern unsigned char *Item_LookupBaseData(int index);

int Item_GetBaseType(int index) {
    return Item_LookupBaseData(index - 1)[6];
}
