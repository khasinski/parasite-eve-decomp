#include "fx_common_setup.h"

#define SET_RGB(color, r, g, b) \
    (color)[0] = (r);           \
    (color)[1] = (g);           \
    (color)[2] = (b)

/* Build both frame buffers' full-screen gradient quads (sky, shade, the
 * semi-transparent bottom and top fades) and their three draw modes. */
void func_80195F6C(void)
{
    u32 i;

    for (i = 0; i < 2; i++) {
        func_80077BC4(&g_FxCommonSkyQuads[i]);
        SET_RGB(g_FxCommonSkyQuads[i].color0, 20, 68, 100);
        SET_RGB(g_FxCommonSkyQuads[i].color1, 20, 68, 100);
        SET_RGB(g_FxCommonSkyQuads[i].color2, 5, 17, 25);
        SET_RGB(g_FxCommonSkyQuads[i].color3, 5, 17, 25);
        g_FxCommonSkyQuads[i].x0 = 0;
        g_FxCommonSkyQuads[i].y0 = 0;
        g_FxCommonSkyQuads[i].x1 = 320;
        g_FxCommonSkyQuads[i].y1 = 0;
        g_FxCommonSkyQuads[i].x2 = 0;
        g_FxCommonSkyQuads[i].y2 = 240;
        g_FxCommonSkyQuads[i].x3 = 320;
        g_FxCommonSkyQuads[i].y3 = 240;

        func_80077BC4(&g_FxCommonShadeQuads[i]);
        SET_RGB(g_FxCommonShadeQuads[i].color0, 10, 24, 40);
        SET_RGB(g_FxCommonShadeQuads[i].color1, 10, 24, 40);
        SET_RGB(g_FxCommonShadeQuads[i].color2, 5, 7, 15);
        SET_RGB(g_FxCommonShadeQuads[i].color3, 5, 7, 15);
        g_FxCommonShadeQuads[i].x0 = 0;
        g_FxCommonShadeQuads[i].y0 = 0;
        g_FxCommonShadeQuads[i].x1 = 320;
        g_FxCommonShadeQuads[i].y1 = 0;
        g_FxCommonShadeQuads[i].x2 = 0;
        g_FxCommonShadeQuads[i].y2 = 240;
        g_FxCommonShadeQuads[i].x3 = 320;
        g_FxCommonShadeQuads[i].y3 = 240;

        func_80077BC4(&g_FxCommonFloorFadeQuads[i]);
        func_80077B04(&g_FxCommonFloorFadeQuads[i], 1);
        SET_RGB(g_FxCommonFloorFadeQuads[i].color0, 0, 0, 0);
        SET_RGB(g_FxCommonFloorFadeQuads[i].color1, 0, 0, 0);
        SET_RGB(g_FxCommonFloorFadeQuads[i].color2, 255, 255, 255);
        SET_RGB(g_FxCommonFloorFadeQuads[i].color3, 255, 255, 255);
        g_FxCommonFloorFadeQuads[i].x0 = 0;
        g_FxCommonFloorFadeQuads[i].y0 = 200;
        g_FxCommonFloorFadeQuads[i].x1 = 320;
        g_FxCommonFloorFadeQuads[i].y1 = 200;
        g_FxCommonFloorFadeQuads[i].x2 = 0;
        g_FxCommonFloorFadeQuads[i].y2 = 240;
        g_FxCommonFloorFadeQuads[i].x3 = 320;
        g_FxCommonFloorFadeQuads[i].y3 = 240;

        func_80077BC4(&g_FxCommonTopFadeQuads[i]);
        func_80077B04(&g_FxCommonTopFadeQuads[i], 1);
        SET_RGB(g_FxCommonTopFadeQuads[i].color0, 255, 255, 255);
        SET_RGB(g_FxCommonTopFadeQuads[i].color1, 255, 255, 255);
        SET_RGB(g_FxCommonTopFadeQuads[i].color2, 0, 0, 0);
        SET_RGB(g_FxCommonTopFadeQuads[i].color3, 0, 0, 0);
        g_FxCommonTopFadeQuads[i].x0 = 0;
        g_FxCommonTopFadeQuads[i].y0 = 0;
        g_FxCommonTopFadeQuads[i].x1 = 320;
        g_FxCommonTopFadeQuads[i].y1 = 0;
        g_FxCommonTopFadeQuads[i].x2 = 0;
        g_FxCommonTopFadeQuads[i].y2 = 40;
        g_FxCommonTopFadeQuads[i].x3 = 320;
        g_FxCommonTopFadeQuads[i].y3 = 40;

        func_80077C84(&g_FxCommonDrawModes0[i], 0, 0, 0);
        func_80077C84(&g_FxCommonDrawModes1[i], 0, 0, 0x20);
        func_80077C84(&g_FxCommonDrawModes2[i], 0, 0, 0x40);
    }
}
