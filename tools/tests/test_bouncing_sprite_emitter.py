"""Bouncing-sprite emitter: byte match and independent mode/state model."""
from importlib.util import find_spec
from pathlib import Path
import itertools
import struct
import unittest

ROOT = Path(__file__).resolve().parents[2]


class BouncingSpriteEmitterTests(unittest.TestCase):
    def test_plain_source(self):
        source = (ROOT / "src/main/engine/FieldEng_BouncingSpriteEmitter.c").read_text()
        self.assertNotRegex(source, r'\b(?:INCLUDE_ASM|REGALLOC_BARRIER)\b')
        self.assertNotRegex(source, r'asm\s+volatile\s*\(\s*"[^\"]+')

    @unittest.skipUnless(
        (ROOT / "assets/USA/main.exe").is_file()
        and (ROOT / "build/USA/main.exe").is_file()
        and find_spec("unicorn"), "images or Unicorn unavailable"
    )
    def test_behavior(self):
        from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_LITTLE_ENDIAN, UC_HOOK_CODE
        from unicorn import mips_const as R

        base, size = 0x800D6E3C, 644
        stop, stack, gp = 0x80010000, 0x801F0000, 0x8009CD70
        images = [(ROOT / path).read_bytes()[base - 0x8000F800:base - 0x8000F800 + size]
                  for path in ("assets/USA/main.exe", "build/USA/main.exe")]
        self.assertEqual(len(images[1]), size)
        self.assertEqual(images[1], images[0])

        symbols = {
            "rand": 0x80071A54,
            "rsin": 0x80077D34,
            "rcos": 0x80077DC4,
            "init": 0x800CE560,
            "alloc": 0x800CE610,
            "display": 0x800CEDA8,
            "callback": 0x800D6C58,
        }
        # Resolve actual SDK math entry addresses from the linked image.
        import subprocess
        for line in subprocess.check_output(
            ["mipsel-none-elf-nm", str(ROOT / "build/USA/main.elf")], text=True
        ).splitlines():
            fields = line.split()
            if len(fields) == 3 and fields[2] in ("rand", "rsin", "rcos"):
                symbols[fields[2]] = int(fields[0], 16)

        state, particle, channel, slot = 0x80100020, 0x80100120, 0x800F33E0, 0x80100220
        owner_global, owner, actor = 0x800F32D0, 0x80100320, 0x80101000
        time, params, page_index, pages = 0x800E27EC, 0x800F3368, 0x800E11E4, 0x800E2850
        regions = ((state - 8, 20), (particle - 8, 3 * 32 + 16),
                   (channel - 8, 20), (slot - 8, 28),
                   (owner_global - 8, 20), (owner - 8, 28),
                   (actor + 0x260, 32), (time - 8, 20),
                   (params - 8, 36), (page_index - 8, 20), (pages - 8, 32))
        saved = [getattr(R, f"UC_MIPS_REG_S{i}") for i in range(8)] + [R.UC_MIPS_REG_FP]

        def signed(value, bits):
            value &= (1 << bits) - 1
            return value - (1 << bits) if value & (1 << (bits - 1)) else value

        def trig(kind, angle):
            values = (-4096, -1025, 0, 2048, 4096, 3071, -17, 111)
            return values[((angle >> 9) + (3 if kind == "rcos" else 0)) & 7]

        cases = list(itertools.product((-1, 0, 1, 2, 3), (-1, 0, 7, 8, 39, 40),
                                       (0, 0x7FFFFFFF, 0x80000000), (0, 1, 2)))
        hooks = {symbols[name]: name for name in ("rand", "rsin", "rcos", "init", "alloc", "display")}
        for label, body in (("retail", images[0]), ("candidate", images[1])):
            machine = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
            machine.mem_map(0, 0x200000)

            def put(address, data):
                machine.mem_write(address & 0x1FFFFFFF, bytes(data))

            def read(address, length):
                return bytes(machine.mem_read(address & 0x1FFFFFFF, length))

            def get(address, length=4):
                return int.from_bytes(read(address, length), "little")

            def set_(address, value, length=4):
                put(address, (value & ((1 << (8 * length)) - 1)).to_bytes(length, "little"))

            put(base, body)
            for address in hooks:
                put(address, struct.pack("<II", 0x03E00008, 0))

            def hook(uc, address, _size, _data):
                if address not in hooks:
                    return
                kind = hooks[address]
                argc = 4 if kind == "init" else 1 if kind != "rand" else 0
                args = tuple(uc.reg_read(getattr(R, f"UC_MIPS_REG_A{i}")) for i in range(argc))
                result = callback(kind, args, get, set_, actual_events, counters, case)
                for name in ("V1", "A0", "A1", "A2", "A3", "T0", "T1", "T2", "T3", "T4", "T5", "T6", "T7", "T8", "T9"):
                    uc.reg_write(getattr(R, "UC_MIPS_REG_" + name), 0xCCCCCCCC)
                uc.reg_write(R.UC_MIPS_REG_V0, result & 0xFFFFFFFF)

            def callback(kind, args, get_, set_value, events, indexes, current):
                events.append((kind, args))
                if kind == "rand":
                    index = indexes["rand"]
                    indexes["rand"] += 1
                    return (0x12345678 + 0x173 * index + current[2]) & 0x7FFF
                if kind in ("rsin", "rcos"):
                    return trig(kind, args[0])
                if kind == "init":
                    set_value(state, 0xD00D0000 | current[3])
                    return 0x12340000 | current[3]
                if kind == "alloc":
                    index = indexes["alloc"]
                    indexes["alloc"] += 1
                    return 0 if current[3] == 1 and index == 1 else particle + 32 * index
                if kind == "display":
                    for offset in (6, 10, 12):
                        set_value(params + offset, 0x7000 + offset, 2)
                    return 0
                raise AssertionError(kind)

            machine.hook_add(UC_HOOK_CODE, hook)
            for case in cases:
                mode, clock, angle, mutation = case
                initial = {address: 0xA5 for start, length in regions
                           for address in range(start, start + length)}

                def model_get(address, length=4):
                    return int.from_bytes(bytes(initial[address + i] for i in range(length)), "little")

                def model_set(address, value, length=4):
                    for i, byte in enumerate((value & ((1 << (8 * length)) - 1)).to_bytes(length, "little")):
                        initial[address + i] = byte

                model_set(state, angle)
                model_set(time, clock)
                model_set(channel, slot)
                model_set(slot + 8, 0x80102000)
                model_set(owner_global, owner)
                model_set(owner + 8, actor)
                for offset, value in ((0x268, -32768), (0x26A, 1234), (0x26C, 32767)):
                    model_set(actor + offset, value, 2)
                model_set(page_index, mutation + 1, 2)
                for i in range(8):
                    model_set(pages + i * 2, 0x1234 + 0x311 * i, 2)
                for start, length in regions:
                    put(start, bytes(initial[address] for address in range(start, start + length)))
                expected_events, actual_events = [], []
                expected_indexes = {"rand": 0, "alloc": 0}
                counters = {"rand": 0, "alloc": 0}

                def call(kind, *args):
                    return callback(kind, tuple(args), model_get, model_set,
                                    expected_events, expected_indexes, case)

                expected_return = 0
                if mode == 0:
                    model_set(state, call("rand"))
                    expected_return = call("init", model_get(model_get(channel) + 8),
                                           16, 24, symbols["callback"])
                elif mode == 1:
                    if signed(model_get(time), 32) < 8:
                        for _ in range(3):
                            ptr = call("alloc", model_get(model_get(channel) + 8))
                            if ptr:
                                for offset, actor_offset in ((0, 0x268), (2, 0x26A), (4, 0x26C)):
                                    model_set(ptr + offset, model_get(actor + actor_offset, 2), 2)
                                new_angle = (model_get(state) + 0x955 + (call("rand") & 31)) & 0xFFFFFFFF
                                model_set(state, new_angle)
                                amplitude = (call("rand") & 127) + 70
                                product = signed(call("rsin", new_angle), 32) * amplitude
                                model_set(ptr + 6, (product + (4095 if product < 0 else 0)) >> 12, 2)
                                product = signed(call("rcos", new_angle), 32) * amplitude
                                model_set(ptr + 10, (product + (4095 if product < 0 else 0)) >> 12, 2)
                                model_set(ptr + 8, -(call("rand") & 31) - 36, 2)
                                model_set(ptr + 12, (call("rand") & 7) + 26, 2)
                                model_set(ptr + 14, call("rand"), 2)
                    expected_return = int(signed(model_get(time), 32) >= 40)
                elif mode == 2:
                    for offset, value in ((0, 16), (2, 1), (14, 16), (16, 16)):
                        model_set(params + offset, value, 2)
                    page = model_get(pages + 2 * model_get(page_index, 2), 2)
                    model_set(params + 4, 0, 2)
                    model_set(params + 8, page, 2)
                    call("display", 0)
                    for offset in (6, 10, 12):
                        model_set(params + offset, 0, 2)

                for name, value in (("A0", mode), ("A1", state), ("SP", stack),
                                    ("GP", gp), ("RA", stop)):
                    machine.reg_write(getattr(R, "UC_MIPS_REG_" + name), value & 0xFFFFFFFF)
                for index, reg in enumerate(saved):
                    machine.reg_write(reg, 0xABCD0000 + index)
                machine.emu_start(base, stop, count=5000)
                self.assertEqual(actual_events, expected_events, (label, case))
                for start, length in regions:
                    self.assertEqual(read(start, length),
                                     bytes(initial[address] for address in range(start, start + length)),
                                     (label, case, hex(start)))
                self.assertEqual(machine.reg_read(R.UC_MIPS_REG_V0), expected_return,
                                 (label, case))
                for name, value in (("PC", stop), ("SP", stack), ("GP", gp)):
                    self.assertEqual(machine.reg_read(getattr(R, "UC_MIPS_REG_" + name)), value,
                                     (label, case, name))
                for index, reg in enumerate(saved):
                    self.assertEqual(machine.reg_read(reg), 0xABCD0000 + index,
                                     (label, case, index))


if __name__ == "__main__":
    unittest.main()
