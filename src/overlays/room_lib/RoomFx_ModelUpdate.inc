#include "pe1/gte.h"
#include "pe1/room_fx_model.h"
#ifndef ROOMFX_MODEL_UPDATE_FUNC
#error "ROOMFX_MODEL_UPDATE_FUNC must name the room entry point"
#endif
#ifndef ROOMFX_MODEL_UPDATE_AXES
#error "ROOMFX_MODEL_UPDATE_AXES must name the room's fixed axis matrix"
#endif

extern GteMatrix ROOMFX_MODEL_UPDATE_AXES;

/* Mirror the owner's model across the guide line: reflect its position
 * through the line, flip its rotation and point the model along the line,
 * then rebuild the animation matrices. */
void ROOMFX_MODEL_UPDATE_FUNC(RoomFxModelGeom *geom) {
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
    distance /= Gte_ISqrt(slope * slope + 1);
    angle = Gte_Atan2(dz, dx);
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
    gte_ldclmv(&ROOMFX_MODEL_UPDATE_AXES.m[0][0]);
    gte_rtir();
    gte_stclmv(&geom->matrix.m[0][0]);
    gte_ldclmv(&ROOMFX_MODEL_UPDATE_AXES.m[0][1]);
    gte_rtir();
    gte_stclmv(&geom->matrix.m[0][1]);
    gte_ldclmv(&ROOMFX_MODEL_UPDATE_AXES.m[0][2]);
    gte_rtir();
    gte_stclmv(&geom->matrix.m[0][2]);
    func_80039B74(geom, owner->animation, owner->matrix_count, 1);
}
