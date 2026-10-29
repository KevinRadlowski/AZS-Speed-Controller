"""Publish only the CI-built v1.3.1 artifacts, without replacing an existing release."""
import hashlib
import json
import os
from pathlib import Path
import urllib.error
import urllib.parse
import urllib.request
import zipfile


def main():
    repo = "Maidenquinn/AZS-Speed-Controller"
    if os.environ.get("GITHUB_REPOSITORY") != repo or os.environ.get("GITHUB_REF") != "refs/heads/main":
        raise RuntimeError("Publication is restricted to the expected repository and main branch")
    sha = os.environ["GITHUB_SHA"]
    if len(sha) != 40 or any(c not in "0123456789abcdef" for c in sha):
        raise RuntimeError("Invalid source commit")
    tag = "v1.3.1"
    dist = Path("dist")
    name = "AZS-Speed-Controller-v1.3.1-Windows-x64.zip"
    assets = [dist / name, dist / "SHA256SUMS.txt", dist / "SHA256-BINARIES.txt"]
    for asset in assets:
        if not asset.is_file():
            raise RuntimeError(f"Missing release asset: {asset.name}")
    # PowerShell creates these text files with platform-specific newline/BOM behavior.
    sums = (dist / "SHA256SUMS.txt").read_text(encoding="utf-8-sig").splitlines()
    if len(sums) != 1:
        raise RuntimeError("Expected exactly one archive checksum")
    digest, filename = sums[0].split()
    if filename != name or hashlib.sha256(assets[0].read_bytes()).hexdigest() != digest:
        raise RuntimeError("Archive checksum mismatch")
    with zipfile.ZipFile(assets[0]) as archive:
        if archive.testzip() is not None:
            raise RuntimeError("Invalid archive")
        binary_sums = (dist / "SHA256-BINARIES.txt").read_text(encoding="utf-8-sig").splitlines()
        if len(binary_sums) != 2:
            raise RuntimeError("Expected two binary checksums")
        expected_names = {"AZSSpeedController.exe", "AZSSpeedHook.dll"}
        seen = set()
        for line in binary_sums:
            binary_digest, binary_name = line.split()
            if binary_name not in expected_names or binary_name in seen:
                raise RuntimeError("Unexpected binary checksum entry")
            seen.add(binary_name)
            member = f"AZS-Speed-Controller-v1.3.1-Windows-x64/{binary_name}"
            if hashlib.sha256(archive.read(member)).hexdigest() != binary_digest:
                raise RuntimeError("Binary checksum mismatch")

    token = os.environ["GH_TOKEN"]

    def request(method, url, body=None, content_type="application/json"):
        data = None if body is None else body if isinstance(body, bytes) else json.dumps(body).encode()
        req = urllib.request.Request(url, data=data, method=method, headers={
            "Authorization": f"Bearer {token}", "Accept": "application/vnd.github+json",
            "Content-Type": content_type, "X-GitHub-Api-Version": "2022-11-28",
            "User-Agent": "AZS-Speed-Controller-Release",
        })
        with urllib.request.urlopen(req, timeout=120) as response:
            return json.load(response)

    api = f"https://api.github.com/repos/{repo}"
    try:
        request("GET", f"{api}/releases/tags/{tag}")
    except urllib.error.HTTPError as error:
        if error.code != 404:
            raise
    else:
        raise RuntimeError("Release already exists; refusing to overwrite it")
    notes = Path("docs/RELEASE-NOTES-v1.3.1.md").read_text(encoding="utf-8")
    notes += f"\n\nPublished assets: Windows/MSVC build and timer-policy test passed in GitHub Actions for `{sha}`. These rebuilt assets differ from the preceding MinGW preview; no new in-game runtime or antivirus result is claimed.\n"
    release = request("POST", f"{api}/releases", {
        "tag_name": tag, "target_commitish": sha,
        "name": "AZS Speed Controller v1.3.1 — Chrono fix",
        "body": notes, "draft": True, "prerelease": False,
    })
    # Keep the release unpublished if any upload fails.
    upload_url = release["upload_url"].split("{", 1)[0]
    for asset in assets:
        kind = "application/zip" if asset.suffix == ".zip" else "text/plain"
        uploaded = request("POST", upload_url + "?" + urllib.parse.urlencode({"name": asset.name}),
                           asset.read_bytes(), kind)
        if uploaded.get("state") != "uploaded" or uploaded.get("size") != asset.stat().st_size:
            raise RuntimeError(f"Asset upload incomplete: {asset.name}")
    published = request("PATCH", f"{api}/releases/{release['id']}", {"draft": False, "make_latest": "true"})
    print(published["html_url"])


if __name__ == "__main__":
    main()
