#include "pe1/psyq_ds.h"

int DsControlF(u_char command, u_char *parameter) {
    return Render_AllocParticleNode(command, parameter, 0, 0);
}
