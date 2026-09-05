import hashlib
import pathlib
import unittest

import yaml


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


if __name__ == "__main__":
    unittest.main()
