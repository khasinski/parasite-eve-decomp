#ifndef PE1_MENU_STATE_H
#define PE1_MENU_STATE_H

/* Menu open/close state (the in-field item/status/equip menu drawn over the
 * paused field). Confirmed live via DuckStation GDB: snapshot-diffed the game
 * state block 0x8009C000..0x8009E000 across field <-> menu transitions; the
 * toggled words reverted cleanly on open then close.
 *
 * The menu is a LAYER over the field: opening it does NOT change the top game
 * mode; it (1) sets a modal flag, (2) allocates a MenuWidget node pool, and
 * (3) bumps a draw phase. The field data stays resident; field updates gate on
 * the modal flag.
 *
 * Confirmed toggling globals (field value -> menu value):
 *   D_8009D1A0  global game-state flags
 *               bit 0x04 SET while a modal menu is open (clear in plain field).
 *               Also read by battle (Battle_IsActive / Battle_GetStateFlag1);
 *               Boot_RunFrame masks bits 0x30. This is the central mode bitfield.
 *   D_8009D154  MenuWidget node-pool pointer : NULL in field -> 0x800a25b0 in menu
 *   D_8009D15C  MenuWidget node-pool pointer : NULL in field -> 0x800a2370 in menu
 *   D_8009D158  active widget/draw pointer    : 0x800a22e0  -> 0x800a2640
 *   D_8009D0F4  menu draw phase / step        : field 0x01  -> menu 0x08
 *   D_8009D100  active draw buffer pointer     : switches with the layer
 *   (The D_8009D150..D_8009D160 cluster is the MenuWidget pool head/free/tail,
 *    managed by MenuWidget_InitPool / MenuWidget_CreateNode; non-null only
 *    while a menu owns it -> a clean "is a menu open?" test.)
 *
 * OPEN  : MenuWidget_InitPool() allocates the node pool (D_8009D154/15C != NULL),
 *         D_8009D1A0 |= 0x04, D_8009D0F4 set; the inventory root step begins
 *         (Menu_StepInventoryRoot via Menu_InitCategoryTabs).
 * CLOSE : the pool is torn down (pointers NULL), D_8009D1A0 &= ~0x04, phase
 *         restored; control returns to the field step.
 *
 * Per-frame: Boot_RunFrame() -> Field_HandleStateTransition() (field/state
 * machine, D_8009D1A0) then Entity_FrameUpdate(); the menu widget tree is
 * driven by MenuWidget_UpdateAndDraw() while the pool is live.
 *
 * Quick live probe: a menu is open iff *(u32*)0x8009D154 != 0  (or D_8009D1A0 & 4).
 */

#endif /* PE1_MENU_STATE_H */
