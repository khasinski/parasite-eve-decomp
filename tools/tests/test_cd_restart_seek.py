import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class RestartSeekTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_order_parameters_and_result(self):
        source = (ROOT / "src/main/cdrom/CdRom_RestartSeek.c").read_text()
        self.assertEqual(source.count('asm("$3")'), 1)
        source = source.replace('asm("$3")', "")
        harness = source + r'''
#include <assert.h>
#include <limits.h>
int D_8009B6EC;
static CdlLOC first, second;
static int step, modeValue, commandValue, sectorValue, resultValue;
DsEventCallback DsSyncCallback(DsEventCallback callback) {
    assert(step++ == 0 && callback == 0);
    return 0;
}
CdlLOC *CdRom_GetCurrentPosPtr(void) {
    if (step == 1) { ++step; return &first; }
    assert(step++ == 4);
    return &second;
}
int CdPosToInt(CdlLOC *position) {
    assert(step++ == 2 && position == &first);
    return sectorValue;
}
int CdRom_GetCmdMode(void) {
    assert(step++ == 3 && D_8009B6EC == sectorValue);
    return modeValue;
}
int func_8007FC44(void) {
    assert(step++ == 5);
    return commandValue;
}
void func_8008227C(void) { assert(0); }
int Render_BuildParticleFrame(int mode, CdlLOC *position, int command,
                             void (*callback)(void), int limit) {
    assert(step++ == 6);
    assert(mode == (modeValue & 255) && position == &second);
    assert(command == (commandValue & 255));
    assert(callback == func_8008227C && limit == -1);
    return resultValue;
}
int main(void) {
    const int values[] = {INT_MIN, -257, -1, 0, 1, 255, 256, INT_MAX};
    int a, b, c;
    for (a = 0; a < 8; ++a) for (b = 0; b < 8; ++b)
    for (c = 0; c < 8; ++c) {
        modeValue = values[a]; commandValue = values[b];
        sectorValue = values[c]; resultValue = values[7-c];
        D_8009B6EC = ~sectorValue; step = 0;
        assert(CdRom_RestartSeek() == resultValue);
        assert(step == 7 && D_8009B6EC == sectorValue);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "restart-seek"
            result = subprocess.run(
                ["cc", "-include", str(ROOT / "tools/tests/host_psyq.h"), "-std=gnu11", "-O2", "-I", str(ROOT / "include"),
                 "-I", str(ROOT / "tools/m2c"), "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)


if __name__ == "__main__":
    unittest.main()
