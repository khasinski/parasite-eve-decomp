extern char *func_800C2B50(void);
extern int func_80071A54(void);
extern int func_80077CF4(int angle);
extern int func_80077DC4(int angle);
extern volatile u16 g_FrameCount16 __asm__("D_800942EC");

void func_80190254(void *unused, void *unused2, RoomOrbitParticleState *state) {
    char *clock = func_800C2B50();
    unsigned int i;
    RoomOrbitParticleLaneView *particle;

    i = 0;
    particle = (RoomOrbitParticleLaneView *)state;
    while (i < 8) {
        int angle;

        *(volatile u16 *)&particle->position.x = *(s32 *)(clock + 0x18);
        *(volatile u16 *)&particle->position.y = g_FrameCount16;
        particle->position.z = *(s32 *)(clock + 0x20);

        angle = func_80071A54() % 0x1000;
        particle->velocity.x = func_80077CF4(angle);
        angle = func_80071A54() % 0x1000;
        particle->velocity.z = func_80077DC4(angle);
        particle->velocity.angle = -(func_80071A54() % 8000 + 4000);
        i++;
        particle = (RoomOrbitParticleLaneView *)
            ((u8 *)particle + sizeof(RoomOrbitParticlePosition));
    }

    state->decay = 0x80;
    state->height = 0x400;
    state->radiusStep = -30;
    state->intensity = 0;
    state->radius = 0x258;
}
