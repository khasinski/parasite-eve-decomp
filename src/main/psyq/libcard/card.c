/* GCC_VERSION: 2.8.1 */

void _new_card(int arg0);
void _card_write(int arg0, int arg1, int arg2);

void _card_clear(int arg0) {
    _new_card(arg0);
    _card_write(arg0, 0x3F, 0);
}
