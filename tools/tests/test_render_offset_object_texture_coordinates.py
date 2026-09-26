"""Retail-byte and packet-mutation checks for the texture offset routine."""
import pathlib
import random
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
ENTRY, EXIT = 0x8003E474, 0x80010000
HEADER, OBJECT, PACKETS, STACK = 0x80100000, 0x80101000, 0x80110000, 0x801F0000


@unittest.skipUnless(shutil.which("mipsel-none-elf-ld"), "MIPS binutils unavailable")
@unittest.skipUnless((ROOT / "assets/USA/main.exe").is_file(), "retail image unavailable")
class RenderOffsetObjectTextureCoordinatesTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.retail = (ROOT / "assets/USA/main.exe").read_bytes()[0x2EC74:0x2EDF0]
        with tempfile.TemporaryDirectory() as directory:
            work = pathlib.Path(directory)
            obj, elf, binary = (work / name for name in ("offset.o", "offset.elf", "offset.bin"))
            subprocess.run(["tools/scripts/cc.sh",
                            "src/main/render/Render_OffsetObjectTextureCoordinates.c", str(obj)],
                           cwd=ROOT, check=True, capture_output=True)
            script = work / "offset.ld"
            script.write_text(
                "SECTIONS { .text 0x8003E474 : SUBALIGN(4) { *(.text) } "
                "/DISCARD/ : { *(.reginfo) *(.mdebug) *(.pdr) } }\n")
            subprocess.run(["mipsel-none-elf-ld", "-EL", "-T", str(script), str(obj),
                            "-o", str(elf)], check=True, capture_output=True)
            subprocess.run(["mipsel-none-elf-objcopy", "-O", "binary", "-j", ".text",
                            str(elf), str(binary)], check=True, capture_output=True)
            cls.compiled = binary.read_bytes()

    def test_byte_match(self):
        self.assertEqual(self.compiled, self.retail)

    def test_packet_offsets_wrap_and_preserve_unmodified_fields(self):
        try:
            from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN
            from unicorn import mips_const as reg
        except ImportError:
            self.skipTest("unicorn unavailable")

        rng = random.Random(ENTRY)
        deltas = (-512, -256, -129, -128, -1, 0, 1, 127, 128, 255, 256, 511)
        clut_cases = (0, 1, 127, 128, 255, 256, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF)
        for case in range(512):
            nq, nt = case % 22, (case // 22) % 22
            du, dv = deltas[case % len(deltas)], deltas[(case // len(deltas)) % len(deltas)]
            clut = clut_cases[case % len(clut_cases)] if case < 256 else rng.getrandbits(32)
            clut_delta = (clut & 255) - (256 if clut & 128 else 0)
            original = rng.randbytes(4096)
            expected = bytearray(original)
            offset = 0
            for count, stride, us, vs in (
                    (nq, 0x34, (12, 24, 36), (13, 25, 37, 49)),
                    (nt, 0x28, (12, 24, 36), (13, 25, 37))):
                for _ in range(count * 2):
                    for field in us:
                        expected[offset + field] = (expected[offset + field] + du) & 255
                    for field in vs:
                        expected[offset + field] = (expected[offset + field] + dv) & 255
                    value = int.from_bytes(expected[offset + 14:offset + 16], "little")
                    expected[offset + 14:offset + 16] = ((value + clut_delta) & 65535).to_bytes(2, "little")
                    offset += stride

            for name, body in (("retail", self.retail), ("C", self.compiled)):
                with self.subTest(case=case, code=name):
                    machine = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
                    machine.mem_map(0, 0x200000)
                    def put(address, data):
                        machine.mem_write(address & 0x1FFFFFFF, bytes(data))
                    put(ENTRY, body)
                    put(HEADER + 8, nq.to_bytes(2, "little") + nt.to_bytes(2, "little"))
                    put(OBJECT, HEADER.to_bytes(4, "little"))
                    put(OBJECT + 0x54, PACKETS.to_bytes(4, "little"))
                    put(PACKETS, original)
                    for register, value in ((reg.UC_MIPS_REG_A0, OBJECT),
                                            (reg.UC_MIPS_REG_A1, du & 0xFFFFFFFF),
                                            (reg.UC_MIPS_REG_A2, dv & 0xFFFFFFFF),
                                            (reg.UC_MIPS_REG_A3, clut),
                                            (reg.UC_MIPS_REG_SP, STACK),
                                            (reg.UC_MIPS_REG_RA, EXIT)):
                        machine.reg_write(register, value)
                    for i in range(8):
                        machine.reg_write(getattr(reg, f"UC_MIPS_REG_S{i}"), 0xABCD0000 + i)
                    machine.emu_start(ENTRY, EXIT, count=20000)
                    self.assertEqual(machine.reg_read(reg.UC_MIPS_REG_PC), EXIT)
                    self.assertEqual(bytes(machine.mem_read(PACKETS & 0x1FFFFFFF, 4096)), bytes(expected))
                    self.assertEqual(machine.reg_read(reg.UC_MIPS_REG_SP), STACK)
                    for i in range(8):
                        self.assertEqual(machine.reg_read(getattr(reg, f"UC_MIPS_REG_S{i}")), 0xABCD0000 + i)


if __name__ == "__main__":
    unittest.main()
