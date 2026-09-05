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


if __name__ == "__main__":
    unittest.main()
