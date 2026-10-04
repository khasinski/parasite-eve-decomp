#include "pe1/field_particle_chain.h"
#include "pe1/gte.h"

/* Lay the chain's links out from the record matrix: each link gets its end
 * points, then turns by its own tilt (or, from the third link on, a random
 * yaw away from the player) and steps down by the record depth. */
void func_800C5538(FieldChainRecord *record)
{
    GteShortVector tailIn;
    GteShortVector tail;
    GteShortVector headIn;
    GteShortVector head;
    GteMatrix matrix;
    GteShortVector angles;
    GteMatrix rotation;
    GteVector previous;
    GteVector toPlayer;
    GteVector cross;
    FieldChainLink *link;
    unsigned int i;
    int run;
    int turn;

    link = record->links;
    memset(&tail, 0, sizeof(tail));
    tail.x = -record->length;
    tail.y = 0;
    tail.z = 0;
    tailIn = tail;
    memset(&head, 0, sizeof(head));
    head.x = record->length;
    head.y = 0;
    head.z = 0;
    headIn = head;
    matrix = record->matrix;
    record->field0E = 0;
    for (i = 0; i < record->count; i++, link++) {
        ApplyMatrixSV(&matrix, &tailIn, &tail);
        ApplyMatrixSV(&matrix, &headIn, &head);
        link->tail[0] = head.x + matrix.t[0];
        link->tail[1] = head.y + matrix.t[1];
        link->tail[2] = head.z + matrix.t[2];
        link->head[0] = tail.x + matrix.t[0];
        link->head[1] = tail.y + matrix.t[1];
        link->head[2] = tail.z + matrix.t[2];
        if (i >= 2)
            record->bend = 1;
        else
            record->bend = 0;
        if (record->bend == 1) {
            previous.x = link[-1].matrix.t[0] - link[-2].matrix.t[0];
            previous.y = 0;
            previous.z = link[-1].matrix.t[2] - link[-2].matrix.t[2];
            toPlayer.x = D_8009D254->x.part.integer - link[-2].matrix.t[0];
            toPlayer.y = 0;
            toPlayer.z = D_8009D254->z.part.integer - link[-2].matrix.t[2];
            OuterProduct0(&previous, &toPlayer, &cross);
            run = Gte_ISqrt(previous.x * previous.x + previous.z * previous.z);
            Gte_Atan2(run, Gte_ISqrt(toPlayer.x * toPlayer.x + toPlayer.z * toPlayer.z));
            if (cross.y > 0)
                turn = rand() % 512 + 0x40;
            else
                turn = -(rand() % 512 + 0x40);
            angles.y = turn;
            angles.x = 0;
            angles.z = 0;
        } else {
            angles.x = link->tiltX;
            angles.y = link->tiltY;
            angles.z = 0;
        }
        if (i >= 6) {
            angles.x = 0;
            angles.y = 0;
            angles.z = 0;
        }
        RotMatrix(&angles, &rotation);
        rotation.t[0] = 0;
        rotation.t[1] = 0;
        rotation.t[2] = -record->depth;
        gte_CompMatrix(&matrix, &rotation, &matrix);
        link->matrix = matrix;
    }
}
