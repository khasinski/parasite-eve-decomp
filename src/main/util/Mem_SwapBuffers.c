/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef unsigned char u8;
typedef unsigned int u32;

void Mem_SwapBuffers(u8 *arg0, u8 *arg1, u32 len) {
    u8 *left = arg0;
    register u8 *right asm("$9") = arg1;
    u32 i;

    asm volatile("" : "=r"(left) : "0"(left));
    i = 0;
    if (len != 0) {
        do {
            u8 *left_ptr;
            u8 *right_ptr;
            u8 left_value;
            u8 right_value;

            left_ptr = left + i;
            right_ptr = right + i;
            left_value = *left_ptr;
            right_value = *right_ptr;
            i++;
            *left_ptr = right_value;
            *right_ptr = left_value;
        } while (i < len);
    }
}
