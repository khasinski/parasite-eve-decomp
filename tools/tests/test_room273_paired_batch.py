import pathlib
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m273/RoomEffect_PairedBatchEmitter.c'


class PairedBatch(unittest.TestCase):
    def test_target_layout(self):
        source = SOURCE.read_text() + r'''
typedef char layout[sizeof(Effect)==12 &&
    (unsigned long)&((Effect *)0)->size==6 &&
    (unsigned long)&((Effect *)0)->phase==8 &&
    (unsigned long)&((Effect *)0)->unknown10==10 &&
    (unsigned long)&((Context *)0)->pool==8 &&
    (unsigned long)&((Batch *)0)->y==16 &&
    (unsigned long)&((Batch *)0)->z==32 &&
    (unsigned long)&((Batch *)0)->count==48 &&
    (unsigned long)&((Batch *)0)->stopped==64 ? 1:-1];
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
        source = re.sub(r'\s+asm\("\$\d+"\)', '', source)
        # These linker symbols alias members of a single PSX allocation.
        source = source.replace('extern unsigned char D_8019AEF8;',
                                'static Batch storage;\n#define D_8019AEF8 storage.stopped')
        source = source.replace('extern short D_8019AEE8;',
                                '#define D_8019AEE8 storage.count')
        source = '#include <assert.h>\n#include <string.h>\n#include <limits.h>\n' + source + r'''
static Context context;
Context *D_800F33E0=&context;
static unsigned char primary[128],otherPrimary[128],secondary[128],otherSecondary[128];
void *D_8019AE7C=secondary;
unsigned short D_800E11EA,D_800E2850[8];
unsigned short D_800F3368,D_800F336A,D_800F336C,D_800F336E;
unsigned short D_800F3370,D_800F3372,D_800F3374;
volatile unsigned short D_800F3376,D_800F3378;
static Effect records[8],*links[8];
static int init,attempts,aux,updates,renders,failAt,mutateCount;
int func_80195984(void) { assert(0); return 0; }
int func_8019A290(void) { assert(0); return 0; }
int func_800CE560(void *p,int stride,int count,int (*callback)()) {
    assert(init++==0 && p==primary && stride==12 && count==10 && callback==func_80195984);
    return 120;
}
int func_800CE5AC(void **p,int offset,int stride,int count,int (*callback)()) {
    assert(init++==1 && p==&D_8019AE7C && offset==120 && stride==4 && count==10 && callback==func_8019A290);
    *p=secondary; return 40;
}
void *func_800CE610(void *p) {
    int n;
    if(p==secondary || p==otherSecondary) {
        n=aux++;
        assert(n<8 && attempts==n+1 && p==(n==0 ? secondary:otherSecondary));
        assert(records[n].x==0xA5A5 && records[n].phase==0xA5A5 && links[n]==0);
        storage.x[n]=65535-n; storage.y[n]=n; storage.z[n]=32768+n;
        if(mutateCount && n==0) storage.count=3;
        D_8019AE7C=otherSecondary;
        return &links[n];
    }
    n=attempts++;
    assert(n<8 && p==(n==0 ? primary:otherPrimary));
    context.pool=otherPrimary;
    return n==failAt ? 0:&records[n];
}
void func_800CE688(void *p) {
    assert(p==D_8019AE7C && updates++==0);
    if(!storage.stopped) assert(storage.count==0);
}
void func_800CE78C(void *p) {
    assert(p==D_8019AE7C && renders++==0);
    D_800E11EA=3; D_800E2850[3]=0xFEDC;
}
int main(void) {
    int counts[]={-32768,-1,0,1,3,8};
    int stops[]={0,1,255};
    int s,c,f,n,successes,active;
    context.pool=primary;
    assert(func_80195BD0(0)==160 && init==2 && D_8019AE7C==secondary);
    for(s=0;s<3;++s) for(c=0;c<6;++c) for(f=0;f<=8;++f) {
        storage.stopped=stops[s]; storage.count=counts[c];
        context.pool=primary; D_8019AE7C=secondary;
        attempts=aux=updates=renders=0; failAt=f;
        memset(records,0xA5,sizeof(records)); memset(links,0,sizeof(links));
        active=!stops[s] && counts[c]>0;
        successes=active ? (f<counts[c] ? f:counts[c]):0;
        assert(func_80195BD0(1)==(stops[s] ? 2:0));
        assert(attempts==successes+(active && f<counts[c]));
        assert(aux==successes && updates==1 && renders==0);
        assert(storage.count==(stops[s] ? counts[c]:0));
        assert(storage.stopped==stops[s]);
        for(n=0;n<8;++n) {
            assert(records[n].unknown10==0xA5A5);
            if(n<successes) {
                assert(records[n].x==65535-n && records[n].y==(unsigned short)(n-128));
                assert(records[n].z==32768+n && records[n].size==128 && records[n].phase==0);
                assert(links[n]==&records[n]);
            } else assert(records[n].x==0xA5A5 && records[n].phase==0xA5A5 && links[n]==0);
        }
    }
    storage.stopped=0; storage.count=1; mutateCount=1; failAt=8;
    context.pool=primary; D_8019AE7C=secondary;
    attempts=aux=updates=0;
    memset(records,0xA5,sizeof(records)); memset(links,0,sizeof(links));
    assert(func_80195BD0(1)==0 && attempts==3 && aux==3 && storage.count==0);
    for(s=0;s<3;++s) {
        storage.stopped=stops[s]; renders=0;
        D_800E11EA=0; D_800E2850[0]=0;
        assert(func_80195BD0(2)==0 && renders==1);
        assert(D_800F3368==32 && D_800F336A==2 && D_800F3376==32 && D_800F3378==32);
        assert(D_800F336C==3 && D_800F336E==0 && D_800F3372==0 && D_800F3374==0 && D_800F3370==0xFEDC);
    }
    attempts=updates=renders=0;
    assert(func_80195BD0(-1)==0 && func_80195BD0(3)==0);
    assert(func_80195BD0(INT_MIN)==0 && func_80195BD0(INT_MAX)==0);
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
