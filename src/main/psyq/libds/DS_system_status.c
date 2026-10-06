/* Psy-Q LIBDS DSSYS_1.OBJ: DS_system_status. */

#include "pe1/psyq_cd.h"

u32 DS_system_status(u32 mode) {
    u32 offset;
    u32 table_page;

    offset = mode << 2;
    asm volatile("" : : "r"(offset));
    /* g_DsReadStatusBlock is at 0x8009B574 in the USA image. */
    table_page = 0x800A0000u;
    asm volatile("" : "=r"(table_page) : "0"(table_page));
    return *(u32 *)(table_page + offset - 0x4A8Cu);
}
