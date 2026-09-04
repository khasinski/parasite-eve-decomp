#ifndef ROOMLIB_DRAW_SIX_PARTICLES_FUNC
#error "ROOMLIB_DRAW_SIX_PARTICLES_FUNC must name the room entry point"
#endif

typedef struct RoomSixParticlePacket {
    u32 tag;
    u8 code;
    u8 mode;
    u8 pad06[4];
    s16 depth;
    u8 pad0C[4];
} RoomSixParticlePacket;

char *func_800C2B50(void);
void func_800C2EAC(u8 owner);
void func_800C2FF0(int width, int height);
void func_800C3098(int depth);
void func_800C3238(int page);
void Gte_ScaleMatrix(RoomSpriteMatrix *matrix, RoomFxVec4 *scale);
void func_800C42A4(void *packet, RoomSpriteMatrix *matrix, int mode);

extern RoomFxVec4 D_8018F034;
extern u8 ROOMLIB_SIX_PARTICLE_PACKET_CODE;
extern u8 ROOMLIB_SIX_PARTICLE_PACKET_MODE;
extern s16 ROOMLIB_SIX_PARTICLE_PACKET_DEPTH;
extern s16 ROOMLIB_SIX_PARTICLE_FRAME;

void ROOMLIB_DRAW_SIX_PARTICLES_FUNC(void *arg0, void *arg1, char *state) {
    RoomSpriteMatrix matrix;
    RoomFxVec4 scale;
    char *owner;
    int depth;
    register unsigned int i asm("$16");
    char *particle;
    u8 *packetFields;
    RoomSixParticlePacket *packet;
    u8 code;
    u8 mode;

    owner = func_800C2B50();
    PE1_COMPILER_USE(owner);
    depth = 0x80;
    i = 0;
    code = 0x42;
    mode = 3;
    packetFields = &ROOMLIB_SIX_PARTICLE_PACKET_CODE;
    packet = (RoomSixParticlePacket *)(packetFields - 4);
    func_800C2EAC(owner[0x24]);
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
    scale = D_8018F034;
    Gte_ScaleMatrix(&matrix, &scale);

    particle = state;
    do {
        PE1_COMPILER_LAUNDER(particle);
        depth -= 20;
        if (*(short *)(state + 0x78 + i * 2) == 1) {
            matrix.t[0] = *(short *)(particle + 0);
            matrix.t[1] = *(short *)(particle + 2);
            matrix.t[2] = *(short *)(particle + 4);
            if (i == 0) {
                func_800C3238(2);
                packetFields[0] = code;
                packetFields[1] = mode;
                *(s16 *)(packetFields + 6) = 0xFF;
            } else {
                func_800C3238(3);
                packetFields[0] = 0x47;
                packetFields[1] = 5;
                *(s16 *)(packetFields + 6) = depth;
            }
            func_800C42A4(packet, &matrix, 1);
            if (i == 0) {
                matrix.t[0] = *(short *)(state + 0);
                matrix.t[1] = ROOMLIB_SIX_PARTICLE_FRAME;
                matrix.t[2] = *(short *)(state + 4);
                ROOMLIB_SIX_PARTICLE_PACKET_DEPTH = 0x40;
                ROOMLIB_SIX_PARTICLE_PACKET_CODE = code;
                ROOMLIB_SIX_PARTICLE_PACKET_MODE = mode;
                func_800C3238(2);
                func_800C42A4(packet, &matrix, 1);
            }
        }
        i++;
        particle += 8;
    } while (i < 6);
}

#undef ROOMLIB_DRAW_SIX_PARTICLES_FUNC
#undef ROOMLIB_SIX_PARTICLE_PACKET_CODE
#undef ROOMLIB_SIX_PARTICLE_PACKET_MODE
#undef ROOMLIB_SIX_PARTICLE_PACKET_DEPTH
#undef ROOMLIB_SIX_PARTICLE_FRAME
