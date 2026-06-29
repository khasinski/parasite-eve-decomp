void Spu_ManageVoices(int arg0, int arg1);

void Spu_ManageVoicesCmd(int *arg0) {
    Spu_ManageVoices(arg0[1], arg0[2]);
}
