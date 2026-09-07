import pathlib
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m350/RoomEffect_TurningAreaController.c'


class TurningAreaTests(unittest.TestCase):
    def test_target_layout(self):
        source = SOURCE.read_text().split('extern Actor', 1)[0] + r'''
#define OFF(t,f) ((unsigned long)&((t *)0)->f)
typedef char a[sizeof(State)==20 && OFF(State,yaw)==4 && OFF(State,size)==8 ? 1:-1];
typedef char b[OFF(State,brightness)==10 && OFF(State,turn)==14 && OFF(State,initialized)==16 ? 1:-1];
typedef char c[sizeof(Vector)==8 && sizeof(Matrix)==32 && OFF(Matrix,position)==20 ? 1:-1];
typedef char d[sizeof(Scale)==16 && OFF(Owner,status)==0x4C ? 1:-1];
typedef char e[OFF(Instance,localPosition)==0x28 && OFF(Instance,position)==0x1FC ? 1:-1];
typedef char f[OFF(Instance,transforms)==0x238 && OFF(Actor,instance)==8 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'layout.c'
            path.write_text(source)
            result = subprocess.run([str(ROOT/'tools/scripts/cc.sh'), str(path), str(path.with_suffix('.o'))], cwd=ROOT, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    @unittest.skipUnless(shutil.which('cc'), 'host compiler unavailable')
    def test_events_and_hardware_boundary(self):
        source = SOURCE.read_text().replace('#include "pe1/gte.h"', '')
        # Keep the target call stub separate from host libc used by the harness.
        source = re.sub(r'\bmemset\b', 'target_memset', source)
        source = re.sub(r' asm\("\$\d+"\)', '', source)
        transfer = 'asm("" : "=r"(high) : "0"(intermediate), "r"(product));'
        self.assertEqual(source.count(transfer), 1)
        source = source.replace(transfer, 'high=intermediate;')
        # All other empty asms are tied identities; preserve values, not MIPS constraints.
        source = re.sub(r'\basm\s*(?:volatile\s*)?\(""[^;]*\);', 'asm volatile("" ::: "memory");', source)
        source = re.sub(r'extern short D_8019A80A[^;]*;', '', source)
        source = re.sub(r'extern int D_8019A81C[^;]*;', '', source)
        source = source.replace('D_800966EC[],D_800966EE[]', 'D_800966EC[]')
        prefix = r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
static void load_matrix(void *,int),load_vertex(void *),transform(void),screen(int,int),clip(int *);
static int ir(int);
#define gte_ldrotmatrix(p) load_matrix(p,0)
#define gte_ldtransmatrix(p) load_matrix(p,1)
#define gte_ldv0(p) load_vertex(p)
#define gte_rtv0tr_mac() transform()
#define gte_mfc2_9(x) ((x)=ir(0))
#define gte_mfc2_10(x) ((x)=ir(1))
#define gte_mfc2_11(x) ((x)=ir(2))
#define gte_ldsxy0(x) screen(0,x)
#define gte_ldsxy1(x) screen(1,x)
#define gte_ldsxy2(x) screen(2,x)
#define gte_nclip() ((void)0)
#define gte_stmac0(p) clip(p)
#define D_8019A80A D_8019A808.rotation[0][1]
#define D_8019A80C D_8019A808.rotation[0][2]
#define D_8019A80E D_8019A808.rotation[1][0]
#define D_8019A810 D_8019A808.rotation[1][1]
#define D_8019A812 D_8019A808.rotation[1][2]
#define D_8019A814 D_8019A808.rotation[2][0]
#define D_8019A816 D_8019A808.rotation[2][1]
#define D_8019A818 D_8019A808.rotation[2][2]
#define D_8019A81C D_8019A808.position[0]
#define D_8019A820 D_8019A808.position[1]
#define D_8019A824 D_8019A808.position[2]
#define D_800966EE (D_800966EC+1)
'''
        harness = prefix + source + r'''
Actor *D_800F32D0;
Instance *g_PlayerEntity;
void *D_800B0E64;
short D_8019A82A,D_800966EC[8193];
unsigned char D_8019A831,D_8019A82F,D_8019A830;
int D_800E27EC,D_800F3428;
Matrix D_8019A808;
Vector D_8019A574[4];
char D_8019A594[8],D_8019A3C8[8];
unsigned short D_800E11FA,D_800E2850[65536],D_800F336C,D_800E1204[65536];
static Actor actors[2];
static Instance instances[2],players[2];
static Owner owners[3];
static Matrix transforms[34],expectedMatrix;
static State state;
static int assets[8],mode,mutate,calls,distances,angles,cosines,sines;
static int matrixLoads,vertices,irIndex,screens,clips,outsideAt,turnDistance,turnAngle;
static int expectedYaw,cosineYaw,expectedBright,clears,scaled,models,sprites,drawStage;
static int tailSize,tailYaw,expectedSpriteYaw,initialScale,kind,special;
static uint32_t packed[3];
static int word(uint32_t u) { int x; memcpy(&x,&u,4); return x; }
static int half(uint32_t u) { return (u&65535)<32768 ? (int)(u&65535):(int)(u&65535)-65536; }
static int sar(int x,int n) { return x>=0 ? x/(1<<n):word(~((uint32_t)~x>>n)); }
static int pack(int x,int z) { return word(((uint32_t)z<<16)|(uint16_t)x); }
static int coord(int v,int axis) { return word(0x76540000u+(uint32_t)(v*97+axis*311+0x8000)); }
static int next_size(int n) { return n==1 ? -32768:32767; }
static void reset(void) {
    memset(&state,0x5A,sizeof(state)); memset(instances,0,sizeof(instances));
    memset(players,0,sizeof(players)); memset(owners,0,sizeof(owners));
    actors[0].instance=&instances[0]; actors[1].instance=&instances[1]; D_800F32D0=&actors[0];
    instances[0].owner=&owners[0]; instances[0].transforms=transforms;
    players[0].owner=&owners[1]; players[1].owner=&owners[2]; g_PlayerEntity=&players[0];
    instances[0].position[0]=-100; instances[0].position[1]=200; instances[0].position[2]=300;
    players[0].position[0]=17; players[0].position[2]=-19;
    players[1].position[0]=-32768; players[1].position[2]=32767;
    owners[0].flags=0x1234; owners[1].status=0x5678; owners[2].status=0x9876;
    D_8019A82F=0; D_8019A830=73; D_8019A831=0;
    calls=distances=angles=cosines=sines=matrixLoads=vertices=irIndex=screens=clips=0;
    clears=scaled=models=sprites=drawStage=0; mutate=0; outsideAt=-1;
}
void *Asset_FindTable08ByU32Key(void *base,unsigned int id) {
    assert(mode==0 && calls++==0 && base==D_800B0E64 && id==0xC54E0704);
    return &assets[0];
}
void func_800C6D5C(void *asset,int a,int b) {
    assert(mode==0 && calls++==1 && asset==&assets[0] && !a && !b);
    D_8019A82A=-32768; state.size=123; state.initialized=255;
}
int func_800DFE20(int *a,int *b) {
    assert(mode==1 && distances++==0 && a==players[0].localPosition && b==instances[0].localPosition);
    if(mutate) { g_PlayerEntity=&players[1]; D_800F32D0=&actors[1]; }
    return turnDistance;
}
int FieldEng_VecToAngle(int *a,int *b) {
    assert(angles++==0 && distances==1 && a==players[mutate].localPosition && b==instances[0].localPosition);
    return turnAngle;
}
int rcos(int a) {
    assert(mode==1 && cosines++==0 && a==sar(expectedYaw,8)); cosineYaw=expectedYaw;
    if(mutate) { state.yaw=word((uint32_t)state.yaw+0x76540000u); expectedYaw=state.yaw; }
    return 0x12348000;
}
int rsin(int a) { assert(sines++==0 && cosines==1 && a==sar(expectedYaw,8)); return 0x76547FFF; }
static void check_matrix(void) {
    int r,c;
    for(r=0;r<3;r++) for(c=0;c<3;c++) {
        int v=r==c ? (r==1 ? 4096:-32768):r==0&&c==2 ? 32767:r==2&&c==0 ? -32767:0;
        assert(D_8019A808.rotation[r][c]==v);
    }
    for(r=0;r<3;r++) assert(D_8019A808.position[r]==instances[0].position[r]);
}
static void load_matrix(void *p,int which) {
    assert(p==&D_8019A808 && matrixLoads++==which && state.brightness>=64); check_matrix();
}
static void load_vertex(void *p) { assert(matrixLoads==2 && vertices<4 && p==&D_8019A574[vertices] && irIndex==0); }
static void transform(void) { assert(vertices<4 && irIndex==0); }
static int ir(int axis) {
    int v=vertices;
    assert(axis==irIndex++);
    if(axis==2) { vertices++; irIndex=0; }
    return coord(v,axis);
}
static void screen(int index,int value) {
    static const int order[3]={0,2,1};
    assert(vertices==4 && index==order[screens%3]); screens++; packed[index]=(uint32_t)value;
}
static void clip(int *out) {
    int prev=(clips+3)%4;
    assert(screens==(clips+1)*3);
    assert(packed[0]==(uint32_t)pack(g_PlayerEntity->position[0],g_PlayerEntity->position[2]));
    assert(packed[1]==(uint32_t)pack(coord(clips,0),coord(clips,2)));
    assert(packed[2]==(uint32_t)pack(coord(prev,0),coord(prev,2)));
    *out=clips==outsideAt ? -1:0; clips++;
}
static int reference_turn(int *yaw,int distance,int angle,int sample) {
    int d=((uint32_t)angle-(uint32_t)sar(*yaw,8))&4095,span,speed,factor;
    if(d<512) { *yaw=word((uint32_t)*yaw-0x20000u); d+=512; }
    if(d>3584) { d-=512; *yaw=word((uint32_t)*yaw+0x20000u); }
    if(d>2048) d=4096-d;
    span=half(distance)-512; if(span<0) span=0;
    factor=span>1024 ? 1024:1536;
    speed=sample*factor/4096; if(speed<0) speed=0; speed+=d;
    if((half(angle)-sar(*yaw,8))&2048) speed=-speed;
    return half(word((uint32_t)speed<<8)/128);
}
static void update(int timer,int sample,int initialized,int enabled,int distance,int angle,int yaw,int mutation,int outside,int inside,int nullOwner) {
    State before;
    int raw,expectedSize,turn,heading=yaw,index,ret;
    reset(); mode=1; mutate=mutation; outsideAt=outside;
    state.yaw=yaw; state.turn=-77; state.initialized=initialized;
    D_800E27EC=timer; D_8019A831=enabled; D_8019A82F=inside;
    if(nullOwner) instances[0].owner=0;
    turnDistance=distance; turnAngle=angle;
    if(timer<16) {
        index=(((uint32_t)timer<<8)&0x3F00)/2; D_800966EC[index]=sample;
        raw=sar(4096-sample,1)+2048;
    } else if(timer>=91) {
        index=(((uint32_t)(timer-90)<<8)&0x3F00)/2; D_800966EE[index]=sample;
        raw=sar(sample,1);
    } else raw=2048;
    expectedSize=raw/3; expectedBright=raw<=2048 ? sar(raw,4):sar(4096-raw,4);
    turn=state.turn;
    if(!initialized && enabled) {
        int span=half(distance)-512; if(span<0) span=0;
        index=((span/4)&4095)*2;
        /* Steering reads the same table and may alias the lifetime sample. */
        turn=reference_turn(&heading,distance,angle,D_800966EE[index]);
    } else if(!initialized) turn=0;
    expectedYaw=word((uint32_t)heading+(uint32_t)turn);
    before=state;
    ret=func_801960C4(1,&state);
    assert(state.size==expectedSize);
    if(expectedSize<=0) {
        before.size=expectedSize; assert(ret==1 && memcmp(&state,&before,sizeof(state))==0);
        assert(!cosines && !distances && !matrixLoads); return;
    }
    assert(ret==0 && state.brightness==expectedBright && state.turn==turn && state.yaw==expectedYaw);
    assert(state.initialized==(initialized ? initialized:1));
    assert(distances==(!initialized && enabled) && angles==distances && cosines==1 && sines==1);
    check_matrix();
    if(expectedBright<64) { assert(!matrixLoads && D_8019A82F==inside && D_8019A830==73); return; }
    assert(vertices==4 && clips==(outside<0 ? 4:outside+1));
    assert(D_8019A82F==(outside<0 ? (inside ? inside:1):0));
    assert(D_8019A830==((outside<0 && !inside) ? 1:73));
    assert(owners[0].flags==((outside<0 && !inside && !nullOwner) ? 0x80001234u:0x1234u));
    assert(g_PlayerEntity->owner->status==((mutate && distances ? 0x9876u:0x5678u)|((outside<0 && !inside) ? 0x4000u:0)));
}
int GetTPage(int a,int b,int c,int d) {
    assert(mode==2 && drawStage++==0 && !a && b==1 && !c && !d);
    D_800E11FA=65535; D_800F32D0=&actors[1];
    return word(0xABCD0100u);
}
int GetClut(int x,int y) {
    assert(drawStage++==1 && !x && y==D_800E1204[kind]+(kind==4 && special ? 7:3));
    state.asset=&assets[1]; return word(0xDEADFFFFu);
}
void GsSetOrign(int page,int clut) { assert(drawStage++==2 && page==(0x0100|0x8055) && clut==65535); }
void func_800C6ED8(int x) { assert(drawStage++==3 && x==1); }
void func_800C6EF8(void *p) { assert(drawStage++==4 && p==&assets[1]); state.asset=&assets[2]; state.brightness=-1; }
void func_800C6FA0(void *p,int b) { assert(drawStage++==5 && p==&assets[2] && b==65535); }
void target_memset(void *p,int a,int b) {
    assert(drawStage==6 && clears==models && scaled==models && !a && b==16);
    initialScale=state.size*(clears+1)/3; memset(p,0,b);
    state.size=next_size(++clears); state.asset=&assets[clears+2];
}
void Gte_ScaleMatrix(Matrix *m,Scale *s) {
    assert(clears==scaled+1 && models==scaled);
    assert(s->x==initialScale && s->y==initialScale && s->z==2048 && s->pad==0);
    assert(memcmp(m->rotation,expectedMatrix.rotation,sizeof(m->rotation))==0);
    assert(memcmp(m->position,expectedMatrix.position,sizeof(m->position))==0);
    m->position[0]=12345+scaled; scaled++;
}
void func_800C71E4(void *p,Matrix *m) {
    assert(clears==models+1 && scaled==clears && p==&assets[clears+2]);
    assert(m->position[0]==12345+models); models++;
    memset(&D_8019A808,0,sizeof(D_8019A808)); memset(transforms,0,sizeof(transforms));
}
void func_800C6F4C(void *p) {
    assert(models==3 && p==&assets[5]);
    state.size=tailSize; state.yaw=tailYaw; state.brightness=-123;
    instances[0].position[0]=word(0x12348000u); instances[0].position[1]=0x76547FFF; instances[0].position[2]=-98765;
    expectedSpriteYaw=half((uint32_t)sar(tailYaw,8)+2024);
}
void func_800D0E88(Vector *p,Vector *r,int sx,int sy,void *v,void *c,void *c2,int bright,int flag) {
    assert(models==3 && sprites<3 && p->x==-32768 && p->y==32767 && p->z==half(-98765));
    assert(r->x==0 && r->z==0 && r->y==expectedSpriteYaw);
    assert(sx==tailSize*6 && sy==512 && v==D_8019A594 && c==D_8019A3C8 && c2==c && flag==1);
    assert(bright==(sprites ? 1234+sprites:-123));
    r->y=sprites==0 ? 32767:-32768; expectedSpriteYaw=half((uint16_t)r->y+24);
    state.size=123; state.brightness=1235+sprites; instances[0].position[0]=999; sprites++;
}
static void draw(int size,int yaw,int k,int s) {
    int i;
    reset(); mode=2; kind=k; special=s; tailSize=size; tailYaw=yaw;
    state.size=size; D_800F336C=k; D_800F3428=s; D_800E2850[65535]=0x8055; D_800E1204[k]=65535;
    for(i=0;i<9;i++) ((short *)D_8019A808.rotation)[i]=i*19-71;
    transforms[33].position[0]=INT_MIN; transforms[33].position[1]=INT_MAX; transforms[33].position[2]=-333;
    expectedMatrix=D_8019A808; memcpy(expectedMatrix.position,transforms[33].position,sizeof(expectedMatrix.position));
    assert(func_801960C4(2,&state)==0 && sprites==3 && models==3);
}
int main(void) {
    int i,j; State before;
    static const int distances[]={INT_MIN,-1,0,511,512,1536,1537,32767,32768,65535};
    static const int angles0[]={0,511,512,2048,3584,3585,4095,0x12348000,-1};
    static const int yaws[]={INT_MIN,INT_MAX,0,-256,0x12345678};
    reset(); mode=0; D_800B0E64=&assets[7]; before=state;
    assert(func_801960C4(0,&state)==0 && calls==2);
    before.asset=&assets[0]; before.size=0; before.initialized=0; before.yaw=-8388608;
    assert(memcmp(&state,&before,sizeof(state))==0);
    for(i=-3;i<=4;i++) if(i<0 || i>2) {
        reset(); before=state; assert(func_801960C4(i,&state)==0 && !calls && memcmp(&before,&state,sizeof(state))==0);
    }
    for(i=0;i<8192;i++) D_800966EE[i]=half(i*97);
    for(i=-32768;i<=32767;i++) {
        update(0,i,1,0,0,0,0,0,-1,0,0);
        update(91,i,1,0,0,0,0,0,-1,0,0);
    }
    for(i=0;i<10;i++) for(j=0;j<9;j++)
        update(32,0,0,1,distances[i],angles0[j],yaws[(i+j)%5],(i+j)&1,-1,0,0);
    for(i=-1;i<4;i++) for(j=0;j<3;j++) {
        update(90,0,0,0,0,0,INT_MAX,0,i,j,j&1);
        update(16,0,1,1,0,0,INT_MIN,1,i,j,j&1);
    }
    for(i=0;i<5;i++) { draw(-32768,yaws[i],4,1); draw(32767,yaws[i],65535,1); draw(0,yaws[i],4,0); }
    for(i=0;i<65536;i++) draw(half(i),0,2,0);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'test.c'
            path.write_text(harness)
            exe = path.with_suffix('')
            result = subprocess.run(['cc', '-std=gnu11', '-O2', '-fno-strict-aliasing', '-fwrapv', str(path), '-o', str(exe)], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run([str(exe)], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)


if __name__ == '__main__':
    unittest.main()
