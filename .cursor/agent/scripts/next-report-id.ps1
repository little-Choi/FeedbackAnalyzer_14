# Returns next 3-digit report ID based on Report/*.md filenames (prefix NNN-)
$ErrorActionPreference = 'Stop'
# scripts -> agent -> .cursor -> project root
$root = Split-Path (Split-Path (Split-Path $PSScriptRoot -Parent) -Parent) -Parent
if (-not (Test-Path (Join-Path $root 'Report'))) {
    New-Item -ItemType Directory -Path (Join-Path $root 'Report') -Force | Out-Null
}
$reportDir = Join-Path $root 'Report'
$max = 0
Get-ChildItem -Path $reportDir -Filter '*.md' -File -ErrorAction SilentlyContinue | ForEach-Object {
    if ($_.BaseName -match '^(\d{3})') {
        $n = [int]$Matches[1]
        if ($n -gt $max) { $max = $n }
    }
}
$next = $max + 1
Write-Output ('{0:D3}' -f $next)
