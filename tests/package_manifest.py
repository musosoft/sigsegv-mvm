#!/usr/bin/env python3
"""Write a deterministic, adjacent manifest for the CSS x64 package."""
import hashlib
import json
import pathlib
import subprocess
import sys

root = pathlib.Path(__file__).resolve().parents[1]
package = pathlib.Path(sys.argv[1]).resolve()


def git(*args):
    return subprocess.check_output(["git", "-C", str(root), *args], text=True).strip()


sm = git("-C", "alliedmodders/sourcemod", "rev-parse", "HEAD")
assert sm == "b34de51b8c7f92fc2a7242fc0195e2d64a69cefd"
files = []
for path in sorted(package.rglob("*")):
    if path.is_symlink():
        raise ValueError(f"unexpected symlink: {path}")
    if path.is_file():
        data = path.read_bytes()
        files.append({"path": path.relative_to(package).as_posix(),
                      "size": len(data), "sha256": hashlib.sha256(data).hexdigest()})
assert files
manifest = {
    "schema": 1,
    "source_revision": git("rev-parse", "HEAD"),
    "source_modified": True,
    "source_diff_sha256": hashlib.sha256(subprocess.check_output(
        ["git", "-C", str(root), "diff", "HEAD", "--", "AMBuildScript", "AMBuilder",
         "configure.py", "src/mem/patch.cpp", "src/mem/patch.h"])).hexdigest(),
    "jump_header_sha256": hashlib.sha256((root / "src/mem/func_replace_jump.h").read_bytes()).hexdigest(),
    "sourcemod_revision": sm,
    "extension_api": 11,
    "game": "CSS", "platform": "Linux", "architecture": "x86_64",
    "server_build": "11003710/24",
    "server_runtime_verified": False,
    "file_count": len(files), "files": files,
}
output = package.with_name(package.name + ".manifest.json")
output.write_text(json.dumps(manifest, indent=2, sort_keys=True) + "\n")
print(output)
