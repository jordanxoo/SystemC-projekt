# Skrypt do pobrania Dear ImGui

Write-Host "Pobieranie Dear ImGui..." -ForegroundColor Cyan

# Tworzenie folderu external
$externalDir = "external"
if (-not (Test-Path $externalDir)) {
    New-Item -ItemType Directory -Path $externalDir | Out-Null
}

Set-Location $externalDir

# Pobieranie ImGui
if (-not (Test-Path "imgui")) {
    Write-Host "Klonowanie ImGui z GitHub..." -ForegroundColor Yellow
    git clone https://github.com/ocornut/imgui.git imgui
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "ImGui pobrane pomyślnie!" -ForegroundColor Green
    } else {
        Write-Host "Błąd podczas pobierania ImGui!" -ForegroundColor Red
        exit 1
    }
} else {
    Write-Host "ImGui już istnieje, pomijanie..." -ForegroundColor Yellow
}

Set-Location ..

Write-Host @"

===========================================
INSTRUKCJE INSTALACJI:
===========================================

Dear ImGui zostało pobrane do folderu 'external/imgui'

Aby użyć nowego GUI:

1. Skompiluj projekt z nowymi plikami
2. Nowe GUI używa DirectX 11 i ImGui
3. Interfejs jest nowoczesny i responsywny

Pliki które zostały utworzone:
- gui_window_imgui.h
- gui_window_imgui.cpp

Zalety nowego GUI:
✓ Nowoczesny wygląd
✓ Łatwe w użyciu suwaki
✓ Czytelny interfejs
✓ Brak problemów z białym ekranem
✓ Responsywny layout

===========================================
"@ -ForegroundColor Cyan
