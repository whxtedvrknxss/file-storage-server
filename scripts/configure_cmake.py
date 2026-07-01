#!/usr/bin/env python3

import subprocess
import os 
from pathlib import Path

script_path = Path(__file__).resolve()
base_dir = script_path.parent.parent

build_path = base_dir / "build"

os.makedirs(build_path, exist_ok=True)

subprocess.run(["cmake", "-B", "build", "-S", ".", "-G", "Ninja", "-DCMAKE_BUILD_TYPE=Debug", "-DCMAKE_TOOLCHAIN_FILE=\"../tools/vcpkg/scripts/buildsystems/vcpkg.cmake\""])
