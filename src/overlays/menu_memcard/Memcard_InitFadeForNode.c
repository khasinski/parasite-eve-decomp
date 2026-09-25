#include "menu_memcard_fade.h"
#include "menu_memcard_node.h"

void Memcard_InitFadeForNode(MenuMemcardFadeState *state) {
    MenuMemcardListNode *node = D_801D1370;
    int kind = 7;

    state->displayTop = 0xB4;
    state->onStep = 0;
    state->phase = 0;
    state->velocity = 0;
    while (node != 0) {
        if (node->kind == kind) {
            break;
        }
        node = node->next;
    }
    node->fadeLevel = 0x100;
}
