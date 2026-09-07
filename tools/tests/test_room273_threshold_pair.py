import pathlib
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m273/RoomEffect_ThresholdPair.c'


class ThresholdPair(unittest.TestCase):
    def test_target_layout(self):
        source = SOURCE.read_text() + r'''
typedef char layout[sizeof(Seed)==8 &&
    (unsigned long)&((Seed *)0)->angle==2 &&
    (unsigned long)&((Context *)0)->pool==8 &&
    (unsigned long)&((StateContext *)0)->state==8 &&
    (unsigned long)&((State *)0)->kind==14 &&
    (unsigned long)&((State *)0)->value22==22 &&
    (unsigned long)&((State *)0)->value26==26 ? 1:-1];
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
        source = re.sub(r'asm\(""[^;]*;', '', SOURCE.read_text())
        source = '#include <assert.h>\n#include <string.h>\n#include <limits.h>\n' + source + r'''
static Context context;
static StateContext stateContext;
static State state;
static Seed output[2];
static unsigned char pool[32],secondPool[32];
Context *D_800F33E0=&context;
StateContext *D_800F32D0=&stateContext;
unsigned char D_8019AE9A;
Seed D_8019AC20[2];
unsigned short D_800E11FA,D_800E2850[8];
unsigned short D_800F3368,D_800F336A,D_800F336C,D_800F336E;
unsigned short D_800F3370,D_800F3372,D_800F3374;
volatile unsigned short D_800F3376,D_800F3378;
static int init,allocations,randomCalls,failAt,randomValue;
int func_80194128(void) { assert(0); return 0; }
int func_800CE560(void *p,int stride,int count,int (*callback)()) {
    assert(init++==0 && p==pool && stride==8 && count==4 && callback==func_80194128);
    return 60;
}
int Inv_ScrambleGrid(void) { assert(randomCalls++==0 && !allocations); return randomValue; }
Seed *func_800CE610(void *p) {
    int index=allocations++;
    assert(randomCalls==1 && index<2 && p==(index ? secondPool:pool));
    context.pool=secondPool;
    return index==failAt ? 0 : &output[index];
}
int main(void) {
    int values[]={-32768,-1,0,1,32767};
    int stop,kind,i,j,f,r,n;
    context.pool=pool; stateContext.state=&state;
    assert(func_80194284(0)==60 && init==1);
    for(stop=0;stop<2;++stop) for(kind=8;kind<=10;++kind)
        for(i=0;i<5;++i) for(j=0;j<5;++j) for(f=0;f<3;++f) for(r=0;r<2;++r) {
            State before;
            int wanted=!stop && kind==9 && values[i]>0 && values[j]<=0;
            memset(&state,0xA5,sizeof(state));
            state.kind=kind; state.value22=values[i]; state.value26=values[j];
            before=state; D_8019AE9A=stop ? 255:0; context.pool=pool;
            memset(D_8019AC20,0x39,sizeof(D_8019AC20));
            D_8019AC20[0].angle=65530; D_8019AC20[1].angle=32768;
            memset(output,0xA5,sizeof(output));
            allocations=randomCalls=0; failAt=f; randomValue=r ? 255:0;
            assert(func_80194284(1)==(stop ? 2:0));
            assert(randomCalls==wanted && allocations==(wanted ? (f<2 ? f+1:2):0));
            assert(memcmp(&state,&before,sizeof(state))==0);
            for(n=0;n<2;++n) {
                Seed expected;
                if(wanted && n<f) {
                    expected=D_8019AC20[n]; expected.angle+=randomValue*16;
                } else memset(&expected,0xA5,sizeof(expected));
                assert(memcmp(&output[n],&expected,sizeof(expected))==0);
            }
        }
    for(i=0;i<8;++i) {
        D_800E11FA=i; D_800E2850[i]=65535-i;
        assert(func_80194284(2)==0);
        assert(D_800F3368==16 && D_800F336A==1 && D_800F336C==3 && D_800F336E==1);
        assert(D_800F3370==65535-i && D_800F3372==3 && D_800F3374==0);
        assert(D_800F3376==16 && D_800F3378==64);
    }
    allocations=randomCalls=0;
    assert(func_80194284(-1)==0 && func_80194284(3)==0);
    assert(func_80194284(INT_MIN)==0 && func_80194284(INT_MAX)==0);
    assert(!allocations && !randomCalls && init==1);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'behavior.c'
            path.write_text(source)
            exe = path.with_suffix('')
            subprocess.run(['cc', '-std=c99', '-O2', str(path), '-o', str(exe)], check=True)
            subprocess.run([str(exe)], check=True)
