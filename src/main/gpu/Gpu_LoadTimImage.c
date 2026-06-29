/* CC1_PROFILE: build0001 */

int LoadImage(int rect, int p);

int Gpu_LoadTimImage(int arg0) {
    int secondary;
    int image;
    int pixels;

    secondary = 0;
    if ((*(int *)(arg0 + 4) & 8) != 0) {
        secondary = arg0 + 8;
        image = secondary + *(int *)(arg0 + 8);
    } else {
        image = arg0 + 8;
    }

    pixels = image + 0xC;
    LoadImage(image + 4, pixels);
    if (secondary != 0) {
        LoadImage(secondary + 4, secondary + 0xC);
    }
    return pixels;
}
