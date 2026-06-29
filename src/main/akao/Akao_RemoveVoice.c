void Akao_RemoveVoice(int *arg0, int arg1) {
    int i = 0;
    int value = 0x18;

    arg0 += 0x3C;
    do {
        if (arg1 == *arg0) {
            *arg0 = value;
        }
        i++;
        arg0 += 0x47;
    } while ((unsigned int)i < 0x18);
}
