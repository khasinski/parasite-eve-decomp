#ifndef PE1_SCENE_FX_H
#define PE1_SCENE_FX_H

typedef struct SceneFxVec3s { short x; short y; short z; short pad6; } SceneFxVec3s;
typedef struct SceneFxVecTable16 { SceneFxVec3s entries[16]; short scale; short count; } SceneFxVecTable16;
typedef struct SceneFxVecState {
    short x;
    short y;
    short z;
    short pad6;
    short scale;
    short zero;
} SceneFxVecState;

#endif
