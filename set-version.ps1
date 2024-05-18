$version = $args[0]

if ($version -match 'rollback') {
    if (Test-Path ./CMakeLists.txt.bak -PathType Leaf) {
        Remove-Item CMakeLists.txt -Force
        Move-Item CMakeLists.txt.bak CMakeLists.txt
        Write-Host "CMakeLists.txt.bak does not exist. Cannot rollback."
    } else {
        Write-Host "CMakeLists.txt.bak does not exist. Cannot rollback."
        exit 2
    }
    exit 0
}

if (-not ($version -match '^v?(\d+\.\d+\.\d+)(-[a-z0-9]+)?')) {
    Write-Host "Version must follow format: 1.2.3 or v1.2.3 or 1.2.3-alpha"
    Write-Host "Usage: set-version.ps1 [version]"
    exit 1
}

if (Test-Path ./CMakeLists.txt.bak -PathType Leaf) {
    Write-Host "CMakeLists.txt.bak exists. Delete this file to set new version."
    exit 3
}

$version = $version -replace '^v', ''
$version = $version -replace '-[a-z0-9]+$', ''
Write-Host "Setting version to $version"

Copy-Item CMakeLists.txt CMakeLists.txt.bak
$cmake = (Get-Content -Path ./CMakeLists.txt) -replace 'set\(PROJECT_VERSION "[^"]*"\)', "set(PROJECT_VERSION ""$version"")"
$cmake | Set-Content -Path ./CMakeLists.txt