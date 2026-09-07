import pathlib
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m273/RoomEffect_PairedEmitter.c'


class PairedEmitter(unittest.TestCase):
    def test_target_layout(self):
        source = SOURCE.read_text() + r'''
typedef char layout[sizeof(Entry)==8 &&
    (unsigned long)&((Entry *)0)->flags==6 &&
    (unsigned long)&((EmitterState *)0)->stopped==30 &&
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
        source = re.sub(r'asm\(""[^;]*;', '', SOURCE.read_text())
        source = '#include <assert.h>\n#include <string.h>\n' + source + r'''
void *D_800B0E64, *D_8019AF6C;
Context *D_800F33E0;
EmitterState D_8019AF84;
short D_800F3372,D_800F3374;
static Context context;
static int asset,pool,stage,initResult,allocs,failAt;
static Entry outputs[2];
void *Asset_FindTable08ByU32Key(void *owner,unsigned int id) {
    assert(stage++==0 && owner==&context && id==0xC5541704);
    return &asset;
}
void func_800C6D5C(void *handle,int a,int b) {
    assert(stage++==1 && handle==&asset && D_8019AF6C==&asset && !a && !b);
    context.pool=&pool;
}
int func_80198E94(int mode,void *p) { assert(0); return 0; }
int func_800CE560(void *p,int size,int count,int (*callback)(int,void *)) {
    assert(stage++==2 && p==&pool && size==8 && count==4);
    assert(callback==func_80198E94);
    return initResult;
}
Entry *func_800CE610(void *p) {
    int index=allocs++;
    assert(p==&pool && index<2);
    if(index==failAt) return 0;
    return &outputs[index];
}
int main(void) {
    int mask,fail,i,next,successful,expectedCalls,stopped;
    D_800B0E64=&context; D_800F33E0=&context;
    for(initResult=-1;initResult<=1;++initResult) {
        stage=0; context.pool=0;
        assert(func_801993F0(0)==initResult && stage==3);
    }
    for(mask=0;mask<4;++mask) for(fail=-1;fail<2;++fail) {
        memset(&D_8019AF84,0xA6,sizeof(D_8019AF84));
        memset(outputs,0x55,sizeof(outputs));
        D_8019AF84.stopped=0;
        for(i=0;i<2;++i) {
            D_8019AF84.entries[i].x=65535-i;
            D_8019AF84.entries[i].y=32768+i;
            D_8019AF84.entries[i].z=123+i;
            D_8019AF84.entries[i].flags=0xA5A0 | ((mask>>i)&1);
        }
        allocs=0; failAt=fail;
        assert(func_801993F0(1)==0);
        next=successful=expectedCalls=stopped=0;
        for(i=0;i<2;++i) {
            int active=(mask>>i)&1;
            if(active && !stopped) {
                ++expectedCalls;
                if(next==fail) stopped=1;
                else {
                    Entry *out=&outputs[next];
                    assert(out->x==65535-i && out->y==32768+i && out->z==123+i);
                    assert(out->flags==0);
                    ++successful;
                }
                ++next;
            }
            assert(D_8019AF84.entries[i].flags==
                   (unsigned short)(0xA5A0 | (active && stopped)));
        }
        assert(allocs==expectedCalls);
        for(i=successful;i<2;++i) assert(outputs[i].flags==0x5555);
        for(i=0;i<14;++i) assert(D_8019AF84.unknown[i]==0xA6);
        D_8019AF84.stopped=255;
        assert(func_801993F0(1)==2 && allocs==expectedCalls);
    }
    D_800F3372=-1; D_800F3374=123;
    assert(func_801993F0(2)==0 && !D_800F3372 && !D_800F3374);
    assert(func_801993F0(-1)==0 && func_801993F0(3)==0);
    assert(stage==3);
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
