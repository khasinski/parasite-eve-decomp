import pathlib
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m350/RoomEffect_AnimationEventController.c'


class AnimationEventTests(unittest.TestCase):
    def test_target_layout(self):
        source = SOURCE.read_text().split('extern Actor', 1)[0] + r'''
#define OFF(t,f) ((unsigned long)&((t *)0)->f)
typedef char a[sizeof(Events)==32 && sizeof(Event)==4 ? 1:-1];
typedef char b[OFF(Owner,status)==0x18 && OFF(Actor,instance)==8 ? 1:-1];
typedef char c[OFF(Instance,animation)==0xE && OFF(Instance,frameCount)==0xF ? 1:-1];
typedef char d[OFF(Instance,time)==0x14 && OFF(Instance,previousFrame)==0x1A ? 1:-1];
typedef char e[OFF(Instance,localPosition)==0x28 && OFF(Instance,yaw)==0x3A ? 1:-1];
typedef char f[OFF(Instance,transform)==0x1E8 && OFF(Matrix,position)==0x14 ? 1:-1];
typedef char g[sizeof(Vector)==8 && sizeof(Matrix)==32 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'layout.c'
            path.write_text(source)
            result = subprocess.run([str(ROOT/'tools/scripts/cc.sh'), str(path), str(path.with_suffix('.o'))], cwd=ROOT, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    @unittest.skipUnless(shutil.which('cc'), 'host compiler unavailable')
    def test_events_and_helper_boundaries(self):
        source = SOURCE.read_text()
        source = re.sub(r'extern unsigned char D_8019A854[^;]*;', '', source)
        source = source.replace('extern int D_8019A858;', '')
        source = re.sub(r'extern short D_8019A85C[^;]*;', '', source)
        source = source.replace('extern int D_800966EC[];', '')
        source = source.replace('extern short D_800966EE[];', '')
        prefix = r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
static union { int words[8]; short halves[16]; unsigned char bytes[32]; } globals;
static union { int words[4097]; short halves[8194]; } trig;
#define D_8019A854 (globals.bytes)
#define D_8019A858 (globals.words[1])
#define D_8019A859 (globals.bytes[5])
#define D_8019A85C (globals.halves[4])
#define D_8019A85E (globals.halves[5])
#define D_8019A86A (globals.halves[11])
#define D_8019A86C (globals.halves[12])
#define D_8019A86E (globals.bytes[26])
#define D_800966EC (trig.words)
#define D_800966EE (trig.halves+1)
'''
        harness = prefix + source + r'''
Actor *D_800F32D0;
Instance *g_PlayerEntity;
Events D_8018F1CC;
static Actor actors[2];
static Instance instances[2],player;
static Owner owner;
static unsigned char status;
static short cursor;
static int angles,turns,moves,expectedSpeed,expectedZ,mutate;
static int word(uint32_t x) { int y; memcpy(&y,&x,4); return y; }
static int half(unsigned int x) { return (x&65535)<32768 ? (int)(x&65535):(int)(x&65535)-65536; }
static int floor64(int x) { return x>=0 ? x/64:-((-x+63)/64); }
int FieldEng_VecToAngle(int *a,int *b) {
    assert(a==player.transform.position && b==instances[0].transform.position);
    assert(++angles==1);
    if (mutate) {
        D_800F32D0=&actors[1];
        instances[0].yaw=-777;
        memset(&D_8018F1CC,255,sizeof(D_8018F1CC));
    }
    return 0x12348001;
}
int FieldEng_TurnToward(int yaw,int angle,int speed) {
    assert(++turns==1 && angles==1 && angle==-32767 && speed==expectedSpeed);
    assert(yaw==(mutate ? -777:123));
    return 0x1234ABCD;
}
void ApplyMatrixSV(Matrix *m,Vector *a,Vector *b) {
    assert(++moves==1 && m==&instances[0].transform && a==b);
    assert(a->x==0 && a->y==0 && a->z==expectedZ);
    /* Only the helper boundary is modeled, not GTE arithmetic. */
    a->x=-32768; a->y=111; a->z=32767;
    if (mutate) {
        D_800F32D0=&actors[1];
        instances[0].localPosition[0]=INT_MIN;
        instances[0].localPosition[2]=INT_MAX;
        D_8019A86A=-32768;
    }
}
static void reset(void) {
    int i;
    memset(&globals,0,sizeof(globals));
    memset(instances,0,sizeof(instances));
    memset(&player,0,sizeof(player));
    actors[0].instance=&instances[0]; actors[1].instance=&instances[1];
    D_800F32D0=&actors[0]; g_PlayerEntity=&player;
    owner.status=&status; instances[0].owner=&owner; status=1;
    instances[0].frameCount=64; instances[0].yaw=123;
    instances[0].localPosition[0]=INT_MAX-12;
    instances[0].localPosition[1]=42;
    instances[0].localPosition[2]=INT_MIN+12;
    for (i=0;i<8;i++) {
        D_8018F1CC.entries[i].frame=i==7 ? 255:7+i*4;
        D_8018F1CC.entries[i].slot=i;
        D_8018F1CC.entries[i].value=100+i;
        D_8018F1CC.entries[i].pad=0xA5;
    }
    cursor=0; angles=turns=moves=mutate=0;
}
static void run(int frame,int previous,int start,int animation,int repeat,int recoil,int cooldown,int hasOwner,int mutation) {
    int phase=0,result,expectedResult=0,expectedCursor=start,i;
    int expectedStatus=hasOwner ? 2:1,expectedRepeat=repeat,expectedMotion=recoil;
    int expectedTime=frame<<16,expectedCooldown=half((unsigned int)cooldown-(cooldown!=0));
    unsigned char flags[8]={0};
    int done=0,ended=0,doTurn=0,doMove=0;
    reset(); cursor=start; mutate=mutation;
    instances[0].time.parts.frame=frame;
    instances[0].previousFrame=previous;
    instances[0].animation=animation;
    if (!hasOwner) instances[0].owner=0;
    D_8019A85E=repeat; D_8019A86A=recoil; D_8019A86C=cooldown;
    if (animation==6) {
        if (half(frame)>=6 && half(previous)<6) flags[5]=5;
        done=1;
    } else if (repeat>0) {
        if (half(frame)>=31) { expectedTime=0x60000; expectedRepeat--; }
    } else if (half(frame)>=63) {
        if (hasOwner) expectedStatus=4;
        ended=1; expectedResult=1; done=1;
    }
    if (!done) {
        if (frame>=7 && frame<15) phase=15-frame;
        else if (frame>=19 && frame<27) phase=27-frame;
        doTurn=phase!=0;
        expectedSpeed=floor64(half((unsigned)trig.words[phase*256]));
        for (i=0;i<8;i++) {
            Event *e=&D_8018F1CC.entries[expectedCursor];
            if (half(frame)<e->frame || half(previous)>=e->frame) break;
            flags[e->slot]=e->value;
            if (++expectedCursor==8) expectedCursor=2;
        }
        assert(i<8);
        if (recoil>0) {
            doMove=1;
            expectedZ=D_800966EE[((8-recoil)*512 & 0x3E00)/2]/256;
            expectedMotion=mutation ? 32767:recoil-1;
        }
    }
    result=func_80197DA8(1,&cursor);
    assert(result==expectedResult && cursor==expectedCursor);
    assert(status==expectedStatus && D_8019A85E==expectedRepeat);
    assert(D_8019A86C==expectedCooldown && D_8019A86A==expectedMotion);
    assert(D_8019A86E==ended && memcmp(flags,globals.bytes,8)==0);
    assert(instances[0].time.fixed==expectedTime);
    assert(angles==doTurn && turns==doTurn && moves==doMove);
    assert(instances[0].yaw==(doTurn ? half(0xABCD):123));
    assert(instances[0].localPosition[1]==42);
    if (doMove) {
        int x=mutation ? INT_MIN:INT_MAX-12;
        int z=mutation ? INT_MAX:INT_MIN+12;
        assert(instances[0].localPosition[0]==word((uint32_t)x+0x80000000u));
        assert(instances[0].localPosition[2]==word((uint32_t)z+0x7FFF0000u));
    }
}
int main(void) {
    int f,p,c,i;
    for (i=0;i<4097;i++) trig.words[i]=word(0xFEDC0000u+(unsigned)(i*211));
    reset(); memset(&globals,0x55,sizeof(globals)); cursor=4;
    assert(func_80197DA8(0,&cursor)==0 && cursor==0);
    for(i=0;i<8;i++) assert(globals.bytes[i]==0);
    assert(D_8019A85C==0 && D_8019A86A==0 && D_8019A86C==0 && D_8019A86E==0);
    assert(D_8019A85E==0x5555 && globals.bytes[27]==0x55 && status==1);
    reset(); cursor=3; D_8019A86C=7;
    assert(func_80197DA8(-1,&cursor)==0 && cursor==3 && D_8019A86C==7);
    assert(func_80197DA8(2,&cursor)==0 && status==1);
    reset(); cursor=7;
    instances[0].time.parts.frame=10; instances[0].previousFrame=8;
    D_8018F1CC.entries[7].frame=9;
    expectedSpeed=floor64(half((unsigned)trig.words[5*256]));
    assert(func_80197DA8(1,&cursor)==0 && cursor==2 && globals.bytes[7]==107);
    for (i=0;i<256;i++) {
        reset(); status=i; instances[0].animation=6;
        assert(func_80197DA8(1,&cursor)==0 && status==(i==1 ? 2:i));
    }
    for (f=0;f<=40;f++) for(p=0;p<=40;p++) for(c=0;c<8;c++)
        run(f,p,c,0,1,8,3,1,0);
    for (f=0;f<=64;f++) for(p=0;p<=8;p++) {
        run(f,p,0,6,0,8,-32768,1,0);
        run(f,p,0,0,0,0,0,0,0);
    }
    for (i=0;i<65536;i++) {
        trig.words[2048]=word(0xF00D0000u+(unsigned)i);
        run(7,6,0,0,0,0,1,1,1);
        D_800966EE[0]=half(i);
        run(16,15,3,0,0,8,1,1,1);
    }
    run(31,30,6,0,1,1,-1,1,1);
    run(65535,65534,0,0,0,-32768,32767,1,0);
    run(32768,32767,0,0,0,32767,0,0,0);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'test.c'
            path.write_text(harness)
            executable = path.with_suffix('')
            result = subprocess.run(['cc', '-std=gnu11', '-O2', '-fno-strict-aliasing', '-fwrapv', str(path), '-o', str(executable)], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run([str(executable)], capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
