/* PSY-Q LIBGTE REG13: SetGeomScreen. psyq_provenance.json has no signature
 * for this range; the object name follows the REG10..REG12 neighbours. */
void SetGeomScreen(int h) {
    asm volatile("ctc2 %0,$26" : : "r"(h));
}
