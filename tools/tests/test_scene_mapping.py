import hashlib
import pathlib
import unittest

import yaml

from tools.scripts import source_quality


ROOT = pathlib.Path(__file__).resolve().parents[2]


class SceneE19MappingTests(unittest.TestCase):
    def setUp(self):
        self.config = yaml.safe_load(
            (ROOT / "configs/USA/overlays/scene_e19.yaml").read_text())
        self.segment = self.config["segments"][0]
        self.rows = {row[0]: row for row in self.segment["subsegments"]}

    def test_header_is_loaded_before_directory_buffer(self):
        self.assertEqual(self.segment["vram"], 0x8018EFF0 - 8)
        self.assertEqual(self.config["sha1"],
                         "8f20a25d31be101d10ac79447efad8e39469ec7c")

    def test_epilogues_are_not_separate_functions(self):
        for offset in (0xC2C, 0x19B0, 0x2554, 0x3144, 0x3578, 0x3B20):
            with self.subTest(offset=hex(offset)):
                self.assertNotIn(offset, self.rows)

    def test_dispatchers_and_notify_functions_have_real_entries(self):
        for offset, name in (
            (0x5BC, "RoomLib_StateDispatchVariant2_8018F5A4"),
            (0x12FC, "RoomLib_StateDispatchVariant2_801902E4"),
            (0xC34, "RoomLib_FxNotify"),
            (0x19B8, "RoomLib_FxNotify2"),
        ):
            with self.subTest(name=name):
                self.assertEqual(self.rows[offset], [offset, "c", name])
                self.assertTrue(
                    (ROOT / "src/overlays/scene_e19" / (name + ".c")).is_file())

    def test_handlers_have_complete_semantic_c_ranges(self):
        rows = self.segment["subsegments"]
        ends = {row[0]: following[0] for row, following in zip(rows, rows[1:])}
        for offset, size, name in (
            (0x6EC, 1352, "RoomLib_HandlerD"),
            (0xF54, 156, "RoomLib_ResetSignalWithTargetGate"),
            (0x142C, 1420, "RoomLib_HandlerE"),
            (0x2940, 1148, "RoomLib_HandlerC"),
        ):
            with self.subTest(name=name):
                self.assertEqual(self.rows[offset], [offset, "c", name])
                self.assertEqual(ends[offset] - offset, size)
                path = ROOT / "src/overlays/scene_e19" / (name + ".c")
                self.assertEqual(source_quality.classify(path), "semantic_c")

    def test_retail_epilogues_restore_the_preceding_stack_frame(self):
        path = ROOT / self.config["options"]["target_path"]
        if not path.is_file():
            self.skipTest("local retail overlay required")
        data = path.read_bytes()
        self.assertEqual(hashlib.sha1(data).hexdigest(), self.config["sha1"])
        for offset in (0xC2C, 0x19B0, 0x2554, 0x3144, 0x3578, 0x3B20):
            with self.subTest(offset=hex(offset)):
                restore = int.from_bytes(data[offset - 4:offset], "little")
                self.assertEqual(restore >> 16, 0x27BD)  # addiu sp, sp, frame
                self.assertGreater(restore & 0xFFFF, 0)
                self.assertLess(restore & 0xFFFF, 0x8000)
                self.assertEqual(data[offset:offset + 8],
                                 bytes.fromhex("0800e00300000000"))


