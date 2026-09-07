import pathlib
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m273/RoomEffect_PeriodicDualPools.c'


class PeriodicDualPools(unittest.TestCase):
    def test_target_layout(self):
        source = SOURCE.read_text() + r'''
typedef char layout[sizeof(Vector)==8 &&
    (unsigned long)&((Context *)0)->pool==8 &&
    (unsigned long)&((StateContext *)0)->state==8 &&
    (unsigned long)&((State *)0)->kind==14 &&
    (unsigned long)&((State *)0)->timer==22 ? 1:-1];
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
        # The two addresses are aliases within the same PSX allocation.
        source = source.replace('extern unsigned char D_8019AEF8;', '')
        source = source.replace('extern Vector D_8019AE9C[];', r'''
static struct { Vector positions[2]; unsigned char unknown[76],stop; } storage;
#define D_8019AEF8 storage.stop
#define D_8019AE9C storage.positions
''')
        source = '#include <assert.h>\n#include <string.h>\n#include <limits.h>\n' + source + r'''
static Context context;
static StateContext stateContext;
static State state,replacement;
Context *D_800F33E0=&context;
StateContext *D_800F32D0=&stateContext;
static unsigned char primary[64],otherPrimary[64],secondary[64],otherSecondary[64];
void *D_8019AE80=secondary;
unsigned short D_800E11FA,D_800E2850[8];
unsigned short D_800F3368,D_800F336A,D_800F336C,D_800F336E;
unsigned short D_800F3370,D_800F3372,D_800F3374;
volatile unsigned short D_800F3376,D_800F3378;
static Vector *primaryLinks[2],*secondaryLinks[2];
static int init,attempts,aux,updates,renders,failPrimary,failSecondary;
int func_8019A398(void) { assert(0); return 0; }
int func_8019A4CC(void) { assert(0); return 0; }
int func_800CE560(void *p,int stride,int count,int (*callback)()) {
    assert(init++==0 && p==primary && stride==4 && count==6 && callback==func_8019A398);
    return 48;
}
int func_800CE5AC(void **p,int offset,int stride,int count,int (*callback)()) {
    assert(init++==1 && p==&D_8019AE80 && offset==48 && stride==4 && count==6 && callback==func_8019A4CC);
    *p=secondary; return 32;
}
void *func_800CE610(void *p) {
    int n;
    if(p==primary || p==otherPrimary) {
        n=attempts++;
        assert(n<2 && p==(n==0 ? primary:otherPrimary) && aux==0);
        context.pool=otherPrimary;
        stateContext.state=&replacement; state.timer=100; storage.stop=1;
        return n==failPrimary ? 0:&primaryLinks[n];
    }
    n=aux++;
    assert(n<2 && p==(n==0 ? secondary:otherSecondary));
    assert(attempts==(failPrimary<2 ? failPrimary+1:2));
    D_8019AE80=otherSecondary;
    return n==failSecondary ? 0:&secondaryLinks[n];
}
void func_800CE688(void *p) { assert(p==D_8019AE80 && updates++==0); }
void func_800CE78C(void *p) {
    assert(p==D_8019AE80 && renders++==0);
    D_800E11FA=3; D_800E2850[3]=0xFEDC;
}
int main(void) {
    int timers[]={-32768,-16,-8,-1,0,1,7,8,16,24,32,34,35,40,32767};
    int stops[]={0,1,255};
    int s,k,t,f,g,n,active,pSuccess,sSuccess;
    context.pool=primary;
    assert(func_801960F4(0)==80 && init==2 && D_8019AE80==secondary);
    for(s=0;s<3;++s) for(k=10;k<=12;++k) for(t=0;t<15;++t)
        for(f=0;f<=2;++f) for(g=0;g<=2;++g) {
            storage.stop=stops[s]; state.kind=k; state.timer=timers[t];
            stateContext.state=&state; context.pool=primary; D_8019AE80=secondary;
            attempts=aux=updates=renders=0; failPrimary=f; failSecondary=g;
            memset(primaryLinks,0,sizeof(primaryLinks)); memset(secondaryLinks,0,sizeof(secondaryLinks));
            active=!stops[s] && k==11 && timers[t]<35 && !(timers[t]&7);
            pSuccess=active ? f:0; sSuccess=active ? g:0;
            assert(func_801960F4(1)==(stops[s] ? 2:0));
            assert(attempts==(active ? (f<2 ? f+1:2):0));
            assert(aux==(active ? (g<2 ? g+1:2):0));
            assert(updates==(!stops[s] && k==11) && renders==0);
            for(n=0;n<2;++n) {
                assert(primaryLinks[n]==(n<pSuccess ? &storage.positions[n]:0));
                assert(secondaryLinks[n]==(n<sSuccess ? &storage.positions[n]:0));
            }
        }
    for(s=0;s<3;++s) {
        storage.stop=stops[s]; renders=0; D_800E11FA=0; D_800E2850[0]=0;
        assert(func_801960F4(2)==0 && renders==1);
        assert(D_800F3368==32 && D_800F336A==2 && D_800F3376==32 && D_800F3378==32);
        assert(D_800F336C==3 && D_800F336E==1 && D_800F3372==0 && D_800F3374==0 && D_800F3370==0xFEDC);
    }
    attempts=updates=renders=0;
    assert(func_801960F4(-1)==0 && func_801960F4(3)==0);
    assert(func_801960F4(INT_MIN)==0 && func_801960F4(INT_MAX)==0);
    assert(!attempts && !updates && !renders && init==2);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'behavior.c'
            path.write_text(source)
            exe = path.with_suffix('')
            subprocess.run(['cc', '-std=c99', '-O2', str(path), '-o', str(exe)], check=True)
            subprocess.run([str(exe)], check=True)
