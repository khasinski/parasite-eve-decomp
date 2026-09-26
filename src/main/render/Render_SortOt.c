/* Matching debt: register pins, empty scheduling barriers, an $a0 clobber,
 * volatile packed-word rereads and a 32-byte artificial frame reservation. */
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
#include "pe1/render_prim.h"
extern int D_8009CDB8;
extern int D_8009CDBC;
extern int D_8009CDC0;
extern int D_8009CDC4;
int Render_SortOt(PrimObj *input, u32 *entries, u8 direction, u8 restart) {
    register PrimObj *entry = input;
    u8 ascending = direction;
    int count;
    int i;
    register int best_index asm("$10");
    register int descending_index asm("$11");
    register int best_x;
    register int best_y;
    int y;
    int x;
    u8 has_later_x;
    register int step_y;
    int previous;
    u32 value;
    register int selected_flag asm("$3");
    u32 frameReserve[8];

    asm("" : "=m"(frameReserve[0]) : : "$4");
    best_x = 0x7FFF;
    best_index = 0;
    descending_index = 0;
    if (restart != 0) {
        if (ascending == 0) {
            best_y = -1;
            best_x = -1;
            {
                register int loaded = entry->count;
                i = 0;
                if (best_index < loaded) {
                    asm("" : : "r"(best_index));
                    best_index = loaded;
                    {
                        register u32 *cursor asm("$6") = entries;
                        do {
                            int y = (*cursor >> 12) & 0x3FF;
                            if (best_y < y) {
                                best_y = y;
                            }
                            ++i;

                            ++cursor;
                        } while (i < best_index);
                    }
                }
            }
            count = entry->count;
            asm("" : "=r"(count) : "0"(count));
            for (i = 0; i < count; i++) {
                u32 value = entries[i];
                int x;
                int y;
                y = (value >> 12) & 0x3FF;
                x = value >> 22;
                if ((y == best_y) && (best_x < x)) {
                    best_x = x;
                    descending_index = i;
                }
            }

            {

                register int result = descending_index;

                previous = D_8009CDBC;

                D_8009CDBC = result;
                D_8009CDB8 = previous;

                {
                    u32 packed_x = *(volatile u32 *)&entries[result];
                    u32 packed_y = *(volatile u32 *)&entries[result];
                    D_8009CDC0 = packed_x >> 22;
                    D_8009CDC4 = (packed_y >> 12) & 0x3FF;
                }
                return result;
            }
        } else {
            best_y = 0x7FFF;
            {
                register int loaded asm("$3") = entry->count;
                i = 0;
                if (best_index < loaded) {
                    descending_index = loaded;
                    {
                        register u32 *cursor asm("$6") = entries;
                        do {
                            int y = (*cursor >> 12) & 0x3FF;
                            if (y < best_y) {
                                best_y = y;
                            }
                            ++i;

                            ++cursor;
                        } while (i < descending_index);
                    }
                }
            }
            count = entry->count;
            asm("" : "=r"(count) : "0"(count));
            for (i = 0; i < count; i++) {
                u32 value = entries[i];
                int x;
                int y;
                y = (value >> 12) & 0x3FF;
                x = value >> 22;
                if ((y == best_y) && (x < best_x)) {
                    best_x = x;
                    best_index = i;
                }
            }
        }
        goto selected;
    }
    step_y = 0x10;
    if (ascending == 0) {
        step_y = -0x10;
    }
    descending_index = 0;
    has_later_x = 0;
    {
        register int loaded = entry->count;
        i = 0;
        if (best_index < loaded) {
            register int bound = loaded;
            do {
                register int raw_y;

                value = entries[i];
                x = value >> 22;

                if (x == (D_8009CDC0 + step_y)) {
                    raw_y = value >> 12;
                    if ((raw_y & 0x3FF) == D_8009CDC4) {
                        register int result asm("$2") = i;
                        previous = D_8009CDBC;
                        D_8009CDBC = result;
                        D_8009CDB8 = previous;
                        asm volatile("" : : : "memory");
                        value = entries[i];
                        D_8009CDC0 = entries[i] >> 22;
                        D_8009CDC4 = (entries[i] >> 12) & 0x3FF;
                        return result;
                    }
                } else {
                    raw_y = value >> 12;
                    if (((raw_y & 0x3FF) == D_8009CDC4) && (D_8009CDC0 < x)) {
                        has_later_x = 1;
                    }
                }

                ++i;
            } while (i < bound);
        }
    }
    if ((!((u8)descending_index)) && (has_later_x == 1)) {
        if (ascending == 0) {
            best_x = 0;
            {
                register int loaded = entry->count;
                i = 0;

                if (loaded > 0) {
                    register int bound = loaded;
                    register u32 *cursor = entries;
                    register int state_y asm("$4") = D_8009CDC4;
                    register int state_x = D_8009CDC0;
                    do {
                        u32 value = *cursor;
                        int x;
                        int y;
                        y = (value >> 12) & 0x3FF;
                        x = value >> 22;
                        if (((y == state_y) && (x < state_x)) && (best_x < x)) {
                            best_x = x;
                            best_index = i;
                            descending_index = 1;
                        }

                        ++i;
                        ++cursor;
                    } while (i < bound);
                }
            }

        } else {
            best_x = 0x7FFF;
            {
                register int loaded = entry->count;
                i = 0;

                if (loaded > 0) {
                    register int bound = loaded;
                    register u32 *cursor = entries;
                    register int state_y asm("$4") = D_8009CDC4;
                    register int state_x = D_8009CDC0;
                    do {
                        u32 value = *cursor;
                        int x;
                        int y;
                        y = (value >> 12) & 0x3FF;
                        x = value >> 22;
                        if (((y == state_y) && (state_x < x)) && (x < best_x)) {
                            best_x = x;
                            best_index = i;
                            descending_index = 1;
                        }

                        ++i;
                        ++cursor;
                    } while (i < bound);
                }
            }
        }
    }
    selected_flag = (u8)descending_index;

    if (selected_flag == 1)
        goto selected;
    {
        descending_index = 0;
        if (ascending == 0) {
            best_x = -1;
            {
                register int loaded = entry->count;

                i = 0;
                if (loaded > 0) {
                    register int bound = loaded;
                    y = D_8009CDC4 + step_y;
                    {
                        register u32 *cursor asm("$8") = entries;
                        do {
                            u32 value = *cursor;
                            int x;
                            if (((value >> 12) & 0x3FF) == y) {
                                x = value >> 22;
                                if (best_x < x) {
                                    best_x = x;
                                    best_index = i;
                                    descending_index = 1;
                                }
                            }
                            ++i;

                            ++cursor;
                        } while (i < bound);
                    }
                }
            }

        } else {
            best_x = 0x7FFF;
            {
                register int loaded asm("$2") = entry->count;

                i = 0;
                if (loaded > 0) {
                    register int bound asm("$4") = loaded;
                    y = D_8009CDC4 + step_y;
                    {
                        register u32 *cursor = entries;
                        do {
                            u32 value = *cursor;
                            int x;
                            if (((value >> 12) & 0x3FF) == y) {
                                x = value >> 22;
                                if (x < best_x) {
                                    best_x = x;
                                    best_index = i;
                                    descending_index = 1;
                                }
                            }
                            ++i;

                            ++cursor;
                        } while (i < bound);
                    }
                }
            }
        }
    }
    if (((u8)descending_index) == 0) {
        return Render_SortOt(entry, entries, ascending, 1);
    }
    asm("" : : "r"(descending_index));
selected: {
    register int result = best_index;

    previous = D_8009CDBC;

    D_8009CDBC = result;
    D_8009CDB8 = previous;

    {
        u32 packed_x = *(volatile u32 *)&entries[result];
        u32 packed_y = *(volatile u32 *)&entries[result];
        D_8009CDC0 = packed_x >> 22;
        D_8009CDC4 = (packed_y >> 12) & 0x3FF;
    }
    return result;
}
}