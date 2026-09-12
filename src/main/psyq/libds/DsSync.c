/* ASSEMBLER: GNU */
/* Psy-Q DsSync. Retains the existing link symbol for callers.
 * Empty constraints preserve the retail schedule; see proposals/PsyqQueueFour. */
#include "pe1/psyq_ds_queue.h"

/* Word-offset views of the eight 16-byte history records. */
extern int D_800A3610[], D_800A3614[], D_800A3618[], D_800A361C[];
extern int D_800A3690;
extern DsResult D_800A3500;

s32 Render_FindParticleEffect(s32 inId, void *inResult) {
    register s32 id = inId;
    register void *result = inResult;
    register DsResult *selected asm("$16");
    register s32 cursor;
    register s32 scanned;
    register s32 searched;
    register s32 offset;
    register s32 status;
    register s32 entryIdOrOffset;
    register s32 forward;
    register s32 readiness;
    register s32 backward;

    if (id != 0) {
        CdRom_Sync(0);
        forward = D_800A3690;
        scanned = 0;
        entryIdOrOffset = forward * 0x10;
    scanForward:
        entryIdOrOffset = *(int *)((char *)D_800A3610 + entryIdOrOffset);
        forward += 1;
        if (entryIdOrOffset != id) {
            if (forward >= 8) {
                forward = 0;
            }
            scanned += 1;
            if (scanned >= 8) {
                readiness = 0;
                if (id < *(int *)((char *)D_800A3610 + (D_800A3690 * 0x10))) {
                    goto completed;
                }
            } else {
                entryIdOrOffset = forward * 0x10;
                goto scanForward;
            }
        } else {
        completed:
            readiness = 7;
        }
        if (readiness != 7) {
            status = 0;
            goto done;
        }
        {
            cursor = D_800A3690;
            backward = cursor - 1;
            if (backward < 0) {
                backward = 7;
            }
            searched = 0;
            if (id != 0) {
                offset = backward * 0x10;
            scanBackward:
                backward -= 1;
                if (*(int *)((char *)D_800A3610 + offset) != id) {
                    if (backward < 0) {
                        backward = 7;
                    }
                    searched += 1;
                    offset = backward * 0x10;
                    if (searched >= 8) {
                        selected = 0;
                    } else {
                        goto scanBackward;
                    }
                } else {
                    goto publishResult;
                }
            } else {
                offset = backward * 0x10;
                selected = 0;
                if (*(int *)((char *)D_800A3610 + offset) == 0) {

                } else {
                    goto publishResult;
                }
            }
            goto copyResult;
        }
    }
    cursor = D_800A3690 - 1;
    offset = cursor * 0x10;
    if (cursor < 0) {
        cursor = 7;
        asm("" : "=r"(cursor) : "0"(cursor));
        offset = cursor * 0x10;
    }
    selected = 0;
    if (*(int *)((char *)D_800A3610 + offset) != 0) {
    publishResult:
        {
            register int *dst = D_800A3500.words;
            register int first asm("$2");
            register int second asm("$3");
            register int third asm("$4");
            asm("" : "=r"(dst) : "0"(dst) : "memory");
            first = *(int *)((char *)D_800A3610 + offset);
            second = *(int *)((char *)D_800A3614 + offset);
            third = *(int *)((char *)D_800A3618 + offset);
            asm(""
                : "=r"(first), "=r"(second), "=r"(third)
                : "0"(first), "1"(second), "2"(third));
            dst[0] = first;
            dst[1] = second;
            dst[2] = third;
            dst[3] = *(int *)((char *)D_800A361C + offset);
        }
        asm("" ::: "$16", "memory");
        selected = &D_800A3500;
    }
copyResult:
    if (selected != 0) {
        Util_Copy8(result, selected->slot.payload);
        status = (s32)selected->slot.command;
        goto done;
    }
    status = 6;
done:
    return status;
}
