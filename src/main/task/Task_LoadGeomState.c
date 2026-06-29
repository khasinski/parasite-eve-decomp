void Gpu_LoadGeomState(int arg0);

int Task_LoadGeomState(int **arg0) {
    Gpu_LoadGeomState(**arg0);
    return 1;
}
