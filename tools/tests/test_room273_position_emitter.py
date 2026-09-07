import pathlib
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m273/RoomEffect_PositionLayers.c'


def emitter_source():
    source = SOURCE.read_text()
    callback = source.index('int func_801977F8')
    emitter = source.index('int func_80197A48')
    return source[:callback] + 'extern int func_801977F8(int, void *);\n' + source[emitter:]


class PositionEmitter(unittest.TestCase):
    def test_target_layout(self):
        source = emitter_source() + r'''
typedef char layout[sizeof(Vector)==8 && __alignof__(Vector)==2 &&
    (unsigned long)&((Vector *)0)->y==2 &&
    (unsigned long)&((Vector *)0)->z==4 &&
    (unsigned long)&((Vector *)0)->pad==6 &&
    (unsigned long)&((Context *)0)->pool==8 ? 1:-1];
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
        source = re.sub(r'asm\(""[^;]*;', '', emitter_source())
        source = '#include <assert.h>\n#include <limits.h>\n' + source + r'''
Context *D_800F33E0;
unsigned char D_8019AF68,D_8019AF69;
unsigned short D_8019AEFC;
volatile unsigned short D_8019AEFE,D_8019AF00;
unsigned short D_800E11EA,D_800E2850[65536];
unsigned short D_800F3368,D_800F336A,D_800F336C,D_800F336E;
unsigned short D_800F3370,D_800F3372,D_800F3374;
volatile unsigned short D_800F3376,D_800F3378;
static Context context;
static Vector output;
static int pool,initCalls,initResult,allocs,fail;
int func_801977F8(int mode,void *p) { assert(0); return 0; }
int func_800CE560(void *p,int size,int count,int (*callback)(int,void *)) {
    assert(p==&pool && size==8 && count==4 && callback==func_801977F8);
    ++initCalls;
    return initResult;
}
Vector *func_800CE610(void *p) {
    assert(p==&pool && allocs++==0);
    if(fail) return 0;
    /* Source coordinates must be read after allocation. */
    D_8019AEFC=65535; D_8019AEFE=32768; D_8019AF00=123;
    return &output;
}
int main(void) {
    int stop,emit,i,calls;
    unsigned char flags[]={0,1,255};
    unsigned short indices[]={0,1,32768,65535};
    D_800F33E0=&context; context.pool=&pool;
    for(initResult=-1;initResult<=1;++initResult)
        assert(func_80197A48(0)==initResult);
    for(stop=0;stop<3;++stop) for(emit=0;emit<3;++emit)
        for(fail=0;fail<2;++fail) {
            output.x=11; output.y=22; output.z=33; output.pad=0xA55A;
            D_8019AEFC=1; D_8019AEFE=2; D_8019AF00=3;
            D_8019AF68=flags[emit]; D_8019AF69=flags[stop]; allocs=0;
            assert(func_80197A48(1)==(stop ?2:0));
            calls=!stop && emit;
            assert(allocs==calls);
            assert((unsigned short)output.x==(calls && !fail ?65535:11));
            assert((unsigned short)output.y==(calls && !fail ?32768:22));
            assert((unsigned short)output.z==(calls && !fail ?123:33));
            assert((unsigned short)output.pad==0xA55A);
            assert(D_8019AF68==flags[emit] && D_8019AF69==flags[stop]);
        }
    calls=allocs;
    for(i=0;i<4;++i) {
        D_800E11EA=indices[i]; D_800E2850[indices[i]]=65535-i;
        D_800F3368=D_800F336A=D_800F336C=D_800F336E=0xAAAA;
        D_800F3370=D_800F3372=D_800F3374=0xAAAA;
        D_800F3376=D_800F3378=0xAAAA;
        assert(func_80197A48(2)==0 && allocs==calls);
        assert(D_800F3368==32 && D_800F336A==2 && D_800F336C==3);
        assert(D_800F336E==0 && D_800F3370==65535-i && D_800F3372==0);
        assert(D_800F3374==64 && D_800F3376==32 && D_800F3378==32);
    }
    assert(func_80197A48(-1)==0 && func_80197A48(3)==0);
    assert(func_80197A48(INT_MIN)==0 && func_80197A48(INT_MAX)==0);
    assert(initCalls==3 && allocs==calls);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'behavior.c'
            path.write_text(source)
            exe = path.with_suffix('')
            subprocess.run(['cc', '-std=c99', '-O2', str(path), '-o', str(exe)],
                           check=True)
            subprocess.run([str(exe)], check=True)
