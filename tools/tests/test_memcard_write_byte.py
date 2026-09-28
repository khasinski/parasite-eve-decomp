"""MemCard_WriteByte: retail bytes and independent SIO/timeout behavior."""
from importlib.util import find_spec
from pathlib import Path
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]
BASE, SIZE = 0x800832B4, 564
TIMER, ELAPSED = 0x80084FC4, 0x80084FE4
STOP, STACK = 0x80010000, 0x801F0000
OBJ, RESPONSE, SIO, IRQ = 0x80104000, 0x80104100, 0x80103000, 0x80102000


class MemCardWriteByteTests(unittest.TestCase):
    @unittest.skipUnless(
        (ROOT / "assets/USA/main.exe").is_file()
        and (ROOT / "build/USA/main.exe").is_file()
        and find_spec("unicorn"), "images or Unicorn unavailable"
    )
    def test_exact_bytes_and_protocol_paths(self):
        from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
        from unicorn import mips_const as R

        offset = BASE - 0x8000F800
        retail = (ROOT / "assets/USA/main.exe").read_bytes()[offset:offset + SIZE]
        built = (ROOT / "build/USA/main.exe").read_bytes()[offset:offset + SIZE]
        self.assertEqual(len(built), SIZE)
        self.assertEqual(built, retail)

        # IRQ high skips timeout polling. Both timeout branches and a counter
        # wrap are exercised when IRQ is low. The last case checks the optional
        # 60-tick delay and the response-index wrap guard.
        cases = (
            dict(head=0x10, index=0, data=0x82, sent=0x42, irq=0x80,
                 field_e8=8, dispatch=0, baud=0x22, result=0x82, timers=[400]),
            dict(head=0x80, index=9, data=0x31, sent=0x7A, irq=0x80,
                 field_e8=8, dispatch=0, baud=0x22, result=0x31, timers=[400]),
            dict(head=0x10, index=255, data=0x31, sent=0x123, irq=0x80,
                 field_e8=0, dispatch=2, baud=0x88, result=0x31, timers=[400, 60]),
            dict(head=0x10, index=1, data=0x7F, sent=0x22, irq=0,
                 field_e8=8, dispatch=0, start=100, count=112, limit=10,
                 mode=0x200, target=0, result=-2, timers=[400]),
            dict(head=0x10, index=1, data=0x7F, sent=0x22, irq=0,
                 field_e8=8, dispatch=0, start=100, count=180, limit=10,
                 mode=0, target=0, result=-2, timers=[400]),
            dict(head=0x10, index=1, data=0x7F, sent=0x22, irq=0,
                 field_e8=8, dispatch=0, start=65530, count=20, limit=2,
                 mode=0, target=0, result=-2, timers=[400]),
        )

        saved = [getattr(R, f"UC_MIPS_REG_S{i}") for i in range(8)] + [R.UC_MIPS_REG_FP]
        for label, code in (("retail", retail), ("built", built)):
            for case in cases:
                with self.subTest(image=label, case=case):
                    uc = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
                    uc.mem_map(0, 0x200000)
                    uc.mem_map(0x1F800000, 0x10000)

                    def put(address, data):
                        uc.mem_write(address & 0x1FFFFFFF, bytes(data))

                    def get(address, size=4):
                        return int.from_bytes(uc.mem_read(address & 0x1FFFFFFF, size), "little")

                    def set_(address, value, size=4):
                        put(address, (value & ((1 << (size * 8)) - 1)).to_bytes(size, "little"))

                    put(BASE, code)
                    for address in (TIMER, ELAPSED):
                        put(address, struct.pack("<II", 0x03E00008, 0))
                    put(OBJ, b"\xA5" * 0xF0)
                    put(RESPONSE, b"\x5A" * 0x120)
                    put(SIO, b"\x69" * 32)
                    put(IRQ, b"\x3C" * 32)
                    put(STACK - 64, b"\xC3" * 96)
                    set_(0x8009B788, SIO)
                    set_(0x8009B784, IRQ)
                    set_(0x8009B768, case["dispatch"])
                    set_(0x800A76D0, case.get("start", 0))
                    set_(0x800BD02C, case.get("limit", 10))
                    set_(0x1F801120, case.get("count", 0), 2)
                    set_(0x1F801124, case.get("mode", 0), 2)
                    set_(0x1F801128, case.get("target", 0), 2)
                    set_(IRQ, case["irq"])
                    set_(SIO, case["data"], 1)
                    set_(SIO + 4, 2, 2)
                    set_(SIO + 14, 0x9999, 2)
                    set_(OBJ + 0x3C, RESPONSE)
                    set_(OBJ + 0x44, case["index"], 1)
                    set_(OBJ + 0x45, 7, 1)
                    set_(OBJ + 0xE8, case["field_e8"], 1)
                    set_(RESPONSE, case["head"], 1)

                    calls = []
                    elapsed_count = 0

                    def hook(machine, address, _size, _data):
                        nonlocal elapsed_count
                        if address == TIMER:
                            calls.append(machine.reg_read(R.UC_MIPS_REG_A0))
                        elif address == ELAPSED:
                            elapsed_count += 1
                            machine.reg_write(R.UC_MIPS_REG_V0, int(elapsed_count >= 2))

                    uc.hook_add(UC_HOOK_CODE, hook)
                    for reg, value in ((R.UC_MIPS_REG_A0, OBJ),
                                       (R.UC_MIPS_REG_A1, case["sent"]),
                                       (R.UC_MIPS_REG_SP, STACK),
                                       (R.UC_MIPS_REG_GP, 0x8009CD70),
                                       (R.UC_MIPS_REG_RA, STOP)):
                        uc.reg_write(reg, value)
                    for i, reg in enumerate(saved):
                        uc.reg_write(reg, 0xABCD0000 + i)
                    before = bytes(uc.mem_read(OBJ & 0x1FFFFFFF, 0xF0))
                    uc.emu_start(BASE, STOP, count=10000)

                    self.assertEqual(uc.reg_read(R.UC_MIPS_REG_V0), case["result"] & 0xFFFFFFFF)
                    self.assertEqual(calls, case["timers"])
                    self.assertEqual(uc.reg_read(R.UC_MIPS_REG_SP), STACK)
                    for i, reg in enumerate(saved):
                        self.assertEqual(uc.reg_read(reg), 0xABCD0000 + i)
                    if case["result"] == -2:
                        self.assertEqual(bytes(uc.mem_read(OBJ & 0x1FFFFFFF, 0xF0)), before)
                        self.assertEqual(get(SIO, 1), case["data"])
                    else:
                        self.assertEqual(get(SIO, 1), case["sent"] & 0xFF)
                        self.assertEqual(get(SIO + 14, 2), case["baud"])
                        self.assertEqual(get(OBJ + 0x45, 1), 8)
                        self.assertEqual(get(OBJ + 0x44, 1), (case["index"] + 1) & 0xFF)
                        if case["index"] != 255:
                            self.assertEqual(get(RESPONSE + case["index"], 1), case["data"])
                        else:
                            self.assertEqual(get(RESPONSE + 255, 1), 0x5A)


if __name__ == "__main__":
    unittest.main()
