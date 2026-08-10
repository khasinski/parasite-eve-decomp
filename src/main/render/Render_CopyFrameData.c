#include "common.h"
#include "pe1/render_object.h"

void Render_CopyFrameData(RenderObjectEntity *dst, RenderObjectEntity *src, s32 frame) {
    s16 index = frame;

    dst->model_matrix.rotation[0][0] = src->matrices[index].rotation[0][0];
    dst->model_matrix.rotation[0][1] = src->matrices[index].rotation[0][1];
    dst->model_matrix.rotation[0][2] = src->matrices[index].rotation[0][2];
    dst->model_matrix.rotation[1][0] = src->matrices[index].rotation[1][0];
    dst->model_matrix.rotation[1][1] = src->matrices[index].rotation[1][1];
    dst->model_matrix.rotation[1][2] = src->matrices[index].rotation[1][2];
    dst->model_matrix.rotation[2][0] = src->matrices[index].rotation[2][0];
    dst->model_matrix.rotation[2][1] = src->matrices[index].rotation[2][1];
    dst->model_matrix.rotation[2][2] = src->matrices[index].rotation[2][2];
    dst->model_matrix.translation[0] = src->matrices[index].translation[0];
    dst->model_matrix.translation[1] = src->matrices[index].translation[1];
    dst->model_matrix.translation[2] = src->matrices[index].translation[2];
}
