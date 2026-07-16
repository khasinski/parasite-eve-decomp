extern short D_8009489C[];
extern short D_8009589C[];

int sin_1(int angle);

int rsin(int angle) {
    if (angle < 0) {
        return -sin_1((-angle) & 0xFFF);
    }

    return sin_1(angle & 0xFFF);
}

int sin_1(int angle) {
    int value;

    if (angle < 0x801) {
        if (angle < 0x401) {
            return D_8009589C[angle];
        }

        return D_8009589C[0x800 - angle];
    }

    if (angle < 0xC01) {
        value = D_8009489C[angle];
        return -value;
    }

    value = D_8009589C[0x1000 - angle];
    return -value;
}
