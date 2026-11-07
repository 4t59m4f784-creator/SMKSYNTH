#!/usr/bin/env python3
"""Stage both current SMKSYNTH pages and retained legacy pages for GitHub Pages."""
from pathlib import Path
import shutil
import zipfile

root = Path(__file__).resolve().parents[1]
site = root / "site"
if site.exists():
    shutil.rmtree(site)
site.mkdir()
shutil.copytree(root / "web", site, dirs_exist_ok=True,
                ignore=shutil.ignore_patterns("__pycache__", "*.pyc", ".DS_Store"))
for name in ("index.html", "style.css", "review.html"):
    shutil.copy2(root / "publication" / name, site / name)
for name in ("LICENSE", "LICENSING.md", "CREDITS.md"):
    src = root / name
    if src.exists():
        shutil.copy2(src, site / ("LICENSE.txt" if name == "LICENSE" else name))
# The legacy home page advertises a source archive.
with zipfile.ZipFile(site / "source.zip", "w", zipfile.ZIP_DEFLATED) as archive:
    for path in sorted((root / "web").rglob("*")):
        if path.is_file() and path.suffix in (".js", ".html", ".css", ".json", ".md", ".txt"):
            archive.write(path, path.relative_to(root))
for rel in ("install/index.html", "install/controller.js", "install/protocol.js",
            "backup/index.html", "backup/app.js", "backup/client.js",
            "backup/bank.js", "backup/file.js"):
    assert (site / rel).is_file(), f"Missing required published asset: {rel}"
backup_code = (site / "backup/app.js").read_text()
assert "19627" in backup_code and "READ_ONLY_VERSIONS" in backup_code
assert "19627" in (site / "backup/client.js").read_text()
(site / ".nojekyll").touch()
print("PASS: legacy paths retained, current install and backup staged, 19627 read-only detection included")
