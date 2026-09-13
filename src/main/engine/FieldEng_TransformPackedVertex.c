#include "pe1/render_object.h"

void FieldEng_TransformPackedVertex(RenderPackedGeometry *geometry,
                                   GteMatrix *matrix, u16 index,
                                   GteShortVector *out) {
    u16 vertex_offset;
    u32 translation;

    /* The packed format deliberately truncates the byte offset to 16 bits. */
    vertex_offset = 16
        + (geometry->counts.record12_counts[0] + geometry->counts.record12_counts[1]) * 12
        + (geometry->counts.record16_counts[0] + geometry->counts.record16_counts[1]) * 16;
    ApplyMatrixSV(matrix,
                  (GteShortVector *)&geometry->bytes[vertex_offset] + index,
                  out);
    translation = matrix->t[0];
    out->x += translation;
    translation = matrix->t[1];
    out->y += translation;
    translation = matrix->t[2];
    out->z += translation;
}
