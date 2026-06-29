void Render_SetEntryPosition(int arg0, int arg1, int arg2);

int Task_SetActorScreenPos(int **arg0) {
    Render_SetEntryPosition(*arg0[0], *(short *)arg0[1], *(short *)arg0[2]);
    return 1;
}
