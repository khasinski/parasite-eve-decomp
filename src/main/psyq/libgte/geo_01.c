extern char D_8009409C[];
extern char D_8009509C[];
extern char D_8009589C[];

int rcos(int angle) {
    if (angle < 0) {
        angle = -angle;
    }

    angle &= 0xFFF;
    if (angle < 0x801) {
        if (angle < 0x401) {
            return *(short *)(D_8009589C + ((0x400 - angle) << 1));
        }
        return -*(short *)(D_8009509C + (angle << 1));
    }

    if (angle < 0xC01) {
        return -*(short *)(D_8009589C + ((0xC00 - angle) << 1));
    }

    return *(short *)(D_8009409C + (angle << 1));
}
