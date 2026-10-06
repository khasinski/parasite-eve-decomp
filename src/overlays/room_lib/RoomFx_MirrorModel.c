/* MASPSX_FLAGS: --expand-div */
/*
 * The mirror room controller: seven script handlers (three of them empty)
 * that find an actor by kind, bind a mirror model to it across a guide
 * line and, every frame, reflect the actor's pose into the mirror model
 * and draw it.
 *
 * room_m017, room_m018, room_m021, room_m045, room_m102, room_m151 and
 * room_m319 are the same program: these ten functions in this order are
 * all of their code, and their data starts with the same handler table and
 * mirror axes. This unit is that object, compiled into each of them; the
 * room's model and images follow in its own data.
 */
#include "pe1/room_fx_model.h"
#include "pe1/gte.h"
#include "pe1/render_object.h"
#include "pe1/render_prim.h"
#include "pe1/render_tint.h"
#include "pe1/entity_frame_update.h"

void RoomFx_ModelDraw(RenderObjectEntity *entity);

void *g_RoomFxMirrorHandlers[7] = {
    RoomFx_MirrorNoOp0, RoomFx_MirrorReset, RoomFx_MirrorSelect, RoomFx_MirrorTick,
    RoomFx_MirrorNoOp4, RoomFx_MirrorKill, RoomFx_MirrorNoOp6,
};

/* Mirror across the YZ plane: x negated. */
GteMatrix g_RoomFxMirrorAxes = {
    { { -0x1000, 0, 0 }, { 0, 0x1000, 0 }, { 0, 0, 0x1000 } },
    { 0, 0, 0 },
};

int RoomFx_MirrorNoOp0(void) {
    return 0;
}

int RoomFx_MirrorReset(RoomFxMirror *mirror) {
    mirror->target = 0;
    mirror->lineZ0 = 0;
    mirror->lineX0 = 0;
    mirror->lineZ1 = 0;
    mirror->lineX1 = 0;
    mirror->bound = 0;
    mirror->active = 0;
    return 0;
}

/* Script entry: mode 0 finds the first idle actor of the given kind,
 * modes 1 and 2 set the guide line's ends, mode 3 starts or stops the
 * mirror. */
int RoomFx_MirrorSelect(RoomFxMirror *mirror, int cancel, u32 mode, int a, int b) {
    RoomFxModelOwner **target = &mirror->target;

    switch (mode) {
    case 0:
        if (cancel != 0) {
            break;
        }
        *target = (RoomFxModelOwner *)g_FieldActorListHead;
        while (*target != 0) {
            RoomFxModelOwner *node = *target;
            if (node->kind == a && node->subKind == b && !(node->status & 0x10)) {
                break;
            }
            *target = (*target)->next;
        }
        break;
    case 1:
        mirror->lineX0 = a;
        mirror->lineZ0 = b;
        break;
    case 2:
        mirror->lineX1 = a;
        mirror->lineZ1 = b;
        break;
    case 3:
        mirror->active = a;
        break;
    default:
        return -6;
    }
    return 0;
}

int RoomFx_MirrorTick(RoomFxMirror *mirror) {
    if (mirror->bound == 0) {
        RoomFx_ModelBind(&mirror->object->geom, mirror->target, mirror->lineX0,
                         mirror->lineZ0, mirror->lineX1, mirror->lineZ1);
        mirror->bound = 1;
    }
    if (mirror->active != 0) {
        RoomFx_ModelUpdate(&mirror->object->geom);
        Render_TransformVertices((RenderObjectEntity *)&mirror->object->geom);
        Render_TransformMorphVertices((RenderObjectEntity *)&mirror->object->geom,
                                      g_GeomVramPacketDst);
        RoomFx_ModelDraw((RenderObjectEntity *)&mirror->object->geom);
    }
    return 0;
}

int RoomFx_MirrorNoOp4(void) {
    return 0;
}

int RoomFx_MirrorKill(u8 *state) {
    *state = 4;
    return 0;
}

int RoomFx_MirrorNoOp6(void) {
    return 0;
}

/* An old-style definition, as retail: the callers pass the line ends as
 * ints, the binder reads them back as halfwords. */
void RoomFx_ModelBind(geom, owner, x0, z0, x1, z1)
    RoomFxModelGeom *geom;
    RoomFxModelOwner *owner;
    u16 x0;
    u16 z0;
    u16 x1;
    u16 z1;
{
    geom->mode = 5;
    geom->owner = owner;
    geom->line_x0 = x0;
    geom->line_z0 = z0;
    geom->flags |= 0x1000;
    geom->line_x1 = x1;
    geom->line_z1 = z1;
}

