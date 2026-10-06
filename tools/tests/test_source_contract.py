import pathlib
import tempfile
import unittest

import yaml

from tools.scripts import check_c_subseg_sources


class SourceContractTests(unittest.TestCase):
    def test_contract_rejects_missing_extra_and_untracked_sources(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = pathlib.Path(tmp)
            source_root = root / "src"
            source_root.mkdir()
            configured_source = source_root / "configured.c"
            extra_source = source_root / "extra.c"
            extra_source.write_text("void extra(void) {}\n")

            config = root / "main.yaml"
            config.write_text(yaml.safe_dump({
                "options": {"src_path": str(source_root)},
                "segments": [{"subsegments": [[0x100, "c", "configured"]]}],
            }))

            missing, extra, untracked = check_c_subseg_sources.source_contract(
                [config], [source_root], tracked=set()
            )

        self.assertEqual(missing, {configured_source})
        self.assertEqual(extra, {extra_source})
        self.assertEqual(untracked, {extra_source})

    def test_matching_tracked_source_satisfies_contract(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = pathlib.Path(tmp)
            source_root = root / "src"
            source_root.mkdir()
            source = source_root / "unit.c"
            source.write_text("void unit(void) {}\n")
            config = root / "main.yaml"
            config.write_text(yaml.safe_dump({
                "options": {"src_path": str(source_root)},
                "segments": [{"subsegments": [[0x100, "c", "unit"]]}],
            }))

            result = check_c_subseg_sources.source_contract(
                [config], [source_root], tracked={source}
            )

        self.assertEqual(result, (set(), set(), set()))

    def test_duplicate_manifest_entry_is_detected(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = pathlib.Path(tmp)
            source_root = root / "src"
            source_root.mkdir()
            config = root / "main.yaml"
            config.write_text(yaml.safe_dump({
                "options": {"src_path": str(source_root)},
                "segments": [{"subsegments": [
                    [0x100, "c", "unit"],
                    [0x200, "c", "unit"],
                ]}],
            }))

            duplicates = check_c_subseg_sources.duplicate_configured_sources(
                [config]
            )

        self.assertEqual(duplicates, {source_root / "unit.c"})

    def test_shared_library_source_may_serve_several_manifests(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = pathlib.Path(tmp)
            library = root / "src" / "lib"
            library.mkdir(parents=True)
            source = library / "shared.c"
            source.write_text("void shared(void) {}\n")
            configs = []
            for name in ("a", "b"):
                overlay = root / "src" / name
                overlay.mkdir()
                config = root / f"{name}.yaml"
                config.write_text(yaml.safe_dump({
                    "options": {"src_path": str(overlay)},
                    "segments": [{"subsegments": [[0x100, "c", "../lib/shared"]]}],
                }))
                configs.append(config)
            saved = check_c_subseg_sources.SHARED_ROOTS
            check_c_subseg_sources.SHARED_ROOTS = (library,)
            try:
                contract = check_c_subseg_sources.source_contract(
                    configs, [root / "src"], tracked={source})
                shared = check_c_subseg_sources.duplicate_configured_sources(configs)
                check_c_subseg_sources.SHARED_ROOTS = ()
                unshared = check_c_subseg_sources.duplicate_configured_sources(configs)
            finally:
                check_c_subseg_sources.SHARED_ROOTS = saved

        self.assertEqual(contract, (set(), set(), set()))
        self.assertEqual(shared, set())
        self.assertEqual(unshared, {source})


if __name__ == "__main__":
    unittest.main()
