#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Build the real example with a native port and verify its captured wire data."""
import argparse
import importlib.util
import os
from pathlib import Path
import shutil
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cxx', default='g++')
    parser.add_argument('--library', type=Path, default=ROOT.parents[1] / 'lib/KymoCore')
    parser.add_argument('--app', type=Path, help='Optional KymoStudio checkout for decoder checks')
    args = parser.parse_args()
    cxx = shutil.which(args.cxx)
    if cxx is None:
        parser.error('C++ compiler missing; pass --cxx with its path')
    library = args.library.resolve()
    verifier_path = library / 'examples/integration/run.py'
    if not verifier_path.is_file():
        parser.error('Initialize the pinned dependency first: git submodule update --init lib/KymoCore')
    spec = importlib.util.spec_from_file_location('released_wire_verifier', verifier_path)
    verifier = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(verifier)
    output = ROOT / '.pio/example-tests'
    output.mkdir(parents=True, exist_ok=True)
    binary = output / ('example.exe' if os.name == 'nt' else 'example')
    sources = [*sorted((ROOT / 'src').glob('*.cpp')),
               *sorted((ROOT / 'src/features').glob('*.cpp')),
               ROOT / 'tests/native_port.cpp', *sorted((library / 'src').rglob('*.cpp'))]
    os.environ['PATH'] = str(Path(cxx).parent) + os.pathsep + os.environ['PATH']
    subprocess.run([cxx, '-std=c++11', '-Wall', '-Wextra', '-Werror', '-pedantic',
                    '-fno-exceptions', '-fno-rtti', '-O1',
                    '-I' + str(ROOT / 'include'), '-I' + str(library / 'src'),
                    '-DKYMO_USER_CONFIG_HEADER="kymo_user_config.h"',
                    *map(str, sources), '-o', str(binary)], check=True)
    for name, requests in [('controls', verifier.requests()), ('no-requests', b'')]:
        directory = output / name
        directory.mkdir(exist_ok=True)
        incoming, capture = directory / 'requests.bin', directory / 'capture.bin'
        incoming.write_bytes(requests)
        subprocess.run([str(binary), str(incoming), str(capture)], check=True, timeout=15)
        stats = verifier.verify(capture.read_bytes(), with_requests=bool(requests))
        print(f'{name}: PASS {stats}', flush=True)
    if args.app:
        verifier.verify_with_app(args.app, output)


if __name__ == '__main__':
    main()
