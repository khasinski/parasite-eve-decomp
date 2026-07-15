int Save_ClassifyPlaytime(int playTime) {
    int result;

    if (playTime >= 0x80) {
        if (playTime >= 0x138) {
            if (playTime >= 0x1B0) {
                if (playTime >= 0x218) {
                    if (playTime < 0x2D3) {
                        result = 5;
                    } else {
                        result = 6;
                    }
                } else {
                    result = 4;
                }
            } else {
                result = 3;
            }
        } else {
            result = 2;
        }
    } else {
        result = 1;
    }

    return result;
}
