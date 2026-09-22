/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --expand-div */

#include "pe1/render_object.h"
#include "pe1/render_lighting.h"
#include "pe1/render_prim.h"
u32 D_8009CDA0 = 0x808080;
int Render_TickObject(RenderObjectEntity *object) {
    int state;
    u8 r, g, b;
    if (!object->header || !object->draw_count) return 1;
    state = object->fade_remaining;
    if (!state) {
        object->fade_remaining = -1;
        Field_GetMapEntry(object, 0);
        Render_InitRoomPrimState(object);
        Render_SetEntityBlendMode(object, 0);
        object->flags_9C |= 0x820;
        return 1;
    }
    if (state < 0) {
        object->variant_visible = 1;
        object->flags_9C &= ~0x200;
        Field_GetMapEntry(object, 1);
        Render_SetEntityBlendMode(object, 1);
        r = object->primitive_red;
        g = object->primitive_green;
        b = object->primitive_blue;
        object->fade_red = 0;
        object->fade_green = 0;
        object->fade_blue = 0;
        Render_FadeEntityColor(object, 0, 0, 0);
        object->primitive_red = r;
        object->primitive_green = g;
        object->primitive_blue = b;
        object->fade_remaining = object->fade_duration;
    } else if (state == object->fade_duration - 1) {
        Field_GetMapEntry(object, 1);
        Render_SetEntityBlendMode(object, 1);
    }
    D_8009CDA0 = (object->fade_blue << 16) | (object->fade_green << 8) | object->fade_red;
    Render_DrawObject(object, &D_800BEA40);
    if (!(object->flags_9C & 8)) Render_UpdateClutTable(object, 0, (s16)D_8009CDDC);
    D_8009CDA0 = 0x808080;
    object->fade_red += object->fade_red_step;
    object->fade_green += object->fade_green_step;
    object->fade_blue += object->fade_blue_step;
    object->fade_remaining--;
    return 0;
}

int Render_ColorEntity(RenderObjectEntity *object) {
    int state;
    if (!object->header || !object->draw_count) return 1;
    state = object->fade_remaining;
    if (!state) {
        object->fade_remaining = -1;
        D_8009CDA0 = 0x808080;
        return 1;
    }
    if (state == 1) {
        object->variant_visible = 0;
    } else if (state < 0) {
        object->fade_remaining = object->fade_duration;
        Render_SetEntityBlendMode(object, 1);
        Field_GetMapEntry(object, 1);
        if (object->primitive_red > 128) object->primitive_red = 128;
        if (object->primitive_green > 128) object->primitive_green = 128;
        if (object->primitive_blue > 128) object->primitive_blue = 128;
        object->fade_red_step = object->primitive_red / object->fade_duration;
        object->fade_green_step = object->primitive_green / object->fade_duration;
        object->fade_blue_step = object->primitive_blue / object->fade_duration;
        object->fade_red = object->primitive_red;
        object->fade_green = object->primitive_green;
        object->fade_blue = object->primitive_blue;
    } else if (state == object->fade_duration - 1) {
        Render_SetEntityBlendMode(object, 1);
        Field_GetMapEntry(object, 1);
    } else {
        object->fade_red -= object->fade_red_step;
        if (object->fade_red > 128) object->fade_red = 0;
        object->fade_green -= object->fade_green_step;
        if (object->fade_green > 128) object->fade_green = 0;
        object->fade_blue -= object->fade_blue_step;
        if (object->fade_blue > 128) object->fade_blue = 0;
    }
    D_8009CDA0 = (object->fade_blue << 16) | (object->fade_green << 8) | object->fade_red;
    Render_DrawObject(object, &D_800BEA40);
    if (!(object->flags_9C & 8)) Render_UpdateClutTable(object, 0, (s16)D_8009CDDC);
    D_8009CDA0 = 0x808080;
    object->fade_remaining--;
    return 0;
}
