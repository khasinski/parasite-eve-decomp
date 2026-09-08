import pathlib
import shutil
import subprocess
import tempfile
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[2]


class PadControlTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_start_and_stop_manage_the_pad_lifecycle(self):
        source = (ROOT / "src/main/psyq/libapi/pad_control.c").read_text()
        harness = source + r'''
#include <assert.h>
int g_InitPadFlag;
static int start2, clear, enable, disable, stop2, remove;
long StartPAD2(void) { ++start2; return 1; }
void ChangeClearPAD(int value) { assert(value == 0); ++clear; }
void EnablePAD(void) { ++enable; }
void DisablePAD(void) { ++disable; }
void StopPAD2(void) { ++stop2; }
int RemovePatchPad(void) { ++remove; return 1; }
int main(void) {
    g_InitPadFlag = 1;
    assert(StartPAD() == 1);
    assert(start2 == 1 && clear == 1 && enable == 1);
    StopPAD();
    assert(disable == 1 && stop2 == 1 && remove == 1 && g_InitPadFlag == 0);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "pad-control-test"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-I", str(ROOT / "include"), "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)


if __name__ == "__main__":
    unittest.main()
