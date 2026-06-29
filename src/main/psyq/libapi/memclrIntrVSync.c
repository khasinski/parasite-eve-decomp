void memclrIntrVSync(int *ptr, int count) {
    int i = count - 1;

    if (count != 0) {
        do {
            *ptr = 0;
            i--;
            ptr++;
        } while (i != -1);
    }
}
