#include "pe1/gte.h"
#include "pe1/psyq_nop.h"
#include "pe1/render_object.h"
#include "pe1/field_movement.h"
#include "pe1/battle_runtime.h"
#include "pe1/render_camera.h"

/* Initialize camera projection and save the active actor screen position. */
int Render_PrepareFrame(void) {
    struct {
        short vector[4];
        void *result;
        union { int word; short xy[2]; } projected;
    } local;
    unsigned int *state = D_800B89F8;
    Menu_InitGlobals(state, state + 8);
    if (Geo_RenderMeshList((void *)D_800B0E40, &local.result)) return -2;
    SetGeomScreen(state[8]);
    func_800655D4();
    if (D_8009D254) {
        int x = D_8009D254->posX.parts.integer;
        int y, z;
        local.vector[0] = x;
        y = D_8009D254->posY.parts.integer;
        local.vector[1] = y;
        z = D_8009D254->posZ.parts.integer;
        local.vector[2] = z;
    } else local.vector[0] = local.vector[1] = local.vector[2] = 0;
    {
        /* Narrow volatile input preserves the retail stack frame in GCC 2.7.2. */
        register volatile unsigned short cx asm("$6") = 160;
        register unsigned int cy asm("$7") = 112;
        register unsigned int sx asm("$12");
        register unsigned int sy asm("$13");
        asm("" : "=r"(cx), "=r"(cy) : "0"(cx), "1"(cy) : "memory");
        sx = cx<<16;
        sy = cy<<16;
        gte_ctc2_24(sx);
        gte_ctc2_25(sy);
    }
    {
        int **address = &D_800BCFA4.value;
        register int *matrix asm("$6");
        register unsigned int a asm("$12");
        register unsigned int b asm("$13");
        register unsigned int c asm("$14");
        asm("" : "=r"(address) : "0"(address));
        matrix = *address;
        a = matrix[0];
        b = matrix[1];
        gte_ctc2_0(a);
        gte_ctc2_1(b);
        a = matrix[2];
        b = matrix[3];
        c = matrix[4];
        gte_ctc2_2(a);
        gte_ctc2_3(b);
        gte_ctc2_4(c);
        a = matrix[5];
        b = matrix[6];
        gte_ctc2_5(a);
        c = matrix[7];
        gte_ctc2_6(b);
        gte_ctc2_7(c);
    }
    gte_lwc2_0_0(local.vector);
    gte_lwc2_1_4(local.vector);
    PE1_NOP();
    PE1_NOP();
    gte_rtps_command();
    gte_stsxy2(&local.projected);
    {
        int ret = 0;
        int word;
        int x;
        word = local.projected.word;
        x = local.projected.xy[0];
        D_800BCFB4 = x;
        D_800BCFB6 = word >> 16;
        return ret;
    }
}
