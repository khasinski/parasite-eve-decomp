/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/task_node.h"

extern TaskNode *g_TaskNodePool;
extern char *g_CurrentEntity[];

int Entity_FindNodeById(unsigned short **arg0) {
    TaskNode *node;
    TaskNode *cur;
    register unsigned int id asm("$2");
    unsigned int bucket;
    unsigned int search_id;
    char *base;

    node = g_TaskNodePool;
    asm volatile("" : "=r"(node) : "0"(node));
    id = *arg0[0];
    if (node->seq == id) {
        node->flags |= 0x10;
        return 0;
    }

    base = g_CurrentEntity[0];
    search_id = id;
    bucket = 0;
    do {
        cur = *(TaskNode **)(base + 0xA0 + ((bucket & 0xFF) * 4));
        while (cur != 0) {
            if (cur->seq == search_id) {
                cur->flags |= 0x10;
                return 1;
            }
            cur = cur->next;
        }
        bucket++;
    } while ((bucket & 0xFF) < 3);

    return 1;
}
