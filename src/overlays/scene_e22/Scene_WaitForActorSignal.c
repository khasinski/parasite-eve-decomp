typedef struct { char reserved[0x14]; short complete, signaled; } WaitState;
typedef struct { char reserved[0x4C]; unsigned int status; } Owner;
typedef struct { Owner *owner; } Actor;
extern WaitState *D_800E2368;
extern Actor *RoomMain_ActorPtr;
extern int D_800E27EC;

int func_80198F54(int mode) {
    switch (mode) {
    case 0:
        D_800E2368->complete = 0;
        D_800E2368->signaled = 0;
        RoomMain_ActorPtr->owner->status &= ~0x10000000u;
        return 0;
    case 1:
        if (RoomMain_ActorPtr->owner->status & 0x10000000u) {
            D_800E2368->complete = 1;
            D_800E2368->signaled = 1;
            return 1;
        }
        if (D_800E27EC >= 100) {
            D_800E2368->complete = 1;
            D_800E2368->signaled = 0;
            return 1;
        }
        break;
    case 2:
        break;
    }
    return 0;
}
