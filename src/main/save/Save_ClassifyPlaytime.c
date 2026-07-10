int Save_ClassifyPlaytime(int playTime) {
    if (playTime < 0x80) {
        return 1;
    }
    if (playTime < 0x138) {
        return 2;
    }
    if (playTime < 0x1B0) {
        return 3;
    }
    if (playTime < 0x218) {
        return 4;
    }
    if (playTime < 0x2D3) {
        return 5;
    }
    return 6;
}
