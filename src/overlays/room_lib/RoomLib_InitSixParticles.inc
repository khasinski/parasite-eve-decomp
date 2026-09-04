#ifndef ROOMLIB_INIT_SIX_PARTICLES_FUNC
#error "ROOMLIB_INIT_SIX_PARTICLES_FUNC must name the room entry point"
#endif

char *func_800C2B50(void);
void *func_800C2B28(int index);
int rand(void);
int rsin(int angle);
int rcos(int angle);

void ROOMLIB_INIT_SIX_PARTICLES_FUNC(void *arg0, void *arg1, char *state) {
    char *owner;
    unsigned int i;
    short *flag;
    int *life;
    char *particle;
    int angle;
    int wave;

    owner = func_800C2B50();
    func_800C2B28(0);
    angle = rand() % 0x1000;
    i = 0;
    flag = (short *)state;
    life = (int *)state;
    particle = state;
    do {
        PE1_COMPILER_LAUNDER(particle);
        *(short *)(particle + 0x0) = *(int *)(owner + 0x18);
        *(short *)(particle + 0x2) = *(int *)(owner + 0x1C);
        *(short *)(particle + 0x4) = *(int *)(owner + 0x20);

        wave = rsin(angle);
        *(short *)(particle + 0x30) =
            (wave * (rand() % 0x800 + 0x800)) >> 12;
        *(short *)(particle + 0x32) = -(rand() % 6096 + 3000);
        wave = rcos(angle);
        *(short *)(particle + 0x34) =
            (wave * (rand() % 0x800 + 0x800)) >> 12;

        i++;
        *(int *)((char *)life + 0x60) = 500;
        *(short *)((char *)flag + 0x78) = 1;
        flag++;
        life++;
        particle += 8;
    } while (i < 6);

    *(short *)(state + 0x86) = 0x80;
    *(short *)(state + 0x84) = 0x400;
    *(short *)(state + 0x88) = 0x320;
}

#undef ROOMLIB_INIT_SIX_PARTICLES_FUNC
