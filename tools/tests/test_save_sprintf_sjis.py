"""Retail versus stock-compiled behavior for Save_SprintfSjis.

The harness links the shipped C with the stock cc.sh pipeline, then runs that
object and the retail bytes. The routine is a leaf, so the caller clobbers
caller-saved registers and HI/LO before entry. An independent memory model
checks the destination, the untouched argument block, and the terminating NUL.
"""
import pathlib
import random
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
ENTRY = 0x8004006C
EXIT = 0x80010000
ARGS = 0x800A1708
DST = 0x80100000
FMT = 0x80101000
POOL = 0x80102000
RETAIL_OFF = 0x3086C
RETAIL_END = 0x3086C + 0x1A4
DST_LEN = 256
FMT_LEN = 256
POOL_LEN = 512
ARG_BYTES = 32
REGIONS = [(DST, DST_LEN), (FMT, FMT_LEN), (ARGS, ARG_BYTES), (POOL, POOL_LEN)]


def physical(address):
    return address & 0x1FFFFFFF


def c_div(value, divisor):
    return int(value / divisor)


def c_mod(value, divisor):
    return value - c_div(value, divisor) * divisor


class SaveSprintfSjisTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("mipsel-none-elf-ld"), "MIPS binutils unavailable")
    @unittest.skipUnless((ROOT / "tools/scripts/cc.sh").is_file(), "stock compiler unavailable")
    @unittest.skipUnless((ROOT / "assets/USA/main.exe").is_file(), "retail image unavailable")
    def test_linked_object_matches_retail_behavior(self):
        try:
            from unicorn import Uc, UcError, UC_ARCH_MIPS, UC_MODE_LITTLE_ENDIAN, UC_MODE_MIPS32
            from unicorn.mips_const import (
                UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_A2, UC_MIPS_REG_A3,
                UC_MIPS_REG_GP, UC_MIPS_REG_HI, UC_MIPS_REG_LO, UC_MIPS_REG_PC,
                UC_MIPS_REG_RA, UC_MIPS_REG_S0, UC_MIPS_REG_S1, UC_MIPS_REG_S2,
                UC_MIPS_REG_S3, UC_MIPS_REG_S4, UC_MIPS_REG_S5, UC_MIPS_REG_S6,
                UC_MIPS_REG_S7, UC_MIPS_REG_FP, UC_MIPS_REG_SP, UC_MIPS_REG_T0,
                UC_MIPS_REG_T1, UC_MIPS_REG_T2, UC_MIPS_REG_T3, UC_MIPS_REG_T4,
                UC_MIPS_REG_T5, UC_MIPS_REG_T6, UC_MIPS_REG_T7, UC_MIPS_REG_T8,
                UC_MIPS_REG_T9, UC_MIPS_REG_V0, UC_MIPS_REG_V1,
            )
        except ImportError:
            self.skipTest("unicorn unavailable")

        saved_regs = [
            UC_MIPS_REG_S0, UC_MIPS_REG_S1, UC_MIPS_REG_S2, UC_MIPS_REG_S3,
            UC_MIPS_REG_S4, UC_MIPS_REG_S5, UC_MIPS_REG_S6, UC_MIPS_REG_S7,
            UC_MIPS_REG_FP,
        ]
        clobber_regs = [
            UC_MIPS_REG_A2, UC_MIPS_REG_A3, UC_MIPS_REG_V0, UC_MIPS_REG_V1,
            UC_MIPS_REG_T0, UC_MIPS_REG_T1, UC_MIPS_REG_T2, UC_MIPS_REG_T3,
            UC_MIPS_REG_T4, UC_MIPS_REG_T5, UC_MIPS_REG_T6, UC_MIPS_REG_T7,
            UC_MIPS_REG_T8, UC_MIPS_REG_T9,
        ]
        retail = (ROOT / "assets/USA/main.exe").read_bytes()[RETAIL_OFF:RETAIL_END]
        with tempfile.TemporaryDirectory() as directory:
            work = pathlib.Path(directory)
            obj, elf, data = work / "sjis.o", work / "sjis.elf", work / "sjis.bin"
            subprocess.run(
                ["tools/scripts/cc.sh", "src/main/save/Save_SprintfSjis.c", str(obj)],
                cwd=ROOT, check=True, capture_output=True,
            )
            (work / "sjis.ld").write_text(
                "D_800A1708 = 0x800A1708;\n"
                "_gp = 0x8009CD70;\n"
                "SECTIONS { .text 0x8004006C : SUBALIGN(4) { *(.text) } "
                "/DISCARD/ : { *(.reginfo) *(.mdebug) *(.pdr) } }\n"
            )
            subprocess.run(
                ["mipsel-none-elf-ld", "-EL", "-T", str(work / "sjis.ld"), str(obj), "-o", str(elf)],
                check=True, capture_output=True,
            )
            subprocess.run(
                ["mipsel-none-elf-objcopy", "-O", "binary", "-j", ".text", str(elf), str(data)],
                check=True, capture_output=True,
            )
            compiled = data.read_bytes()
        self.assertEqual(compiled, retail)

        def read_byte(memory, address):
            return memory[physical(address)]

        def write_byte(memory, address, value):
            memory[physical(address)] = value & 0xFF

        def read_int(memory, address):
            raw = int.from_bytes(memory[physical(address):physical(address) + 4], "little")
            return raw - 0x100000000 if raw & 0x80000000 else raw

        def write_int(memory, address, value):
            memory[physical(address):physical(address) + 4] = (value & 0xFFFFFFFF).to_bytes(4, "little")

        def emit_digit(memory, out, digit):
            code = (digit + 0x824F) & 0xFFFFFFFF
            write_byte(memory, out, code >> 8)
            write_byte(memory, out + 1, code)
            return out + 2

        def model(memory, dst, fmt):
            out = dst
            ch = read_byte(memory, fmt)
            args = ARGS
            while ch != 0:
                ch &= 0xFF
                fmt += 1
                if ch == ord("%"):
                    ch = read_byte(memory, fmt)
                    fmt += 1
                    if ch == ord("d"):
                        value = read_int(memory, args)
                        args += 4
                        out = emit_digit(memory, out, c_mod(c_div(value, 10), 10))
                        out = emit_digit(memory, out, c_mod(value, 10))
                    elif ch == ord("D"):
                        value = read_int(memory, args)
                        args += 4
                        out = emit_digit(memory, out, c_mod(value, 10))
                    elif ch == ord("s"):
                        text = read_int(memory, args) & 0xFFFFFFFF
                        args += 4
                        if text != 0:
                            byte = read_byte(memory, text)
                            if byte != 0:
                                while True:
                                    text = (text + 1) & 0xFFFFFFFF
                                    write_byte(memory, out, byte)
                                    out += 1
                                    byte = read_byte(memory, text)
                                    if byte == 0:
                                        break
                else:
                    write_byte(memory, out, read_byte(memory, fmt - 1))
                    out += 1
                ch = read_byte(memory, fmt)
            write_byte(memory, out, 0)

        def snapshot(memory):
            return tuple(bytes(memory[physical(address):physical(address) + size])
                         for address, size in REGIONS)

        def plant(memory, fmt, args, strings, dst=DST, fmt_at=FMT):
            memory[physical(DST):physical(DST) + DST_LEN] = b"\xA5" * DST_LEN
            memory[physical(FMT):physical(FMT) + FMT_LEN] = b"\x00" * FMT_LEN
            memory[physical(POOL):physical(POOL) + POOL_LEN] = b"\x00" * POOL_LEN
            raw = fmt.encode("latin1") if isinstance(fmt, str) else bytes(fmt)
            self.assertLess(len(raw), FMT_LEN)
            memory[physical(fmt_at):physical(fmt_at) + len(raw)] = raw
            if raw[-1:] != b"\x00":
                memory[physical(fmt_at) + len(raw)] = 0
            for index, value in enumerate(args):
                write_int(memory, ARGS + index * 4, value)
            for address, text in strings.items():
                blob = text.encode("latin1") if isinstance(text, str) else bytes(text)
                memory[physical(address):physical(address) + len(blob)] = blob
            if dst != DST:
                memory[physical(dst):physical(dst) + 64] = memory[physical(DST):physical(DST) + 64]

        cases = []
        numbers = [0, 1, 9, 10, 42, 99, 100, 255, -1, -9, -10, -15, 2147483647, -2147483648]
        cases.append(("", [], {}))
        cases.append(("ABC", [], {}))
        cases.append((" ", [], {}))
        cases.append(("\x82\x4F", [], {}))
        for number in numbers:
            cases.append(("%d", [number], {}))
            cases.append(("%D", [number], {}))
            cases.append(("n=%d.", [number], {}))
        cases.append(("%s", [0], {}))
        cases.append(("%s", [POOL], {POOL: "\x00"}))
        cases.append(("%s", [POOL], {POOL: "Hi\x00"}))
        cases.append(("%s", [POOL], {POOL: "\x82\x4F\x82\x50\x00"}))
        cases.append(("%%", [], {}))
        cases.append(("%x%d", [7], {}))
        cases.append(("%d%D%s", [42, 7, POOL], {POOL: "Z\x00"}))
        cases.append(("a%qb", [], {}))
        cases.append(("%\x00X", [], {}))
        cases.append(("%d%s%D", [8, 0, 3], {}))
        cases.append(("100%", [], {}))
        cases.append(("%d%d%d%d%d%d", [1, 2, 3, 4, 5, 6], {}))

        def consumed_specs(fmt):
            raw = fmt.encode("latin1") + b"\x00\x00"
            index = 0
            ch = raw[0]
            found = []
            while ch != 0:
                index += 1
                if ch == ord("%"):
                    spec = raw[index]
                    index += 1
                    if spec in (ord("d"), ord("D"), ord("s")):
                        found.append(spec)
                ch = raw[index]
            return found

        rng = random.Random(0x4006C)
        alphabet = ["A", " ", "%d", "%D", "%s", "%x", "%", "%%", "\x82"]
        for _ in range(768):
            fmt = "".join(rng.choice(alphabet) for _ in range(rng.randrange(0, 8)))
            args = []
            strings = {}
            cursor = POOL
            for spec in consumed_specs(fmt):
                if spec in (ord("d"), ord("D")):
                    args.append(rng.choice(numbers) if rng.randrange(4) == 0 else rng.randrange(-500, 500))
                else:
                    if rng.randrange(5) == 0:
                        args.append(0)
                    else:
                        length = rng.randrange(0, 12)
                        text = bytes(rng.randrange(1, 256) for _ in range(length)) + b"\x00"
                        strings[cursor] = text
                        args.append(cursor)
                        cursor += len(text)
            cases.append((fmt, args, strings))

        for case, (fmt, args, strings) in enumerate(cases):
            memory = bytearray(0x200000)
            plant(memory, fmt, args, strings)
            expected_mem = bytearray(memory)
            model(expected_mem, DST, FMT)
            expected = snapshot(expected_mem)
            saved = [rng.getrandbits(32) for _ in saved_regs]
            returns = []
            for name, body in (("retail", retail), ("C", compiled)):
                machine = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
                machine.mem_map(0, 0x200000)
                machine.mem_write(0, bytes(memory))
                machine.mem_write(physical(ENTRY), body)
                machine.reg_write(UC_MIPS_REG_SP, 0x801F0000)
                machine.reg_write(UC_MIPS_REG_RA, EXIT)
                machine.reg_write(UC_MIPS_REG_GP, 0x8009CD70)
                machine.reg_write(UC_MIPS_REG_A0, DST)
                machine.reg_write(UC_MIPS_REG_A1, FMT)
                for reg, value in zip(saved_regs, saved):
                    machine.reg_write(reg, value)
                # Leaf: no callee. The caller still leaves junk in the
                # caller-saved registers and in HI/LO.
                for reg in clobber_regs:
                    machine.reg_write(reg, 0xD00D0000 + reg)
                machine.reg_write(UC_MIPS_REG_HI, 0x12345678)
                machine.reg_write(UC_MIPS_REG_LO, 0x87654321)
                machine.reg_write(UC_MIPS_REG_V0, 0x51515151)
                try:
                    machine.emu_start(ENTRY, EXIT, count=20000)
                except UcError as error:
                    self.fail(f"case {case} {name} {fmt!r}: {error}")
                self.assertEqual(machine.reg_read(UC_MIPS_REG_PC), EXIT, (case, name, fmt))
                self.assertEqual(snapshot(machine.mem_read(0, 0x200000)), expected,
                                 (case, name, fmt, args))
                self.assertEqual(machine.reg_read(UC_MIPS_REG_SP), 0x801F0000, (case, name))
                self.assertEqual([machine.reg_read(reg) for reg in saved_regs], saved,
                                 (case, name, "saved"))
                returns.append(machine.reg_read(UC_MIPS_REG_V0))
                if fmt == "":
                    self.assertEqual(machine.reg_read(UC_MIPS_REG_V0), 0x51515151, (case, name))
            self.assertEqual(returns[0], returns[1], (case, fmt, "v0"))


if __name__ == "__main__":
    unittest.main()
