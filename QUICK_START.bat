@echo off
echo ========================================
echo   QUICK START - Symulator Kuchenki
echo ========================================
echo.

:menu
echo Wybierz opcje:
echo.
echo [1] Pobierz ImGui i zbuduj projekt (pelna instalacja)
echo [2] Tylko zbuduj projekt (ImGui juz pobrane)
echo [3] Uruchom GUI ImGui (nowoczesny)
echo [4] Uruchom GUI WinAPI (klasyczny)
echo [5] Wyczysc build i zbuduj od nowa
echo [9] Wyjscie
echo.

set /p choice="Wybor: "

if "%choice%"=="1" goto fullbuild
if "%choice%"=="2" goto build
if "%choice%"=="3" goto runimgui
if "%choice%"=="4" goto runwinapi
if "%choice%"=="5" goto clean
if "%choice%"=="9" goto end

echo Nieprawidlowy wybor!
goto menu

:fullbuild
echo.
echo Uruchamiam pelna instalacje...
powershell -ExecutionPolicy Bypass -File build_imgui.ps1
pause
goto menu

:build
echo.
echo Budowanie projektu...
if not exist build mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
cd ..
echo.
echo Kompilacja zakonczona!
pause
goto menu

:runimgui
echo.
echo Uruchamiam GUI ImGui...
if exist build\stove_simulator.exe (
    cd build
    stove_simulator.exe
    cd ..
) else (
    echo BLAD: Plik build\stove_simulator.exe nie istnieje!
    echo Najpierw zbuduj projekt (opcja 1 lub 2)
)
pause
goto menu

:runwinapi
echo.
echo UWAGA: Stary GUI WinAPI zostal zastapiony przez ImGui!
echo Uzyj opcji [3] aby uruchomic nowoczesny GUI.
pause
goto menu

:clean
echo.
echo Czyszczenie build...
if exist build rmdir /s /q build
echo Build wyczyszczony!
echo.
echo Budowanie od nowa...
if not exist build mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
cd ..
echo.
echo Kompilacja zakonczona!
pause
goto menu

:end
echo.
echo Do zobaczenia!
exit /b
