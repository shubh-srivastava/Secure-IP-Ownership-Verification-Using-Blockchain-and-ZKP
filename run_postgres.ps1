$configPath = Join-Path $PSScriptRoot "postgres.local.ps1"

if (!(Test-Path $configPath)) {
    Write-Error "Missing postgres.local.ps1. Copy postgres.local.ps1.example to postgres.local.ps1 and set your password. (For MSYS bash, use run_postgres.sh + postgres.local.sh.)"
    exit 1
}

. $configPath

if (-not $env:BLOCKCHAIN_POSTGRES_CONNINFO) {
    Write-Error "BLOCKCHAIN_POSTGRES_CONNINFO is not set."
    exit 1
}

$mingwBin = "C:\msys64\mingw64\bin"
if ((Test-Path $mingwBin) -and ($env:Path -notlike "*$mingwBin*")) {
    $env:Path = "$mingwBin;$env:Path"
}

& "$PSScriptRoot\\zkp_server.exe" @args
