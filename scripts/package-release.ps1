# Package the exact binaries produced by a successful Release build.
$ErrorActionPreference = 'Stop'
$version = '1.3.0-rc1'
$root = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$release = Join-Path $root 'build\Release'
$dist = Join-Path $root 'dist'
$packageName = "AZS-Speed-Controller-v$version-Windows-x64"
$folder = Join-Path $dist $packageName
$zip = Join-Path $dist "$packageName.zip"
$files = @('AZSSpeedController.exe', 'AZSSpeedHook.dll')
foreach ($name in $files) {
    if (-not (Test-Path (Join-Path $release $name) -PathType Leaf)) {
        throw "Missing $name. Run: cmake --build build --config Release"
    }
}
New-Item -ItemType Directory -Force -Path $dist | Out-Null
if (Test-Path $folder) { Remove-Item $folder -Recurse -Force }
if (Test-Path $zip) { Remove-Item $zip -Force }
New-Item -ItemType Directory -Path $folder | Out-Null
foreach ($name in $files) { Copy-Item (Join-Path $release $name) $folder }
Copy-Item (Join-Path $root 'README-PLAYERS.txt') (Join-Path $folder 'README.txt')
Copy-Item (Join-Path $root 'LICENSE') (Join-Path $folder 'LICENSE.txt')
$minhookLicense = Join-Path $root 'build\_deps\minhook-src\LICENSE.txt'
if (-not (Test-Path $minhookLicense -PathType Leaf)) { throw 'MinHook LICENSE.txt not found; cannot package without third-party notice.' }
Copy-Item $minhookLicense (Join-Path $folder 'MINHOOK-LICENSE.txt')
Compress-Archive -Path $folder -DestinationPath $zip -CompressionLevel Optimal
$hash = (Get-FileHash $zip -Algorithm SHA256).Hash.ToLowerInvariant()
$checksumFile = Join-Path $dist 'SHA256SUMS.txt'
"$hash  $packageName.zip" | Set-Content -Path $checksumFile -Encoding ascii
Write-Host "Release archive: $zip"
Write-Host "SHA256: $hash"
Write-Host "Checksums: $checksumFile"
