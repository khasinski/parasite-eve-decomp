int Render_SetEntryPosition(int index, int x, int y);

int Task_SetActorScreenPos(int **arg0) {
    Render_SetEntryPosition(*arg0[0], *(short *)arg0[1], *(short *)arg0[2]);
    return 1;
}
