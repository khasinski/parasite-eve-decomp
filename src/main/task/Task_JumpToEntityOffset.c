/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef struct {
    int value;
} TaskJumpOffsetEntry;

typedef struct {
    char pad0[0x9C];
    char *base;
} TaskJumpOffsetEntity;

extern TaskJumpOffsetEntity *D_8009D2F0[];
extern char *D_8009CE00;

int Task_JumpToEntityOffset(TaskJumpOffsetEntry **arg0)
{
    D_8009CE00 = D_8009D2F0[0]->base + ((*arg0)->value * 2);
    return 1;
}