class SceneE02MappingTests(unittest.TestCase):
    def setUp(self):
        self.config = yaml.safe_load(
            (ROOT / "configs/USA/overlays/scene_e02.yaml").read_text())
        self.segment = self.config["segments"][0]
        self.rows = self.segment["subsegments"]

    def test_mapping_and_named_source_entries(self):
        self.assertEqual(self.segment["vram"], 0x8018EFE8)
        for row in self.rows:
            if row[1] != "c":
                continue
            name = row[2]
            suffix = name.rsplit("_", 1)[-1]
            if len(suffix) == 8 and all(c in "0123456789ABCDEF" for c in suffix):
                with self.subTest(name=name):
                    self.assertEqual(int(suffix, 16), 0x8018EFE8 + row[0])

    def test_six_new_functions_own_their_complete_ranges(self):
        by_offset = {r[0]: (r, n[0]) for r, n in zip(self.rows, self.rows[1:])}
        for offset, size, name in (
            (0x1A3C, 136, "RoomLib_StateDispatchVariant2_80190A24"),
            (0x1B6C, 1352, "RoomLib_HandlerD"),
            (0x23D4, 156, "RoomLib_ResetSignalWithTargetGate"),
            (0x277C, 136, "RoomLib_StateDispatchVariant2_80191764"),
            (0x28AC, 1420, "RoomLib_HandlerE"),
            (0x3DC0, 1148, "RoomLib_HandlerC"),
        ):
            with self.subTest(name=name):
                row, end = by_offset[offset]
                self.assertEqual(row, [offset, "c", name])
                self.assertEqual(end - offset, size)
                self.assertEqual(source_quality.classify(
                    ROOT / "src/overlays/scene_e02" / (name + ".c")), "semantic_c")

    def test_false_epilogues_are_merged(self):
        offsets = {r[0] for r in self.rows}
        for offset in (0x370, 0x20AC, 0x2E30, 0x39D4, 0x45C4, 0x49F8):
            self.assertNotIn(offset, offsets)

    def test_retail_epilogue_evidence(self):
        path = ROOT / self.config["options"]["target_path"]
        if not path.is_file():
            self.skipTest("local retail overlay required")
        data = path.read_bytes()
        self.assertEqual(hashlib.sha1(data).hexdigest(),
                         "2db8e96a07ced98b54cb73db9c9812b75abb0b52")
        for offset in (0x370, 0x20AC, 0x2E30, 0x39D4, 0x45C4, 0x49F8):
            with self.subTest(offset=hex(offset)):
                restore = int.from_bytes(data[offset - 4:offset], "little")
                self.assertEqual(restore >> 16, 0x27BD)
                self.assertTrue(0 < restore & 0xFFFF < 0x8000)
                self.assertEqual(data[offset:offset + 8],
                                 bytes.fromhex("0800e00300000000"))


class SceneE09E10MappingTests(unittest.TestCase):
    hashes = {
        "scene_e09": "5bba719fc9e394d1df4d1e1de54646d150b84687",
        "scene_e10": "b1195c5d873cfdc45c0a7ea2e1c8b14e3cf1fd9b",
    }
    epilogues = (0x39C, 0x2604, 0x3388, 0x3F2C, 0x4B1C, 0x4F50)

    def test_complete_c_ranges_and_real_entry_names(self):
        for scene, digest in self.hashes.items():
            config = yaml.safe_load(
                (ROOT / "configs/USA/overlays" / (scene + ".yaml")).read_text())
            segment = config["segments"][0]
            self.assertEqual(segment["vram"], 0x8018EFE8)
            self.assertEqual(config["sha1"], digest)
            rows = segment["subsegments"]
            by_offset = {r[0]: (r, n[0]) for r, n in zip(rows, rows[1:])}
            for offset in self.epilogues:
                self.assertNotIn(offset, by_offset)
            for offset, size, name in (
                (0x1F94, 136, "RoomLib_StateDispatchVariant2_80190F7C"),
                (0x20C4, 1352, "RoomLib_HandlerD"),
                (0x292C, 156, "RoomLib_ResetSignalWithTargetGate"),
                (0x2CD4, 136, "RoomLib_StateDispatchVariant2_80191CBC"),
                (0x2E04, 1420, "RoomLib_HandlerE"),
                (0x4318, 1148, "RoomLib_HandlerC"),
            ):
                with self.subTest(scene=scene, name=name):
                    row, end = by_offset[offset]
                    self.assertEqual(row, [offset, "c", name])
                    self.assertEqual(end - offset, size)
                    self.assertEqual(source_quality.classify(
                        ROOT / "src/overlays" / scene / (name + ".c")), "semantic_c")
            for row in rows:
                if row[1] != "c":
                    continue
                suffix = row[2].rsplit("_", 1)[-1]
                if len(suffix) == 8 and all(c in "0123456789ABCDEF" for c in suffix):
                    self.assertEqual(int(suffix, 16), segment["vram"] + row[0])
            fade = ROOT / "src/overlays" / scene / "Scene_UpdateTimedFade_8018FB88.c"
            self.assertIn("#define SCENE_UPDATE_TIMED_FADE_NAME Scene_UpdateTimedFade_8018FB88",
                          fade.read_text())

    def test_each_retail_image_has_the_epilogue_evidence(self):
        for scene, digest in self.hashes.items():
            path = ROOT / "original/USA/overlays" / (scene + ".bin")
            if not path.is_file():
                self.skipTest("both local retail overlays required")
            data = path.read_bytes()
            self.assertEqual(hashlib.sha1(data).hexdigest(), digest)
            for offset in self.epilogues:
                with self.subTest(scene=scene, offset=hex(offset)):
                    restore = int.from_bytes(data[offset - 4:offset], "little")
                    self.assertEqual(restore >> 16, 0x27BD)
                    self.assertTrue(0 < restore & 0xFFFF < 0x8000)
                    self.assertEqual(data[offset:offset + 8],
                                     bytes.fromhex("0800e00300000000"))


