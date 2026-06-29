int strncmp(char *s1, char *s2, int n);

int _cmp(char *s1, char *s2) {
    return strncmp(s1, s2, 0xC) == 0;
}
