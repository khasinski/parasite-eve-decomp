void Render_SetFadeColour(unsigned int arg0);

int Task_SetFogColor(unsigned short **arg0) {
    Render_SetFadeColour(**arg0);
    return 1;
}
