#include "fx_common_motion.h"
#include "fx_common_render.h"

#define DRAW(node, pass, force, mirrored) \
    func_80190E04((node), D_8019C9C0, (pass), (force), (mirrored))
#define DRAW_PAIR(node, pass, mode) \
    func_80191114((node), D_8019C9C0, (pass), (mode))

/* Draw every scene node for this frame, each with its depth bias: the
 * backdrop, the turning fans, the static props, the camera path nodes and
 * their echoes, the effect node pairs and the overlay nodes. */
void func_80192800(void)
{
    int i;

    if (D_8019C034 != 0) {
        D_801EA5E4 = 0xFA1;
        DRAW(D_8019CC28, 1, 1, 0);
    }
    D_801EA5E4 = 0xFA0;
    DRAW(g_FxCommonRuntime.node25, 1, 1, 0);
    D_801EA5E4 = 0xF9F;
    DRAW(g_FxCommonType26Nodes[0], 1, 1, 0);
    DRAW(g_FxCommonType26Nodes[1], 1, 1, 0);
    DRAW(g_FxCommonType26Nodes[2], 1, 1, 0);
    DRAW(g_FxCommonType26Nodes[3], 1, 1, 0);
    g_FxCommonType26Nodes[0]->state[0] += 10;
    g_FxCommonType26Nodes[1]->state[0] += 2;
    g_FxCommonType26Nodes[2]->state[0] += 40;
    g_FxCommonType26Nodes[3]->state[0] += 30;
    if (g_FxCommonType26Nodes[0]->state[0] > 0x6EE0)
        g_FxCommonType26Nodes[0]->state[0] = -0x6328;
    if (g_FxCommonType26Nodes[1]->state[0] > 0x6EE0)
        g_FxCommonType26Nodes[1]->state[0] = -0x6328;
    if (g_FxCommonType26Nodes[2]->state[0] > 0x6EE0)
        g_FxCommonType26Nodes[2]->state[0] = -0x6328;
    if (g_FxCommonType26Nodes[3]->state[0] > 0x6EE0)
        g_FxCommonType26Nodes[3]->state[0] = -0x6328;

    D_801EA5E4 = 1000;
    DRAW(D_8019CDA4, 0, 1, 0);
    DRAW(D_8019CDAC, 0, 1, 0);
    D_801EA5E4 = 0xF3B;
    DRAW(D_8019CDA0, 1, 1, 0);
    DRAW(D_8019CDA8, 1, 1, 0);
    D_801EA5E4 = 0xF3A;
    DRAW(D_8019CDB0, 1, 1, 0);
    D_801EA5E4 = 0xF39;
    DRAW(D_8019CDB4, 1, 1, 0);
    D_801EA5E4 = 0;
    DRAW(D_8019CDB8, 0, 1, 0);
    DRAW(D_8019CDBC, 0, 1, 0);
    DRAW(D_8019CDC0, 0, 1, 0);
    DRAW(D_8019CDC4, 0, 1, 0);
    DRAW(D_8019CDC8, 0, 1, 0);
    DRAW(D_8019C180, 0, 1, 0);

    if (g_FxCommonPathNodes[0]->kind == 0x24) {
        g_FxCommonPathNodes[0]->kind = 0x52;
        g_FxCommonPathNodes[1]->kind = 0x52;
        g_FxCommonPathNodes[2]->kind = 0x52;
        g_FxCommonPathNodes[3]->kind = 0x52;
    } else {
        g_FxCommonPathNodes[0]->kind = 0x24;
        g_FxCommonPathNodes[1]->kind = 0x24;
        g_FxCommonPathNodes[2]->kind = 0x24;
        g_FxCommonPathNodes[3]->kind = 0x24;
    }
    D_801EA5E4 = 0;
    DRAW(g_FxCommonPathNodes[0], 0, 0, 0);
    if (D_8019C034 != 2)
        DRAW(g_FxCommonPathNodes[2], 0, 0, 0);
    D_801EA5E4 = 1000;
    DRAW(g_FxCommonPathNodes[1], 0, 0, 1);
    if (D_8019C034 != 2)
        DRAW(g_FxCommonPathNodes[3], 0, 0, 1);

    D_801EA5E4 = 0;
    DRAW_PAIR(D_8019C820, 1, 0);
    DRAW_PAIR(g_FxCommonPairedNodes[0], 1, 0);
    D_801EA5E4 = 1000;
    DRAW_PAIR(D_8019C824, 1, 2);
    DRAW_PAIR(g_FxCommonPairedNodes[1], 1, 2);

    D_801EA5E4 = 0;
    DRAW(g_FxCommonPathTailNodes[0], 0, 0, 0);
    DRAW(g_FxCommonPathTailNodes[1], 0, 0, 0);
    D_801EA5E4 = 10;
    DRAW(g_FxCommonPathTailNodes[0], 0, 0, 1);
    DRAW(g_FxCommonPathTailNodes[1], 0, 0, 1);
    D_801EA5E4 = 0;
    DRAW(D_8019CC20, 0, 1, 0);
    D_801EA5E4 = 800;
    DRAW(D_8019CC24, 0, 1, 1);

    if (D_8019C1F0 == 1) {
        D_8019C0D0[0] += (s16)(func_80071A54() % 40 + 20);
        D_8019C0D0[1] += (s16)(func_80071A54() % 52 + 26);
        g_FxCommonEchoNodes[0]->matrix = g_FxCommonPathNodes[0]->matrix;
        g_FxCommonEchoNodes[0]->seed = g_FxCommonPathNodes[0]->seed;
        g_FxCommonEchoNodes[0]->kind = 0x53;
        g_FxCommonEchoNodes[2]->matrix = g_FxCommonPathNodes[2]->matrix;
        g_FxCommonEchoNodes[2]->seed = g_FxCommonPathNodes[2]->seed;
        g_FxCommonEchoNodes[2]->kind = 0x53;
        g_FxCommonEchoNodes[1]->matrix = g_FxCommonPathNodes[0]->matrix;
        g_FxCommonEchoNodes[1]->seed = g_FxCommonPathNodes[0]->seed;
        g_FxCommonEchoNodes[1]->kind = 0x53;
        g_FxCommonEchoNodes[3]->matrix = g_FxCommonPathNodes[2]->matrix;
        g_FxCommonEchoNodes[3]->seed = g_FxCommonPathNodes[2]->seed;
        g_FxCommonEchoNodes[3]->kind = 0x53;
        D_801EA5E4 = 0;
        DRAW(g_FxCommonEchoNodes[0], 0, 0, 0);
        D_801EA5E4 = 800;
        DRAW(g_FxCommonEchoNodes[1], 0, 0, 1);
    }

    for (i = 0; i < D_8019C020 >> 1; i++) {
        D_801EA5E4 = 0;
        if (D_800A77FC & 0x2000) {
            D_801EA264[0] = 60;
            D_801EA264[1] = 60;
            D_801EA264[2] = 40;
        } else {
            D_801EA264[0] = 20;
            D_801EA264[1] = 40;
            D_801EA264[2] = 40;
        }
        DRAW_PAIR(D_8019C3B0[i * 2], 0, 0);
        if (D_8019C032 == 1 && (i & 3)) {
            D_801EA5E4 = 800;
            DRAW_PAIR(D_8019C3B0[i * 2 + 1], 0, 1);
        }
    }

    D_801EA5E4 = 0;
    for (i = 0; i < 8; i++)
        DRAW_PAIR(g_FxCommonRuntime.pairedNodes[i], 0, 0);
    D_801EA5E4 = 0x23A;
    DRAW_PAIR(g_FxCommonRuntime.pairedNodes[8], 0, 0);
    D_801EA5E4 = 0;
    DRAW_PAIR(g_FxCommonRuntime.pairedNodes[9], 0, 0);
    if (D_8019C032 == 1) {
        D_801EA5E4 = 800;
        if (D_8019C044) {
            DRAW_PAIR(g_FxCommonRuntime.pairedNodes[11], 0, 1);
            DRAW_PAIR(g_FxCommonRuntime.pairedNodes[12], 0, 1);
            DRAW_PAIR(g_FxCommonRuntime.pairedNodes[13], 0, 1);
            DRAW_PAIR(g_FxCommonRuntime.pairedNodes[14], 0, 1);
            DRAW_PAIR(g_FxCommonRuntime.pairedNodes[16], 0, 1);
            DRAW_PAIR(g_FxCommonRuntime.pairedNodes[17], 0, 1);
            DRAW_PAIR(g_FxCommonRuntime.pairedNodes[18], 0, 1);
            DRAW_PAIR(g_FxCommonRuntime.pairedNodes[19], 0, 1);
        }
        DRAW_PAIR(g_FxCommonRuntime.pairedNodes[15], 0, 1);
    }
    D_801EA5E0 = 500;
    func_80190D3C(D_801EA260, D_8019C9C0);
}
