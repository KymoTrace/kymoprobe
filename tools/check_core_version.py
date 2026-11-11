"""Check that Probe tags and every example use the pinned official Core release.

Read-only: never creates, moves or pushes tags. GITHUB_REF_TYPE=tag enables the
release-name check in CI; locally use --tag vX.Y.Z before creating a release tag.
"""
import argparse
import json
import os
from pathlib import Path
import re
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def git(directory, *args):
    return subprocess.check_output(['git', '-C', str(directory), *args], text=True).strip()


def validate_tag(tag, version):
    if tag is not None and tag != f'v{version}':
        raise ValueError(f'Probe tag {tag!r} must match KymoCore v{version}')


def validate_dependencies(root):
    core = (root / 'lib/KymoCore').resolve()
    files = [root / 'platformio.ini', *sorted((root / 'examples').rglob('platformio.ini'))]
    count = 0
    for path in files:
        if '.pio' in path.parts:
            continue
        for line in path.read_text(encoding='utf-8').splitlines():
            value = line.split(';', 1)[0].strip()
            if not value or 'kymocore' not in value.lower():
                continue
            match = re.search(r'symlink://([^\s]+)', value)
            if match is None or (path.parent / match[1]).resolve() != core:
                raise ValueError(f'{path.relative_to(root)} must use the pinned lib/KymoCore: {value}')
            count += 1
    if count == 0:
        raise ValueError('No pinned KymoCore consumers found')
    return count


def check(root, tag=None):
    core = root / 'lib/KymoCore'
    version = json.loads((core / 'library.json').read_text(encoding='utf-8'))['version']
    properties = dict(line.split('=', 1) for line in
                      (core / 'library.properties').read_text(encoding='utf-8').splitlines()
                      if '=' in line)
    if not re.fullmatch(r'\d+\.\d+\.\d+', version) or properties['version'] != version:
        raise ValueError('KymoCore package version manifests disagree or are not a stable release')
    validate_tag(tag, version)
    pinned = git(root, 'rev-parse', 'HEAD:lib/KymoCore')
    actual = git(core, 'rev-parse', 'HEAD')
    official = git(core, 'rev-parse', f'refs/tags/v{version}^{{commit}}')
    if pinned != actual or actual != official:
        raise ValueError('Committed submodule pointer, checkout and official Core release tag disagree')
    if git(core, 'status', '--porcelain'):
        raise ValueError('KymoCore consumer checkout must be clean')
    if subprocess.run(['git', '-C', str(core), 'symbolic-ref', '-q', 'HEAD'],
                      stdout=subprocess.DEVNULL, check=False).returncode == 0:
        raise ValueError('KymoCore must be detached at the official release')
    count = validate_dependencies(root)
    print(f'PASS: KymoCore {version}, commit {actual[:12]}, {count} pinned consumer references'
          + (f', Probe tag {tag}' if tag else ' (development checkout)'))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--tag', default=os.environ.get('GITHUB_REF_NAME')
                        if os.environ.get('GITHUB_REF_TYPE') == 'tag' else None)
    args = parser.parse_args()
    try:
        check(ROOT, args.tag)
    except (ValueError, OSError, subprocess.CalledProcessError) as error:
        parser.exit(1, f'Core version check failed: {error}\n')


if __name__ == '__main__':
    main()
