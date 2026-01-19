#!/usr/bin/env powershell

Write-Host @"
╔═══════════════════════════════════════════════════════════╗
║     INSTRUKCJA BUDOWANIA PROJEKTU Z NOWYM GUI            ║
╚═══════════════════════════════════════════════════════════╝
"@ -ForegroundColor Cyan

Write-Host "`nKROK 1: Pobieranie ImGui..." -ForegroundColor Yellow
if (-not (Test-Path "external/imgui")) {
    Write-Host "Uruchamiam download_imgui.ps1..." -ForegroundColor Green
    .\download_imgui.ps1
} else {
    Write-Host "ImGui juz istnieje" -ForegroundColor Green
}

Write-Host "`nKROK 2: Backup starego CMakeLists.txt..." -ForegroundColor Yellow
if (Test-Path "CMakeLists.txt.backup") {
    Write-Host "Backup juz istnieje" -ForegroundColor Green
} else {
    if (Test-Path "CMakeLists.txt") {
        Copy-Item "CMakeLists.txt" "CMakeLists.txt.backup" -Force
        Write-Host "Backup utworzony: CMakeLists.txt.backup" -ForegroundColor Green
    }
}

Write-Host "`nKROK 3: Czyszczenie build..." -ForegroundColor Yellow
if (Test-Path "build") {
    Remove-Item "build" -Recurse -Force
    Write-Host "build/ usuniety" -ForegroundColor Green
}

Write-Host "`nKROK 4: Tworzenie katalogu build..." -ForegroundColor Yellow
New-Item -ItemType Directory -Path "build" -Force | Out-Null
Write-Host "build/ utworzony" -ForegroundColor Green

Write-Host "`nKROK 5: Konfiguracja CMake..." -ForegroundColor Yellow
Set-Location build
cmake .. -G "MinGW Makefiles"

if ($LASTEXITCODE -eq 0) {
    Write-Host "CMake skonfigurowany pomyslnie" -ForegroundColor Green
    
    Write-Host "`nKROK 6: Budowanie projektu..." -ForegroundColor Yellow
    cmake --build .
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "`n=============================================" -ForegroundColor Green
        Write-Host "  KOMPILACJA ZAKONCZONA POMYSLNIE!" -ForegroundColor Green
        Write-Host "=============================================" -ForegroundColor Green
        
        Write-Host "`nProgram gotowy:" -ForegroundColor Cyan
        Write-Host "  stove_simulator.exe - Nowoczesne GUI z ImGui" -ForegroundColor White
        
        Write-Host "`nAby uruchomic:" -ForegroundColor Yellow
        Write-Host "  cd build" -ForegroundColor Gray
        Write-Host "  .\stove_simulator.exe" -ForegroundColor Gray
        
        Write-Host "`nLub uzyj CMake target:" -ForegroundColor Yellow
        Write-Host "  cmake --build . --target run" -ForegroundColor Gray
    } else {
        Write-Host "`nBLAD KOMPILACJI!" -ForegroundColor Red
        Write-Host "Sprawdz bledy powyzej." -ForegroundColor Red
    }
} else {
    Write-Host "`nBLAD KONFIGURACJI CMAKE!" -ForegroundColor Red
    Write-Host "Sprawdz czy SystemC jest poprawnie zainstalowany." -ForegroundColor Red
}

Set-Location ..
