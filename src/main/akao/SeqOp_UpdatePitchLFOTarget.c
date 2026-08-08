#include "pe1/akao.h"

void SeqOp_UpdatePitchLFOTarget(AkaoTrack *track) {
    AkaoU8 *pc;
    int value;
    unsigned int masked;
    unsigned int depth;
    int scale;
    int scaled;
    int product;
    int use_base_scale;

    pc = track->pc;
    track->pc = pc + 1;
    value = pc[0] << 8;
    masked = value & 0x7F00;
    asm volatile("" : "=r"(masked) : "0"(masked));
    depth = masked >> 8;
    track->pitch_lfo_target = value;
    use_base_scale = value & 0x8000;
    scale = track->pitch_base;
    if (use_base_scale == 0) {
        scaled = ((scale << 4) - scale) >> 8;
        product = depth * scaled;
    } else {
        product = depth * scale;
    }
    track->pitch_lfo_depth = (unsigned int)product >> 7;
}
