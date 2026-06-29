void DMACallback(int arg0, int arg1);

void CdDataCallback(int arg) {
    DMACallback(3, arg);
}
