/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* Psy-Q LIBDS DSSYS_1.OBJ: LIBDS_DSSYS_1_text_7FC. */
extern int D_8009B598[];
extern void LIBDS_DSSYS_1_text_EA4(int);
extern void LIBDS_DSSYS_1_text_8B8(int, void *);
extern void LIBDS_DSSYS_1_text_A9C(int, void *);
#include "pe1/psyq_cd.h"

void LIBDS_DSSYS_1_text_7FC(int inputEvent, void *inputResult) {
    void *result = inputResult;
    int event = inputEvent;
    int *state;

    LIBDS_DSSYS_1_text_EA4((unsigned char)event);
    /* The timeout symbol anchors the surrounding CD command state. */
    state = D_8009B598;
    state[1] = 0;
    state[0] = 0;
    if (((unsigned char *)state)[-44] & 0x10) event = 5;
    switch (state[-10]) {
    case 0x1F:
        LIBDS_DSSYS_1_text_8B8((unsigned char)event, result);
        break;
    case 0x20:
        LIBDS_DSSYS_1_text_A9C((unsigned char)event, result);
        break;
    default:
        LIBDS_DSSYS_1_text_D24((unsigned char)event, result);
        break;
    }
    state = D_8009B598;
    if (!state[0]) state[-10] = 0x21;
}
