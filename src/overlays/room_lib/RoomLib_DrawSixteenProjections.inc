#ifndef ROOMLIB_DRAW_SIXTEEN_PROJECTIONS_FUNC
#error "ROOMLIB_DRAW_SIXTEEN_PROJECTIONS_FUNC must name the room entry point"
#endif

typedef struct RoomSixteenProjectionPoint {
    s16 x;
    s16 y;
    s16 z;
    s16 pad;
} RoomSixteenProjectionPoint;

typedef struct RoomSixteenProjectionPacket {
    u32 tag;
    u8 code;
    u8 mode;
    u8 pad06[4];
    s16 depth;
    u8 pad0C[4];
} RoomSixteenProjectionPacket;

char *func_800C2B50(void);
void func_800C2EAC(u8 owner);
void func_800C2FF0(int width, int height);
void func_800C3098(int depth);
void func_800C3238(int page);
void Gte_ScaleMatrix(RoomSpriteMatrix *matrix, RoomFxVec4 *scale);
void func_800C42A4(void *packet, RoomSpriteMatrix *matrix, int mode);

extern RoomFxVec4 D_8018F00C;
extern RoomFxVec4 D_8018F01C;
extern u8 ROOMLIB_SIXTEEN_PACKET_CODE;
extern s16 ROOMLIB_SIXTEEN_PACKET_DEPTH;
extern s16 ROOMLIB_SIXTEEN_FRAME;

void ROOMLIB_DRAW_SIXTEEN_PROJECTIONS_FUNC(void *arg0, void *arg1,
                                            char *state) {
    RoomSpriteMatrix matrix;
    RoomFxVec4 scale1;
    RoomFxVec4 scale2;
    register char *rawOwner asm("$2");
    register char *owner asm("$17");
    register unsigned int i asm("$21");
    register RoomSixteenProjectionPoint *point asm("$18");
    register s16 *packetDepth asm("$20");
    register RoomSixteenProjectionPacket *packet asm("$22");
    register char *workState asm("$19");
    register char *packetTemp asm("$3");
    register int ownerZ asm("$3");
    u8 code;

    asm("" : "=r"(workState) : "0"(state));
    rawOwner = func_800C2B50();
    i = 0;
    PE1_COMPILER_USE(i);
    packetTemp = (char *)&ROOMLIB_SIXTEEN_PACKET_DEPTH;
    PE1_COMPILER_USE(packetTemp);
    packetDepth = (s16 *)packetTemp;
    PE1_COMPILER_USE(packetDepth);
    packetTemp = (char *)packetDepth - 10;
    PE1_COMPILER_USE(packetTemp);
    packet = (RoomSixteenProjectionPacket *)packetTemp;
    PE1_COMPILER_USE(packet);
    owner = rawOwner;
    PE1_COMPILER_USE(owner);
    point = (RoomSixteenProjectionPoint *)workState;
    func_800C2EAC(owner[0x24]);
    PE1_COMPILER_USE(point);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);

    matrix.m[2][2] = 0x1000;
    matrix.m[1][1] = 0x1000;
    matrix.m[0][0] = 0x1000;
    matrix.t[2] = 0;
    matrix.t[1] = 0;
    matrix.t[0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][0] = 0;
    matrix.m[1][2] = 0;
    matrix.m[1][0] = 0;
    matrix.m[0][2] = 0;
    matrix.m[0][1] = 0;
    scale1 = D_8018F00C;
    Gte_ScaleMatrix(&matrix, &scale1);
    matrix.t[0] = *(int *)(owner + 0x18);
    matrix.t[1] = *(int *)(owner + 0x1C);
    ownerZ = *(int *)(owner + 0x20);
    packet->depth = 0x40;
    matrix.t[2] = ownerZ;
    ROOMLIB_SIXTEEN_PACKET_CODE =
        *(u8 *)(workState + 0x104) * 2 + 0x20;
    func_800C42A4(packet, &matrix, 1);

    matrix.m[2][2] = 0x1000;
    matrix.m[1][1] = 0x1000;
    matrix.m[0][0] = 0x1000;
    matrix.t[2] = 0;
    matrix.t[1] = 0;
    matrix.t[0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][0] = 0;
    matrix.m[1][2] = 0;
    matrix.m[1][0] = 0;
    matrix.m[0][2] = 0;
    matrix.m[0][1] = 0;
    scale2 = D_8018F01C;
    Gte_ScaleMatrix(&matrix, &scale2);

    do {
        matrix.t[0] = point->x;
        matrix.t[1] = point->y;
        matrix.t[2] = point->z;
        *packetDepth = *(u16 *)(workState + 0x102);
        code = *(u8 *)(workState + 0x104);
        asm("" : "=r"(i) : "0"(i), "r"(code));
        i++;
        *((u8 *)packetDepth - 6) = code * 2 + 0x20;
        func_800C42A4(packet, &matrix, 1);
        matrix.t[0] = point->x;
        matrix.t[1] = ROOMLIB_SIXTEEN_FRAME;
        matrix.t[2] = point->z;
        *packetDepth = (s16)*(u16 *)(workState + 0x102) >> 2;
        code = *(u8 *)(workState + 0x104);
        point++;
        *((u8 *)packetDepth - 6) = code * 2 + 0x20;
        func_800C42A4(packet, &matrix, 1);
    } while (i < 16);
}

#undef ROOMLIB_DRAW_SIXTEEN_PROJECTIONS_FUNC
#undef ROOMLIB_SIXTEEN_PACKET_CODE
#undef ROOMLIB_SIXTEEN_PACKET_DEPTH
#undef ROOMLIB_SIXTEEN_FRAME
