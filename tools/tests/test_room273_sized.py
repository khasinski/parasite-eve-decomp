import pathlib
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m273/RoomEffect_SizedTableSprite.c'


class SizedSprite(unittest.TestCase):
    def test_target_layout(self):
        source = SOURCE.read_text() + r'''
typedef char layout[sizeof(Vector)==8 &&
    (unsigned long)&((Vector *)0)->z==4 ? 1:-1];
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
        source = re.sub(r' asm\("\$\d+"\)', '', SOURCE.read_text())
        source = re.sub(r'asm\(""[^;]*;', '', source)
        source = '#include <assert.h>\n#include <limits.h>\n' + source + r'''
int D_800E27EC, D_800F3428, D_800966EC[4096];
unsigned short D_800F336C, D_800E1204[8];
static Vector position={12,34,56,78};
static int cluts,draws,expectedPalette,expectedSize,shadeIndex;
unsigned short GetClut(int x, int y) {
    assert(cluts++==0 && !draws && x==0 && y==expectedPalette);
    D_800E27EC=999;
    D_800966EC[shadeIndex]=0x1234FFE0;
    return 0xF123;
}
void func_800CEE20(Vector *p, void *rotation, int w, int h, int tile,
                  int clut, int flag, int shade, void *color) {
    assert(cluts==1 && draws++==0 && p==&position && rotation==0);
    assert(w==expectedSize && h==expectedSize && tile==102);
    assert(clut==0xF123 && flag==1 && shade==-1 && color==0);
}
int main(void) {
    int times[]={INT_MIN,-1,0,7,8,INT_MAX};
    int words[]={0,0x12347FFF,-2049,0x1234FFFF};
    int i,j,kind,variant,frame,sizeIndex;
    for(i=0;i<6;++i) {
        D_800E27EC=times[i];
        assert(func_80197230(1,&position)==(times[i]>=8));
    }
    assert(!cluts && !draws);
    for(kind=0;kind<8;++kind) for(variant=0;variant<2;++variant)
        for(i=0;i<=8;++i) for(j=0;j<4;++j) {
            D_800E27EC=i; frame=i-1;
            sizeIndex=(((unsigned int)frame<<9)&0x3E00)/4;
            shadeIndex=(((unsigned int)frame<<10)&0x3C00)/4;
            D_800966EC[sizeIndex]=words[j];
            expectedSize=(short)(words[j]+2048);
            D_800F336C=kind; D_800F3428=variant;
            D_800E1204[kind]=65535;
            expectedPalette=65535+(kind==4 && variant ? 9:5);
            cluts=draws=0;
            assert(func_80197230(2,&position)==0 && cluts==1 && draws==1);
        }
    assert(func_80197230(0,&position)==0);
    assert(func_80197230(-1,&position)==0);
    assert(func_80197230(INT_MAX,&position)==0);
    assert(cluts==1 && draws==1);
    assert(position.x==12 && position.y==34 && position.z==56 && position.pad==78);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'behavior.c'
            path.write_text(source)
            exe = path.with_suffix('')
            subprocess.run(['cc', '-std=c99', '-O2', str(path), '-o', str(exe)], check=True)
            subprocess.run([str(exe)], check=True)
