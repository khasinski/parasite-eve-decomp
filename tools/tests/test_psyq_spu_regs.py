import pathlib
import shutil
import subprocess
import tempfile
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[2]


class PsyqSpuRegisterMapTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_transfer_register_offsets_match_psyq_spu_rxx(self):
        source = r'''
#include <stddef.h>
#include "pe1/psyq_spu_internal.h"
typedef char trans_addr_at_1a6[(offsetof(SpuRegs, trans_addr) == 0x1A6) ? 1 : -1];
typedef char trans_fifo_at_1a8[(offsetof(SpuRegs, transfer_fifo) == 0x1A8) ? 1 : -1];
typedef char spucnt_at_1aa[(offsetof(SpuRegs, spucnt) == 0x1AA) ? 1 : -1];
typedef char status_at_1ae[(offsetof(SpuRegs, transfer_status) == 0x1AE) ? 1 : -1];
int main(void) { return 0; }
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "psyq-spu-regs-test"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-I", str(ROOT / "include"), "-x", "c", "-", "-o", str(exe)],
                input=source, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)


if __name__ == "__main__":
    unittest.main()
