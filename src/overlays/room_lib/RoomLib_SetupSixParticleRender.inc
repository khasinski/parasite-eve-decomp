#ifndef ROOMLIB_SETUP_SIX_PARTICLE_RENDER_FUNC
#error "ROOMLIB_SETUP_SIX_PARTICLE_RENDER_FUNC must name the room entry point"
#endif

void func_800C2B40(void *state);
int *func_800C2B28(int index);
void *Asset_SearchByKeyType(int key);
void func_800C66C8(void *owner, int id, void *state);

extern u8 ROOMLIB_PACKET_A_R, ROOMLIB_PACKET_A_G, ROOMLIB_PACKET_A_B;
extern u8 ROOMLIB_PACKET_A_CODE, ROOMLIB_PACKET_A_MODE, ROOMLIB_PACKET_A_ZERO;
extern s16 ROOMLIB_PACKET_A_OFFSET, ROOMLIB_PACKET_A_DEPTH;
extern u8 ROOMLIB_PACKET_B_R, ROOMLIB_PACKET_B_G, ROOMLIB_PACKET_B_B;
extern u8 ROOMLIB_PACKET_B_CODE, ROOMLIB_PACKET_B_MODE, ROOMLIB_PACKET_B_ZERO;
extern s16 ROOMLIB_PACKET_B_OFFSET, ROOMLIB_PACKET_B_DEPTH;
extern u8 ROOMLIB_PACKET_C_R, ROOMLIB_PACKET_C_G, ROOMLIB_PACKET_C_B;
extern u8 ROOMLIB_PACKET_C_CODE, ROOMLIB_PACKET_C_MODE, ROOMLIB_PACKET_C_ZERO;
extern s16 ROOMLIB_PACKET_C_OFFSET, ROOMLIB_PACKET_C_DEPTH;
extern u8 ROOMLIB_PACKET_D_R, ROOMLIB_PACKET_D_G, ROOMLIB_PACKET_D_B;
extern u8 ROOMLIB_PACKET_D_CODE, ROOMLIB_PACKET_D_MODE, ROOMLIB_PACKET_D_ZERO;
extern s16 ROOMLIB_PACKET_D_OFFSET, ROOMLIB_PACKET_D_DEPTH;
extern s16 ROOMLIB_SIX_PARTICLE_FRAME;

