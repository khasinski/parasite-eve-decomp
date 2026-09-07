import pathlib
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m273/RoomEffect_LatchedPlayerEmitter.c'


class LatchedPlayer(unittest.TestCase):
    def test_target_layout(self):
        source = SOURCE.read_text() + r'''
typedef char layout[sizeof(Effect)==4 && sizeof(Position)==12 &&
    (unsigned long)&((Context *)0)->pool==8 &&
    (unsigned long)&((Owner *)0)->kind==14 &&
    (unsigned long)&((Owner *)0)->transform==0x238 &&
    (unsigned long)&((Transform *)0)->position==20 ? 1:-1];
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
static Owner owner,replacement;
static Transform transform;
static Effect output;
static Position sentinel;
static unsigned char pool[32];
Context *D_800F33E0=&context;
Owner *g_PlayerEntity=&owner;
int D_800E27EC;
unsigned char D_8019AE5C;
unsigned short D_800E11FA,D_800E2850[8];
unsigned short D_800F3368,D_800F336A,D_800F336C,D_800F336E;
unsigned short D_800F3370,D_800F3372,D_800F3374;
volatile unsigned short D_800F3376,D_800F3378;
static int init,allocations,fail;
int func_80193870(void) { assert(0); return 0; }
int func_800CE560(void *p,int stride,int count,int (*callback)()) {
    assert(init++==0 && p==pool && stride==4 && count==1 && callback==func_80193870);
    assert(D_8019AE5C==0); return 36;
}
Effect *func_800CE610(void *p) {
    assert(p==pool && allocations++==0 && D_8019AE5C==0);
    g_PlayerEntity=&replacement;
    return fail ? 0 : &output;
}
int main(void) {
    int counters[]={INT_MIN,-1,0,19,20,INT_MAX};
    int latches[]={0,1,255};
    int i,l,k,f;
    context.pool=pool; replacement.transform=&transform;
    D_8019AE5C=255;
    assert(func_801939B4(0)==36 && init==1);
    for(i=0;i<6;++i) for(l=0;l<3;++l) for(k=0;k<256;++k) for(f=0;f<2;++f) {
        int stopped=counters[i]>=20 || latches[l]!=0;
        int wanted=!stopped && k<4;
        D_800E27EC=counters[i]; D_8019AE5C=latches[l]; owner.kind=k;
        g_PlayerEntity=&owner; owner.transform=0; allocations=0; fail=f;
        output.position=&sentinel;
        assert(func_801939B4(1)==(stopped ? 2:0) && allocations==wanted);
        assert(D_8019AE5C==(wanted && !f ? 1:latches[l]));
        assert(output.position==(wanted && !f ? &transform.position:&sentinel));
    }
    for(l=0;l<3;++l) for(i=0;i<8;++i) {
        int enabled=latches[l]!=0;
        D_8019AE5C=latches[l]; D_800E11FA=i; D_800E2850[i]=65535-i;
        D_800F3368=D_800F336A=D_800F336C=D_800F336E=99;
        D_800F3370=D_800F3372=D_800F3374=D_800F3376=D_800F3378=99;
        assert(func_801939B4(2)==0);
        assert(D_800F3368==(enabled?32:99) && D_800F336A==(enabled?2:99));
        assert(D_800F336C==(enabled?3:99) && D_800F336E==(enabled?1:99));
        assert(D_800F3370==(enabled?65535-i:99));
        assert(D_800F3372==(enabled?0:99) && D_800F3374==(enabled?0:99));
        assert(D_800F3376==(enabled?32:99) && D_800F3378==(enabled?32:99));
        assert(D_8019AE5C==latches[l]);
    }
    allocations=0;
    assert(func_801939B4(-1)==0 && func_801939B4(3)==0);
    assert(func_801939B4(INT_MIN)==0 && func_801939B4(INT_MAX)==0);
    assert(allocations==0 && init==1);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'behavior.c'
            path.write_text(source)
            exe = path.with_suffix('')
            subprocess.run(['cc', '-std=c99', '-O2', str(path), '-o', str(exe)], check=True)
            subprocess.run([str(exe)], check=True)
