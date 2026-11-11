"""Regression checks for mismatched release tags and unpinned example dependencies."""
from pathlib import Path
import tempfile
import unittest

from check_core_version import validate_dependencies, validate_tag


class CoreVersionTest(unittest.TestCase):
    def test_tag_must_identify_the_core_release(self):
        validate_tag('v7.3.0', '7.3.0')
        validate_tag(None, '7.3.0')
        for tag in ('v7.2.0', 'v7.3.1', 'v7.3.0-2', '7.3.0'):
            with self.subTest(tag=tag), self.assertRaises(ValueError):
                validate_tag(tag, '7.3.0')

    def test_every_example_must_use_the_committed_submodule(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            example = root / 'examples/esp32/demo'
            example.mkdir(parents=True)
            (root / 'platformio.ini').write_text(
                '[env]\nlib_deps = symlink://lib/KymoCore\n', encoding='utf-8')
            config = example / 'platformio.ini'
            config.write_text('[env]\nlib_deps = KymoCore=symlink://../../../lib/KymoCore\n', encoding='utf-8')
            self.assertEqual(validate_dependencies(root), 2)
            for dependency in ('KymoCore=symlink://../../../../KymoCore',
                               'codename666/KymoCore@^7.3.0',
                               'https://github.com/KymoTrace/kymocore.git#main'):
                with self.subTest(dependency=dependency):
                    config.write_text(f'[env]\nlib_deps = {dependency}\n', encoding='utf-8')
                    with self.assertRaises(ValueError):
                        validate_dependencies(root)


if __name__ == '__main__':
    unittest.main()