void ROOMLIB_SETUP_SIX_PARTICLE_RENDER_FUNC(char *owner, void *arg1,
                                             char *state) {
    char *entity;
    void *asset;
    register char *callOwner asm("$4");
    register int depth asm("$5");
    register int color asm("$3");
    register int offset asm("$6");
    register int modeB asm("$2");
    register char *workState asm("$16");

    asm("" : "=r"(workState) : "0"(state));
    func_800C2B40(workState);
    *(s16 *)(workState + 0x2A) = 0;
    *(s16 *)(workState + 0x2C) = 0;
    *(s16 *)(workState + 0x28) = *func_800C2B28(5);
    *(s16 *)(workState + 0x2E) = *func_800C2B28(4);
    entity = *(char **)(owner + 8);
    *(char **)workState = entity;
    *(RoomSpriteMatrix *)(workState + 4) =
        **(RoomSpriteMatrix **)(entity + 0x238);
    *(s32 *)(workState + 0x18) = *func_800C2B28(1);
    *(s32 *)(workState + 0x1C) = *func_800C2B28(2);
    *(s32 *)(workState + 0x20) = *func_800C2B28(3);
    asset = Asset_SearchByKeyType(0xA6);
    callOwner = owner;
    *(void **)(workState + 0x24) = asset;

    ROOMLIB_PACKET_A_CODE = 0x42;
    ROOMLIB_PACKET_A_MODE = 3;
    {
        register int minus50 asm("$2");
        asm volatile("" : "=r"(minus50), "=r"(depth), "=r"(color)
                     : "0"(-50), "1"(0x80), "2"(0x80));
        ROOMLIB_PACKET_A_OFFSET = minus50;
    }
    ROOMLIB_PACKET_D_CODE = 0x20;
    ROOMLIB_PACKET_D_MODE = 1;
    ROOMLIB_PACKET_D_OFFSET = -51;
    ROOMLIB_PACKET_C_CODE = 0x40;
    {
        register int two asm("$2");
        asm volatile("" : "=r"(two), "=r"(offset)
                     : "0"(2), "1"(-41));
        ROOMLIB_PACKET_C_MODE = two;
    }
    ROOMLIB_PACKET_B_CODE = 0x47;
    asm volatile("" : "=r"(modeB) : "0"(5));
    PE1_COMPILER_LAUNDER(workState);
    workState += 4;
    PE1_COMPILER_LAUNDER(workState);
    ROOMLIB_PACKET_A_DEPTH = depth;
    ROOMLIB_PACKET_D_DEPTH = depth;
    ROOMLIB_PACKET_C_DEPTH = depth;
    ROOMLIB_PACKET_B_DEPTH = depth;
    ROOMLIB_PACKET_C_OFFSET = offset;
    ROOMLIB_PACKET_B_OFFSET = offset;
    ROOMLIB_PACKET_A_R = color;
    ROOMLIB_PACKET_A_G = color;
    ROOMLIB_PACKET_A_B = color;
    ROOMLIB_PACKET_A_ZERO = 0;
    ROOMLIB_PACKET_D_R = color;
    ROOMLIB_PACKET_D_G = color;
    ROOMLIB_PACKET_D_B = color;
    ROOMLIB_PACKET_D_ZERO = 0;
    ROOMLIB_PACKET_C_R = color;
    ROOMLIB_PACKET_C_G = color;
    ROOMLIB_PACKET_C_B = color;
    ROOMLIB_PACKET_C_ZERO = 0;
    ROOMLIB_PACKET_B_MODE = modeB;
    ROOMLIB_PACKET_B_R = color;
    ROOMLIB_PACKET_B_G = color;
    ROOMLIB_PACKET_B_B = color;
    ROOMLIB_PACKET_B_ZERO = 0;
    ROOMLIB_SIX_PARTICLE_FRAME = 0;
    func_800C66C8(callOwner, 0x587, workState);
}

#undef ROOMLIB_SETUP_SIX_PARTICLE_RENDER_FUNC
#undef ROOMLIB_PACKET_A_R
#undef ROOMLIB_PACKET_A_G
#undef ROOMLIB_PACKET_A_B
#undef ROOMLIB_PACKET_A_CODE
#undef ROOMLIB_PACKET_A_MODE
#undef ROOMLIB_PACKET_A_ZERO
#undef ROOMLIB_PACKET_A_OFFSET
#undef ROOMLIB_PACKET_A_DEPTH
#undef ROOMLIB_PACKET_B_R
#undef ROOMLIB_PACKET_B_G
#undef ROOMLIB_PACKET_B_B
#undef ROOMLIB_PACKET_B_CODE
#undef ROOMLIB_PACKET_B_MODE
#undef ROOMLIB_PACKET_B_ZERO
#undef ROOMLIB_PACKET_B_OFFSET
#undef ROOMLIB_PACKET_B_DEPTH
#undef ROOMLIB_PACKET_C_R
#undef ROOMLIB_PACKET_C_G
#undef ROOMLIB_PACKET_C_B
#undef ROOMLIB_PACKET_C_CODE
#undef ROOMLIB_PACKET_C_MODE
#undef ROOMLIB_PACKET_C_ZERO
#undef ROOMLIB_PACKET_C_OFFSET
#undef ROOMLIB_PACKET_C_DEPTH
#undef ROOMLIB_PACKET_D_R
#undef ROOMLIB_PACKET_D_G
#undef ROOMLIB_PACKET_D_B
#undef ROOMLIB_PACKET_D_CODE
#undef ROOMLIB_PACKET_D_MODE
#undef ROOMLIB_PACKET_D_ZERO
#undef ROOMLIB_PACKET_D_OFFSET
#undef ROOMLIB_PACKET_D_DEPTH
#undef ROOMLIB_SIX_PARTICLE_FRAME
