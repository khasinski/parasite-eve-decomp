#include "common.h"
#include "pe1/field_actor.h"
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef FieldActor Entity;

Entity *g_FieldActorListHead;
Entity *g_EntityFreeListHead;
Entity *g_PlayerEntity;
u16 g_EntityFreePoolCount;
int g_FieldMoveLock;

void Scene_FreeEntityTable(Entity *arg0);
void Entity_FreeAllocationBlock(int arg0);
int Util_ReturnTrue(void *unused);

void Entity_CollectGarbage(void) {
    Entity *cur;
    Entity *next;
    Entity *head_next;
    Entity *free_head;
    Entity *parent;
    int flags;

    cur = g_FieldActorListHead;
    if (cur != 0) {
        do {
            flags = cur->flags;
            parent = cur->parent;
            cur->flags = flags & ~0x800000;

            if (((parent != 0) && (parent->flags & 0x10)) || (flags & 0x10)) {
                Scene_FreeEntityTable(cur);
                next = cur->next;

                if (cur == g_PlayerEntity) {
                    g_PlayerEntity = 0;
                    g_FieldMoveLock &= ~0xD;
                }

                if (cur->allocation_active != 0) {
                    Entity_FreeAllocationBlock(cur->allocation_block);
                    Util_ReturnTrue(&cur->render_object);
                }

                if (cur->prev == 0) {
                    head_next = cur->next;
                    free_head = g_EntityFreeListHead;
                    g_EntityFreeListHead = cur;
                    g_FieldActorListHead = head_next;
                    head_next->prev = 0;
                    cur->next = free_head;
                } else {
                    cur->prev->next = cur->next;
                    if (cur->next != 0) {
                        cur->next->prev = cur->prev;
                    }
                    cur->next = g_EntityFreeListHead;
                    g_EntityFreeListHead = cur;
                }

                g_EntityFreePoolCount--;
                cur = next;
            } else {
                cur = cur->next;
            }
        } while (cur != 0);
    }
}
