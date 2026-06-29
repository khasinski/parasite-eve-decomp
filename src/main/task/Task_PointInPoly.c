/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int Geo_PointInPoly(int arg0, int arg1, int *arg2, int arg3);

int Task_PointInPoly(int **arg0) {
    int local[8];
    unsigned int i;

    for (i = 0; i < 4; i++) {
        local[i * 2] = *arg0[i * 2];
        local[i * 2 + 1] = *arg0[i * 2 + 1];
    }

    *arg0[10] = Geo_PointInPoly(*arg0[8], *arg0[9], local, 4);
    return 1;
}
