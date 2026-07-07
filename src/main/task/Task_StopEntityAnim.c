/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 */

typedef signed short s16;

void Render_SetupColorTable(s16 arg0, int arg1, s16 *arg2);

int Task_StopEntityAnim(s16 **arg0) {
    s16 value[5];

    value[0] = -1;
    Render_SetupColorTable(*arg0[0], 0, value);
    return 1;
}
