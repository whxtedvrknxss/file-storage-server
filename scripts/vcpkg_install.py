# !/usr/bin/env python
import subprocess 
from pathlib import Path

script_path = Path(__file__).resolve()
base_dir = script_path.parent.parent

subprocess.run(["git", "clone", "https://github.com/microsoft/vcpkg.git", str(base_dir/"tools/vcpkg")])

