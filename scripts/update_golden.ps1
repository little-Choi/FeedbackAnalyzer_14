# Regenerate tests/expected/* golden files from current application output.
param(
    [string]$BuildDir = "build"
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
Push-Location $root

if (-not (Test-Path "$BuildDir/tests/feedback_analyzer_tests.exe")) {
    cmake -S . -B $BuildDir -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=g++.exe -DBUILD_TESTING=ON
    cmake --build $BuildDir --target feedback_analyzer_tests
}

$env:FEEDBACK_ANALYZER_UPDATE_GOLDEN = "1"
& "$BuildDir/tests/feedback_analyzer_tests.exe" "[golden]" --reporter compact
Remove-Item Env:FEEDBACK_ANALYZER_UPDATE_GOLDEN -ErrorAction SilentlyContinue

Write-Host "Golden files updated under tests/expected/"
Pop-Location
