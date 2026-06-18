#!/usr/bin/env python3
import subprocess 
from pathlib import Path

script_path = Path(__file__).resolve()
base_dir = script_path.parent.parent

install_dir = base_dir/"tools/vcpkg"

subprocess.run(["git", "clone", "https://github.com/microsoft/vcpkg.git", install_dir])

