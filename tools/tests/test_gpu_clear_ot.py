import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class ClearOtTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_forward_links_last_entry_and_debug(self):
        source = (ROOT / "src/main/gpu/ClearOTag.c").read_text()
        self.assertEqual(source.count(' asm("$5")'), 1)
        source = source.replace(' asm("$5")', "")
        harness = source + r'''
#include <assert.h>
#include <stdint.h>
u8 g_GraphDebug;
char D_800118F8[] = "ot";
u32 D_800957F8, D_8009580C;
static u32 storage[130];
static int calls, expectedCount;
static void debug(char *message, void *ot, int count) {
    assert(message == D_800118F8 && ot == storage + 1 && count == expectedCount);
    assert(storage[1] == 0xA5ABCDEF);
    ++calls;
}
void (*g_GpuDebugPrintf[])(char *, void *, int) = {debug};
int main(void) {
    int level, count, i;
    for (level = 0; level <= 3; ++level) {
        g_GraphDebug = level;
        for (count = 1; count <= 128; ++count) {
            u32 *result;
            expectedCount = count;
            calls = 0;
            for (i = 0; i < 130; ++i) storage[i] = 0xA5ABCDEF;
            D_800957F8 = 0x12345678;
            D_8009580C = 0;
            result = ClearOTag(storage + 1, count);
            assert(result == storage + count);
            assert(calls == (level >= 2));
            assert(storage[0] == 0xA5ABCDEF && storage[count + 1] == 0xA5ABCDEF);
            for (i = 1; i < count; ++i)
                assert(storage[i] == ((uintptr_t)(storage + i + 1) & 0xFFFFFF));
            assert(storage[count] == ((uintptr_t)&D_8009580C & 0xFFFFFF));
            assert(D_8009580C == (0x04000000 | ((uintptr_t)&D_800957F8 & 0xFFFFFF)));
            assert(D_800957F8 == 0x12345678);
        }
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "clear-ot-test"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-Wno-pointer-to-int-cast",
                 "-Wno-int-to-pointer-cast", "-I", str(ROOT / "include"),
                 "-I", str(ROOT / "tools/m2c"), "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)


if __name__ == "__main__":
    unittest.main()