/* Mirror the owner's model across the guide line: reflect its position
 * through the line, flip its rotation and point the model along the line,
 * then rebuild the animation matrices. */
void RoomFx_ModelUpdate(RoomFxModelGeom *geom) {
    RoomFxModelOwner *owner = geom->owner;
    RoomFxModelGeom *parent = &owner->geom;
    s16 dx = -geom->line_x0 + geom->line_x1;
    s16 dz = -geom->line_z0 + geom->line_z1;
    int slope;
    int x, z;
    int distance;
    int angle;
    int sx, sz;
    int yaw;
    int nz, nx;
    int intercept;
    int adx;
    int rx;
    int rz;

    if (dx == 0) {
        dx = 1;
    }
    slope = dz / dx;
    intercept = (s16)geom->line_z1 - slope * (s16)geom->line_x1;
    x = owner->geom.matrix.t[0];
    z = owner->geom.matrix.t[2];
    distance = slope * x - z + intercept;
    if (distance < 0) {
        distance = -distance;
    }
    distance /= SquareRoot0(slope * slope + 1);
    angle = ratan2(dz, dx);
    sx = (rsin(angle) * distance) >> 12;
    sz = (rcos(angle) * distance) >> 12;
    nx = x - sx * 2;
    geom->matrix.t[0] = nx;
    geom->matrix.t[1] = owner->geom.matrix.t[1];
    nz = z + sz * 2;
    geom->matrix.t[2] = nz;
    adx = abs(dx);
    rx = -owner->geom.rotation.x;
    if (dz >= 0 ? dz < adx : -dz < adx) {
        if (z < nz) {
            nz = angle + 0x800;
            yaw = parent->rotation.y + nz;
        } else {
            yaw = parent->rotation.y + angle;
        }
    } else if (nx < x) {
        nz = angle + 0xC00;
        yaw = parent->rotation.y + nz;
    } else {
        nz = angle + 0x400;
        yaw = parent->rotation.y + nz;
    }
    nz = -yaw;
    rz = -parent->rotation.z;
    geom->rotation.x = rx;
    geom->rotation.y = nz;
    geom->rotation.z = rz;
    RotMatrix(&geom->rotation, &geom->matrix);
    gte_ldrotmatrix(&geom->matrix);
    gte_ldclmv(&g_RoomFxMirrorAxes.m[0][0]);
    gte_rtir();
    gte_stclmv(&geom->matrix.m[0][0]);
    gte_ldclmv(&g_RoomFxMirrorAxes.m[0][1]);
    gte_rtir();
    gte_stclmv(&geom->matrix.m[0][1]);
    gte_ldclmv(&g_RoomFxMirrorAxes.m[0][2]);
    gte_rtir();
    gte_stclmv(&geom->matrix.m[0][2]);
    func_80039B74(geom, owner->animation, owner->matrix_count, 1);
}

/* Projected screen XY and view depth of the transformed model vertices. */
extern u32 D_800B1644[];
extern s32 D_800A636C[];

/* PSY-Q libgpu style ordering-table link through the 24-bit tag address. */
#define ROOMFX_SETADDR(p, a) (((RenderGpuTag *)(p))->address = (u32)(a))
#define ROOMFX_GETADDR(p) (((RenderGpuTag *)(p))->address)
#define ROOMFX_ADDPRIM(ot, p) \
    ROOMFX_SETADDR(p, ROOMFX_GETADDR(ot)), ROOMFX_SETADDR(ot, p)

/* Room build of the main binary's Render_DrawTexturedQuads: NCLIP-tests each
 * face on the projected vertices and links the front-facing ones (negative
 * winding) into the active ordering table at their average depth; culled
 * faces get a null link. Quads and triangles of both packet classes are
 * walked in turn through one shared descriptor cursor. */
