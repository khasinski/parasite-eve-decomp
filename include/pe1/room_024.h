#ifndef PE1_ROOM_024_H
#define PE1_ROOM_024_H

typedef struct Room024Variant38State {
    short final_x, final_y, final_z;
    unsigned char pad06[0xA];
    short final_alpha, final_scale;
    unsigned char pad14[4];
    short sparkle_x, sparkle_y, sparkle_z;
    unsigned char pad1E[2];
    short sparkle_alpha, sparkle_timer, resource_selector, active_flag;
    short drift_value, fade_alpha;
    unsigned char phase, pad2D;
    short transform_index;
    int draw_descriptor;
} Room024Variant38State;

#endif
