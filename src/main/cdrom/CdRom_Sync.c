#include "pe1/psyq_cd.h"

void DS_sync(u8 *result) {
    CD_sync(1, result);
}

void DS_ready(u8 *result) {
    CD_ready(1, result);
}
