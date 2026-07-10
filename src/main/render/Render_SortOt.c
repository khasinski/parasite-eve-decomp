typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern int D_8009CDB8;
extern int D_8009CDBC;
extern int D_8009CDC0;
extern int D_8009CDC4;

#define SORT_X(value) ((value) >> 22)
#define SORT_Y(value) (((value) >> 12) & 0x3FF)

int Render_SortOt(void *entry, u32 *entries, int ascending, int continue_from_state) {
    int count;
    int i;
    int best_index;
    int best_x;
    int best_y;
    int y;
    int x;
    int found;
    int has_later_x;
    int step_y;
    int previous;
    u32 value;

    count = *(u16 *)((char *)entry + 0x26);
    ascending = (u8)ascending;
    continue_from_state = (u8)continue_from_state;
    best_index = 0;
    best_x = 0x7FFF;
    best_y = 0x7FFF;

    if (continue_from_state != 0) {
        if (ascending == 0) {
            best_x = -1;
            best_y = -1;

            for (i = 0; i < count; i++) {
                y = SORT_Y(entries[i]);
                if (best_y < y) {
                    best_y = y;
                }
            }

            for (i = 0; i < count; i++) {
                value = entries[i];
                y = SORT_Y(value);
                x = SORT_X(value);
                if (y == best_y && best_x < x) {
                    best_x = x;
                    best_index = i;
                }
            }
        } else {
            best_y = 0x7FFF;
            best_x = 0x7FFF;

            for (i = 0; i < count; i++) {
                y = SORT_Y(entries[i]);
                if (y < best_y) {
                    best_y = y;
                }
            }

            for (i = 0; i < count; i++) {
                value = entries[i];
                y = SORT_Y(value);
                x = SORT_X(value);
                if (y == best_y && x < best_x) {
                    best_x = x;
                    best_index = i;
                }
            }
        }

        previous = D_8009CDBC;
        D_8009CDBC = best_index;
        D_8009CDB8 = previous;
        value = entries[best_index];
        D_8009CDC0 = SORT_X(value);
        D_8009CDC4 = SORT_Y(value);
        return best_index;
    }

    step_y = ascending != 0 ? 0x10 : -0x10;
    found = 0;
    has_later_x = 0;

    for (i = 0; i < count; i++) {
        value = entries[i];
        x = SORT_X(value);
        y = SORT_Y(value);
        if (x == D_8009CDC0 + step_y && y == D_8009CDC4) {
            previous = D_8009CDBC;
            D_8009CDBC = i;
            D_8009CDB8 = previous;
            value = entries[i];
            D_8009CDC0 = SORT_X(value);
            D_8009CDC4 = SORT_Y(value);
            return i;
        }
        if (y == D_8009CDC4 && D_8009CDC0 < x) {
            has_later_x = 1;
        }
    }

    if (!found && has_later_x == 1) {
        if (ascending == 0) {
            best_x = 0;
            for (i = 0; i < count; i++) {
                value = entries[i];
                y = SORT_Y(value);
                x = SORT_X(value);
                if (y == D_8009CDC4 && x < D_8009CDC0 && best_x < x) {
                    best_x = x;
                    best_index = i;
                    found = 1;
                }
            }
        } else {
            best_x = 0x7FFF;
            for (i = 0; i < count; i++) {
                value = entries[i];
                y = SORT_Y(value);
                x = SORT_X(value);
                if (y == D_8009CDC4 && D_8009CDC0 < x && x < best_x) {
                    best_x = x;
                    best_index = i;
                    found = 1;
                }
            }
        }
    }

    if (found != 1) {
        if (ascending == 0) {
            best_x = -1;
            y = D_8009CDC4 + step_y;
            for (i = 0; i < count; i++) {
                value = entries[i];
                if (SORT_Y(value) == y) {
                    x = SORT_X(value);
                    if (best_x < x) {
                        best_x = x;
                        best_index = i;
                        found = 1;
                    }
                }
            }
        } else {
            best_x = 0x7FFF;
            y = D_8009CDC4 + step_y;
            for (i = 0; i < count; i++) {
                value = entries[i];
                if (SORT_Y(value) == y) {
                    x = SORT_X(value);
                    if (x < best_x) {
                        best_x = x;
                        best_index = i;
                        found = 1;
                    }
                }
            }
        }
    }

    if (found == 0) {
        Render_SortOt(entry, entries, ascending, 1);
        return D_8009CDBC;
    }

    previous = D_8009CDBC;
    D_8009CDBC = best_index;
    D_8009CDB8 = previous;
    value = entries[best_index];
    D_8009CDC0 = SORT_X(value);
    D_8009CDC4 = SORT_Y(value);
    return best_index;
}
