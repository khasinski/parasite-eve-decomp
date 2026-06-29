int StartPAD2(void);
void ChangeClearPAD(int value);
void EnablePAD(void);

int Pad_Start(void) {
    StartPAD2();
    ChangeClearPAD(0);
    EnablePAD();
    return 1;
}
