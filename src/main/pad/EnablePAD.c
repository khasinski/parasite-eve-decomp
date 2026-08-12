#include "pe1/psyq_api_internal.h"

void EnablePAD(void)
{
    jtbl_800A34C8();
}

void DisablePAD(void)
{
    jtbl_800A34CC();
}
