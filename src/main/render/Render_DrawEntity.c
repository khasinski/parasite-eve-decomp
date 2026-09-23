#include "pe1/render_object.h"
#include "pe1/render_prim.h"
#include "pe1/render_lighting.h"
void Render_DrawObjectVariant(RenderObjectEntity *, s16, void *);
void Render_DrawTexturedQuads(RenderObjectEntity *);
void Render_BuildEntityMatrix(RenderObjectEntity *, s16);
void Render_DrawObjectAlt(RenderObjectEntity *, s16, u8, u8, u8);

void Render_DrawEntity(RenderObjectEntity *object, void *viewMatrix) {
    u16 flags;
    if (object->header == 0 || object->draw_count == 0) return;
    if (object->flags_9C & 0x800) {
        Field_GetMapEntry(object, 0);
        Render_SetEntityBlendMode(object, 0);
        object->flags_9C &= ~0x800;
    }
    if (object->flags_9C & 0x10)
        Render_DrawObjectVariant(object, object->script_value9a, viewMatrix);
    if (object->variant_visible == 1)
        Render_DrawTexturedQuads(object);
    flags = object->flags_9C;
    if (flags & 0x20) {
        Render_BuildEntityMatrix(object, (s16)D_8009CDDC);
        object->reserved9f = (u32)D_8009CDDC < 1;
        object->flags_9C = (object->flags_9C & ~0x20) | 0x40;
    } else if (flags & 0x40) {
        Render_BuildEntityMatrix(object, object->reserved9f);
        object->flags_9C &= ~0x40;
    }
    flags = object->flags_9C;
    if (flags & 2) {
        if ((s16)Render_ColorEntity(object))
            object->flags_9C = (object->flags_9C & ~2) | 0x200;
        if (object->flags_9C & 8)
            Render_DrawObjectAlt(object, object->script_value9a, object->script_param97,
                                 object->script_param98, object->script_param99);
    } else if (flags & 4) {
        if ((s16)Render_TickObject(object))
            object->flags_9C &= ~4;
        if (object->flags_9C & 8)
            Render_DrawObjectAlt(object, object->script_value9a, object->script_param97,
                                 object->script_param98, object->script_param99);
    } else if (flags & 8) {
        Render_DrawObject(object, &D_800BEA40);
        Render_DrawObjectAlt(object, object->script_value9a, object->script_param97,
                             object->script_param98, object->script_param99);
    } else if (flags & 1) {
        Render_DrawObject(object, &D_800BEA40);
        Render_UpdateClutTable(object, 0, (s16)D_8009CDDC);
    }
}