class SceneE08MappingTests(unittest.TestCase):
    stack_epilogues = (0x6D14, 0x7A98, 0x863C, 0x922C, 0x9660)
    leaf_epilogues = (0x6694, 0x70D0, 0x73D4, 0x7E68, 0x8980)

    def setUp(self):
        self.config = yaml.safe_load(
            (ROOT / "configs/USA/overlays/scene_e08.yaml").read_text())
        segment = self.config["segments"][0]
        self.assertEqual(segment["vram"], 0x8018EFE8)
        self.rows = [[r["start"], r["type"], r.get("name")]
                     if isinstance(r, dict) else r for r in segment["subsegments"]]

    def test_header_and_compiler_jump_tables_are_data(self):
        for name in ("Scene_UpdateFadeSlots_801905F8", "Scene_InitParticleSlots_80191DB8"):
            source = ROOT / "src/overlays/scene_e08" / (name + ".c")
            self.assertIn("void " + name + "(", source.read_text())
        self.assertEqual(self.rows[:6], [
            [0, "rodatabin", "scene_e08_header"],
            [0x80, ".rodata", "RoomLib_HandlerDArgs"],
            [0x100, ".rodata", "RoomLib_HandlerEArgs"],
            [0x184, "rodata", "RoomLib_HandlerEArgsPad"],
            [0x188, ".rodata", "RoomLib_HandlerBArgs"],
            [0x1F0, ".rodata", "RoomLib_HandlerCArgs"],
        ])
        self.assertFalse((ROOT / "src/overlays/scene_e08/scene_e08_header.c").exists())

    def test_ten_complete_c_functions_and_no_epilogue_stubs(self):
        ranges = {r[0]: (r, n[0]) for r, n in zip(self.rows, self.rows[1:])}
        for offset in self.stack_epilogues + self.leaf_epilogues:
            self.assertNotIn(offset, ranges)
        for offset, size, name in (
            (0x6418, 644, "RoomLib_HandlerDArgs"),
            (0x66A4, 136, "RoomLib_StateDispatchVariant2_8019568C"),
            (0x67D4, 1352, "RoomLib_HandlerD"),
            (0x703C, 156, "RoomLib_ResetSignalWithTargetGate"),
            (0x7164, 632, "RoomLib_HandlerEArgs"),
            (0x73E4, 136, "RoomLib_StateDispatchVariant2_801963CC"),
            (0x7514, 1420, "RoomLib_HandlerE"),
            (0x7C28, 584, "RoomLib_HandlerBArgs"),
            (0x8740, 584, "RoomLib_HandlerCArgs"),
            (0x8A28, 1148, "RoomLib_HandlerC"),
        ):
            with self.subTest(name=name):
                row, end = ranges[offset]
                self.assertEqual(row, [offset, "c", name])
                self.assertEqual(end - offset, size)
                self.assertEqual(source_quality.classify(
                    ROOT / "src/overlays/scene_e08" / (name + ".c")), "semantic_c")

    def test_retail_return_paths_and_jump_table_ownership(self):
        path = ROOT / "original/USA/overlays/scene_e08.bin"
        if not path.is_file():
            self.skipTest("local retail overlay required")
        data = path.read_bytes()
        self.assertEqual(hashlib.sha1(data).hexdigest(),
                         "0561952b402bdd3f599bf37627a7d67737e8de9d")
        for offset in self.stack_epilogues:
            restore = int.from_bytes(data[offset - 4:offset], "little")
            self.assertEqual(restore >> 16, 0x27BD)
            self.assertTrue(0 < restore & 0xFFFF < 0x8000)
            self.assertEqual(data[offset:offset + 8], bytes.fromhex("0800e00300000000"))
        for offset in self.leaf_epilogues:
            self.assertEqual(data[offset:offset + 8], bytes.fromhex("0800e00321100000"))
        for table, end, owner, ret in (
            (0x80, 0x100, 0x6418, 0x6694),
            (0x100, 0x184, 0x7164, 0x73D4),
            (0x188, 0x1F0, 0x7C28, 0x7E68),
            (0x1F0, 0x258, 0x8740, 0x8980),
        ):
            targets = [int.from_bytes(data[i:i + 4], "little")
                       for i in range(table, end, 4)]
            self.assertIn(0x8018EFE8 + ret, targets)
            self.assertTrue(all(0x8018EFE8 + owner <= t <= 0x8018EFE8 + ret
                                for t in targets))
        # The reset's final conditional branch also targets its own leaf return.
        self.assertEqual(int.from_bytes(data[0x70C4:0x70C8], "little"), 0x10400002)


if __name__ == "__main__":
    unittest.main()