void RoomFx_ModelDraw(RenderObjectEntity *entity) {
    int i;
    s32 *area = (s32 *)0x1F800000;
    u32 *xy = D_800B1644;
    RenderPrimitiveDescriptor *record = entity->primitive_descriptors;
    u8 *packets = entity->primitive_buffer;
    int slot = D_8009CDDC;
    u32 *ordering = (u32 *)D_800B0E38.ordering[slot];
    s32 *z = D_800A636C;
    u32 xy0, xy1, xy2;
    int a, b, c, d;
    u8 *packet;

    i = 0;
    while (i < entity->header->packet34_count) {
        a = record->lookup_indices[0];
        b = record->lookup_indices[1];
        c = record->lookup_indices[2];
        xy0 = xy[a];
        xy1 = xy[b];
        xy2 = xy[c];
        gte_ldsxy0(xy0);
        gte_ldsxy2(xy2);
        gte_ldsxy1(xy1);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_nclip_now();
        packet = (u8 *)((RenderPacket34 *)packets + slot);
        d = record->lookup_indices[3];
        gte_stmac0(area);
        if (*area < 0) {
            int depth = z[a] + z[b];
            depth += z[c];
            depth += z[d];
            depth >>= 4;
            if (depth < 0x1000) {
                u32 *entry = (u32 *)(depth * 4 + (u32)ordering);
                ROOMFX_ADDPRIM(entry, packet);
                ((RenderPacket34 *)packet)->sxy0 = xy0;
                ((RenderPacket34 *)packet)->sxy1 = xy1;
                ((RenderPacket34 *)packet)->sxy2 = xy2;
                ((RenderPacket34 *)packet)->sxy3 = xy[d];
            }
        } else {
            ROOMFX_SETADDR(packet, 0);
        }
        packets += 2 * sizeof(RenderPacket34);
        i++;
        record++;
    }

    i = 0;
    while (i < entity->header->packet28_count) {
        a = record->lookup_indices[0];
        b = record->lookup_indices[1];
        c = record->lookup_indices[2];
        xy0 = xy[a];
        xy1 = xy[b];
        xy2 = xy[c];
        gte_ldsxy0(xy0);
        gte_ldsxy2(xy2);
        gte_ldsxy1(xy1);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_nclip_now();
        packet = (u8 *)((RenderPacket28 *)packets + slot);
        gte_stmac0(area);
        if (*area < 0) {
            int depth = z[a] + z[b] + z[c];
            depth = depth / 3 >> 2;
            if (depth < 0x1000) {
                u32 *entry = (u32 *)(depth * 4 + (u32)ordering);
                ROOMFX_ADDPRIM(entry, packet);
                ((RenderPacket28 *)packet)->sxy0 = xy0;
                ((RenderPacket28 *)packet)->sxy1 = xy1;
                ((RenderPacket28 *)packet)->sxy2 = xy2;
            }
        } else {
            ROOMFX_SETADDR(packet, 0);
        }
        packets += 2 * sizeof(RenderPacket28);
        i++;
        record++;
    }

    i = 0;
    while (i < entity->header->packet24_count) {
        a = record->lookup_indices[0];
        b = record->lookup_indices[1];
        c = record->lookup_indices[2];
        xy0 = xy[a];
        xy1 = xy[b];
        xy2 = xy[c];
        gte_ldsxy0(xy0);
        gte_ldsxy2(xy2);
        gte_ldsxy1(xy1);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_nclip_now();
        packet = (u8 *)((RenderPacket24 *)packets + slot);
        d = record->lookup_indices[3];
        gte_stmac0(area);
        if (*area < 0) {
            int depth = z[a] + z[b];
            depth += z[c];
            depth += z[d];
            depth >>= 4;
            if (depth < 0x1000) {
                u32 *entry = (u32 *)(depth * 4 + (u32)ordering);
                ROOMFX_ADDPRIM(entry, packet);
                ((RenderPacket24 *)packet)->sxy0 = xy0;
                ((RenderPacket24 *)packet)->sxy1 = xy1;
                ((RenderPacket24 *)packet)->sxy2 = xy2;
                ((RenderPacket24 *)packet)->sxy3 = xy[d];
            }
        } else {
            ROOMFX_SETADDR(packet, 0);
        }
        packets += 2 * sizeof(RenderPacket24);
        i++;
        record++;
    }

    i = 0;
    while (i < entity->header->packet1c_count) {
        a = record->lookup_indices[0];
        b = record->lookup_indices[1];
        c = record->lookup_indices[2];
        xy0 = xy[a];
        xy1 = xy[b];
        xy2 = xy[c];
        gte_ldsxy0(xy0);
        gte_ldsxy2(xy2);
        gte_ldsxy1(xy1);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_nclip_now();
        packet = (u8 *)((RenderPacket1C *)packets + slot);
        gte_stmac0(area);
        if (*area < 0) {
            int depth = z[a] + z[b] + z[c];
            depth = depth / 3 >> 2;
            if (depth < 0x1000) {
                u32 *entry = (u32 *)(depth * 4 + (u32)ordering);
                ROOMFX_ADDPRIM(entry, packet);
                ((RenderPacket1C *)packet)->sxy0 = xy0;
                ((RenderPacket1C *)packet)->sxy1 = xy1;
                ((RenderPacket1C *)packet)->sxy2 = xy2;
            }
        } else {
            ROOMFX_SETADDR(packet, 0);
        }
        packets += 2 * sizeof(RenderPacket1C);
        i++;
        record++;
    }
}
