# Symulator Kuchenki Elektrycznej - SystemC

Projekt symulacji systemowej kuchenki elektrycznej z interfejsem graficznym (GUI) wykorzystujący SystemC i Dear ImGui.

## Opis projektu

Symulator implementuje dwuprocesorowy system kontroli kuchenki elektrycznej z następującymi komponentami:

- **4 palniki** z kontrolą temperatury (poziom 0-7)
- **Piekarnik** z 6 trybami grzania i kontrolą temperatury
- **System nawiewu** z 3 prędkościami
- **System alarmowy** monitorujący nadmierne zużycie energii
- **CPU1** - procesor interfejsu użytkownika
- **CPU2** - procesor sterowania urządzeniami

## Funkcje GUI

Nowoczesny interfejs graficzny zapewnia:

### Panel Sterowania
- Interaktywne kontrolki dla każdego palnika (włącz/wyłącz + suwak temperatury)
- Kontrola piekarnika z wyborem funkcji i temperatury
- Kontrola prędkości nawiewu
- Wizualizacja statusu wszystkich urządzeń w czasie rzeczywistym
- Wskaźnik alarmu systemowego

### Monitor Statusu
- Graficzna wizualizacja 4 palników z kolorowym wskaźnikiem temperatury
- Animowany wskaźnik piekarnika
- Animowany wentylator (obraca się zgodnie z prędkością)
- Wskaźniki LED dla wszystkich urządzeń

## Wymagania

### Linux
```bash
sudo apt-get update
sudo apt-get install -y cmake g++ libsystemc-dev libglfw3-dev libgl1-mesa-dev libglu1-mesa-dev
```

### Windows
- Visual Studio 2019/2022 z C++ toolchain
- CMake 3.10 lub nowszy
- SystemC 3.0.1 (zainstalowany w ścieżce określonej w CMakeLists.txt)
- GLFW3 i OpenGL

## Budowanie projektu

### Linux

```bash
# Klonuj repozytorium
git clone <repository-url>
cd SystemC-projekt

# Utwórz katalog build
mkdir build
cd build

# Skonfiguruj projekt
cmake ..

# Zbuduj projekt
make -j$(nproc)

# Uruchom symulator
./stove_simulator
```

### Windows

```bash
# Utwórz katalog build
mkdir build
cd build

# Skonfiguruj projekt dla Visual Studio
cmake ..

# Zbuduj projekt
cmake --build . --config Release

# Uruchom symulator
.\Release\stove_simulator.exe
```

## Opcje kompilacji

### Wyłączenie GUI
Jeśli chcesz zbudować projekt bez GUI (tylko konsola):

```bash
cmake -DENABLE_GUI=OFF ..
make
```

## Architektura systemu

```
User Input (GUI) → CPU1 (UI Processor) → Command Channel (FIFO)
                                              ↓
                                         CPU2 (Control)
                                              ↓
                           ┌──────────────────┼──────────────────┐
                           ↓                  ↓                  ↓
                      Burners (4)          Oven              Fan
                           ↓                  ↓                  ↓
                        Status LEDs → Display Manager → GUI Status
```

## Moduły SystemC

- **GUIManager** - interfejs graficzny i generowanie sygnałów wejściowych
- **UserInterface** - alternatywny interfejs konsolowy (gdy GUI wyłączone)
- **CPU1** - przetwarzanie sygnałów wejściowych i generowanie komend
- **CPU2** - wykonywanie komend i kontrola urządzeń
- **CommandChannel** - kanał komunikacji FIFO między procesorami
- **Burner** (x4) - model palników z monitorowaniem stanu
- **Oven** - model piekarnika z trybami grzania
- **Fan** - model wentylacji
- **DisplayManager** - agregacja i wyświetlanie statusów (konsola)

## System Alarmowy

System automatycznie wykrywa nieprawidłowe warunki pracy:
- Alarm gdy włączone są więcej niż 3 palniki
- Alarm gdy włączone są wszystkie 4 palniki + piekarnik
- Alarm gdy wentylator jest włączony bez piekarnika

## Technologie

- **SystemC 2.3.4/3.0.1** - środowisko symulacji systemów
- **Dear ImGui** - nowoczesna biblioteka GUI
- **GLFW** - zarządzanie oknem i input
- **OpenGL 3.0** - renderowanie grafiki
- **C++17** - standard języka

## Tryby pracy piekarnika

1. **Ogrzewanie dolne** - grzanie od dołu
2. **Ogrzewanie górne** - grzanie od góry
3. **Ogrzewanie lewe** - grzanie z lewej strony
4. **Ogrzewanie prawe** - grzanie z prawej strony
5. **Ogrzewanie wszystko** - grzanie ze wszystkich stron
6. **Termoobieg** - grzanie z obiegiem powietrza

## Struktura projektu

```
SystemC-projekt/
├── CMakeLists.txt          # Konfiguracja CMake
├── README.md               # Ten plik
├── sc_main.cpp            # Główny punkt wejścia
├── gui_manager.h/cpp      # Moduł GUI
├── user_interface.h/cpp   # Interfejs konsolowy
├── cpu1.h/cpp            # Procesor UI
├── cpu2.h/cpp            # Procesor sterowania
├── burner.h/cpp          # Model palnika
├── oven.h/cpp            # Model piekarnika
├── fan.h/cpp             # Model wentylatora
├── display_manager.h/cpp # Manager wyświetlania
├── command.h             # Definicje komend
├── command_channel.h     # Kanał FIFO
└── imgui/                # Biblioteka Dear ImGui
```

## Rozwiązywanie problemów

### Błąd: SystemC nie znaleziony
Upewnij się że SystemC jest zainstalowany:
```bash
# Linux
sudo apt-get install libsystemc-dev

# Windows
# Ustaw poprawną ścieżkę SYSTEMC_HOME w CMakeLists.txt
```

### Błąd: GLFW nie znaleziony
```bash
# Linux
sudo apt-get install libglfw3-dev

# Windows
# Zainstaluj GLFW z https://www.glfw.org/download.html
```

### GUI nie uruchamia się
- Sprawdź czy masz dostępne X11 display (Linux)
- Sprawdź czy sterowniki graficzne są zainstalowane
- Spróbuj wyłączyć GUI: `cmake -DENABLE_GUI=OFF ..`

## Autorzy

Projekt stworzony jako demonstracja integracji SystemC z nowoczesnym GUI dla celów edukacyjnych.

## Licencja

Projekt edukacyjny - SystemC używa własnej licencji Apache 2.0, ImGui używa licencji MIT.
