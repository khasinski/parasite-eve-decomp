void MenuWidget_NavScrollTo(int arg0);

void Menu_CloseNotificationDialogs(void) {
    MenuWidget_NavScrollTo(0x28);
    MenuWidget_NavScrollTo(0x3D);
}
