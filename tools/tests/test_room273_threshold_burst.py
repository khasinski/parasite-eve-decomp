import pathlib
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m273/RoomEffect_ThresholdBurst.c'


def controller_source():
    source = SOURCE.read_text()
    callback = source.index('int func_80194470')
    controller = source.index('int func_801945A8')
    return source[:callback] + 'extern int func_80194470();\n' + source[controller:]


class ThresholdBurst(unittest.TestCase):
    def test_target_layout(self):
        source = controller_source() + r'''
typedef char layout[sizeof(Vector)==8 &&
    (unsigned long)&((Context *)0)->pool==8 &&
    (unsigned long)&((StateContext *)0)->state==8 &&
    (unsigned long)&((State *)0)->kind==14 &&
    (unsigned long)&((State *)0)->value22==22 &&
    (unsigned long)&((State *)0)->value26==26 &&
    (unsigned long)&((Owner *)0)->transform==0x238 &&
    (unsigned long)&((Transform *)0)->x==20 &&
    (unsigned long)&((Transform *)0)->y==24 &&
    (unsigned long)&((Transform *)0)->z==28 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'layout.c'
            path.write_text(source)
            result = subprocess.run([str(ROOT / 'tools/scripts/cc.sh'), str(path),
                                     str(path.with_suffix('.o'))], cwd=ROOT,
                                    capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    @unittest.skipUnless(shutil.which('cc'), 'host compiler unavailable')
    def test_behavior(self):
        source = re.sub(r'asm\(""[^;]*;', '', controller_source())
        source = '#include <assert.h>\n#include <string.h>\n#include <limits.h>\n' + source + r'''
static Context context;
static StateContext stateContext;
static State state;
static Owner owner,replacement;
static Transform transform;
static Vector output;
static unsigned char pool[64];
Context *D_800F33E0=&context;
StateContext *D_800F32D0=&stateContext;
Owner *g_PlayerEntity=&owner;
unsigned char D_8019AE9A;
short D_8019AE60,D_8019AE64;
unsigned short D_800E11FA,D_800E2850[8];
unsigned short D_800F3368,D_800F336A,D_800F336C,D_800F336E;
unsigned short D_800F3370,D_800F3372,D_800F3374;
volatile unsigned short D_800F3376,D_800F3378;
static int init,allocations,fail;
int func_80194470(void) { assert(0); return 0; }
int func_800CE560(void *p,int stride,int count,int (*callback)()) {
    assert(init++==0 && p==pool && stride==8 && count==4 && callback==func_80194470);
    assert(D_8019AE60==0 && D_8019AE64==0); return 60;
}
Vector *func_800CE610(void *p) {
    assert(p==pool && allocations++==0);
    g_PlayerEntity=&replacement;
    return fail ? 0 : &output;
}
int main(void) {
    int values[]={-32768,-1,0,1,2,32767};
    int counts[]={-1,0,1,4};
    int stop,kind,i,j,c,d,f;
    context.pool=pool; stateContext.state=&state; replacement.transform=&transform;
    transform.x=65535; transform.y=-65537; transform.z=32768;
    D_8019AE60=9; D_8019AE64=7;
    assert(func_801945A8(0)==60 && init==1);
    for(stop=0;stop<2;++stop) for(kind=8;kind<=9;++kind)
        for(i=0;i<6;++i) for(j=0;j<6;++j) for(c=0;c<4;++c)
        for(d=0;d<6;++d) for(f=0;f<2;++f) {
            State before;
            short count=counts[c],delay=values[d];
            int wanted=0;
            memset(&state,0xA5,sizeof(state));
            state.kind=kind; state.value22=values[i]; state.value26=values[j]; before=state;
            D_8019AE9A=stop; D_8019AE60=count; D_8019AE64=delay;
            if(!stop) {
                if(kind==9 && values[i]>=2 && values[j]<2) { count=4; delay=0; }
                if(count) {
                    short old=delay; delay=(short)(delay-1);
                    if(old<=0) { wanted=1; if(!f) { delay=2; --count; } }
                }
            }
            allocations=0; fail=f; g_PlayerEntity=&owner;
            memset(&output,0xA5,sizeof(output));
            assert(func_801945A8(1)==(stop ? 2:0));
            assert(allocations==wanted && D_8019AE60==count && D_8019AE64==delay);
            assert(memcmp(&state,&before,sizeof(state))==0 && output.pad==(short)0xA5A5);
            if(wanted && !f) assert(output.x==-1 && output.y==-1 && output.z==-32768);
            else assert(output.x==(short)0xA5A5 && output.y==(short)0xA5A5 && output.z==(short)0xA5A5);
        }
    for(i=0;i<8;++i) {
        D_800E11FA=i; D_800E2850[i]=65535-i;
        assert(func_801945A8(2)==0);
        assert(D_800F3368==32 && D_800F336A==2 && D_800F336C==3 && D_800F336E==1);
        assert(D_800F3370==65535-i && D_800F3372==0 && D_800F3374==32);
        assert(D_800F3376==32 && D_800F3378==32);
    }
    allocations=0;
    assert(func_801945A8(-1)==0 && func_801945A8(3)==0);
    assert(func_801945A8(INT_MIN)==0 && func_801945A8(INT_MAX)==0);
    assert(!allocations && init==1);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'behavior.c'
            path.write_text(source)
            exe = path.with_suffix('')
            subprocess.run(['cc', '-std=c99', '-O2', str(path), '-o', str(exe)], check=True)
            subprocess.run([str(exe)], check=True)
