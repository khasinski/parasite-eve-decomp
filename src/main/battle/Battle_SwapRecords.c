/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 */

typedef struct {
    int field_0;
    int field_4;
    int field_8;
} Record;

void Battle_SwapRecords(char *records, int from_arg, int to_arg) {
    Record tmp;
    int from_index;
    int to_index;

    from_index = (signed char)from_arg;
    to_index = (signed char)to_arg;

    tmp = *(Record *)(from_index * sizeof(Record) + records);
    *(Record *)(from_index * sizeof(Record) + records) = *(Record *)(to_index * sizeof(Record) + records);
    *(Record *)(to_index * sizeof(Record) + records) = tmp;
}
