# gbafix wrapper for Windows x64
# Downloads and caches the official gbafix tool from devkitPro

$SCRIPT_DIR = Split-Path -Parent $MyInvocation.MyCommand.Path
$GBAFIX_REAL = Join-Path $SCRIPT_DIR ".gbafix-real.exe"
$GBAFIX_URL = "https://github.com/devkitPro/gba-tools/releases/download/v1.2.0/gba-tools-1.2.0-windows.zip"

# Download gbafix if not already cached
if (-not (Test-Path $GBAFIX_REAL)) {
    Write-Host "Downloading gbafix tool for Windows (first time only)..." -ForegroundColor Yellow

    # Create temp directory
    $TEMP_DIR = Join-Path $env:TEMP "gbafix_download"
    New-Item -ItemType Directory -Force -Path $TEMP_DIR | Out-Null

    try {
        # Download
        $TEMP_ZIP = Join-Path $TEMP_DIR "gba-tools.zip"
        Invoke-WebRequest -Uri $GBAFIX_URL -OutFile $TEMP_ZIP -ErrorAction Stop

        # Extract
        Expand-Archive -Path $TEMP_ZIP -DestinationPath $TEMP_DIR -Force

        # Find and copy gbafix
        $GBAFIX_SRC = Join-Path $TEMP_DIR "bin\gbafix.exe"
        if (Test-Path $GBAFIX_SRC) {
            Copy-Item $GBAFIX_SRC $GBAFIX_REAL
            Write-Host "gbafix downloaded successfully!" -ForegroundColor Green
        } else {
            Write-Host "ERROR: gbafix.exe not found in downloaded archive" -ForegroundColor Red
            exit 1
        }
    } catch {
        Write-Host "ERROR: Failed to download gbafix - $_" -ForegroundColor Red
        exit 1
    } finally {
        # Cleanup
        if (Test-Path $TEMP_DIR) {
            Remove-Item -Recurse -Force $TEMP_DIR
        }
    }
}

# Run the real gbafix
& $GBAFIX_REAL $args
exit $LASTEXITCODE
