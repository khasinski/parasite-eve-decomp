import pathlib
import shutil
import subprocess
import tempfile
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[2]


class GpuGraphDebugTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_updates_debug_and_reports_type_and_reverse(self):
        source = (ROOT / "src/main/gpu/SetGraphDebug.c").read_text()
        for annotation in (' asm("$3")', ' asm("$2")'):
            self.assertEqual(source.count(annotation), 1)
            source = source.replace(annotation, "")
        harness = source + r'''
#include <assert.h>
GpuDebugState D_8009574C;
char D_80011814[] = "SetGraphDebug";
static int calls, expected_level, expected_type, expected_reverse;
static void print(char *format, int level, int type, int reverse) {
    assert(format == D_80011814 && level == expected_level);
    assert(type == expected_type && reverse == expected_reverse);
    ++calls;
}
void (*D_80095748)() = (void (*)())print;
static void check(int old, int level, int type, int reverse) {
    D_8009574C.queueState.debugLevel = old;
    D_8009574C.type = expected_type = type;
    D_8009574C.reverse = expected_reverse = reverse;
    expected_level = level;
    calls = 0;
    assert(SetGraphDebug(level) == old);
    assert(D_8009574C.queueState.debugLevel == (unsigned char)level);
    assert(calls == !!(unsigned char)level);
}
int main(void) {
    check(0, 0, 1, 0);
    check(7, 1, 3, 1);
    check(2, 255, 255, 42);
    check(9, 256, 4, 8);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "graph-debug-test"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-I", str(ROOT / "include"),
                 "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)


if __name__ == "__main__":
    unittest.main()
