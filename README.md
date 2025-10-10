# Projekt SystemC

## Wymagania
- SystemC 3.0.1 zainstalowany w: `C:\Users\micha\Desktop\semestr 5\SW\systemc-3.0.1`
- CMake (wersja 3.10 lub nowsza)
- Kompilator C++ obsługujący C++17 (MinGW-w64, MSVC, lub Clang)

## Struktura projektu
```
projekt/
├── CMakeLists.txt       # Konfiguracja CMake
├── src/                 # Pliki źródłowe (.cpp)
│   └── main.cpp
├── include/             # Opcjonalne pliki nagłówkowe (.h)
├── build/               # Katalog budowania (tworzony automatycznie)
└── README.md
```

## Kompilacja

### Metoda 1: CMake (zalecana)

1. Utwórz katalog build:
```powershell
mkdir build
cd build
```

2. Wygeneruj pliki projektu:
```powershell
# Dla MinGW:
cmake -G "MinGW Makefiles" ..

# Dla Visual Studio 2022:
cmake -G "Visual Studio 17 2022" ..

# Dla Visual Studio 2019:
cmake -G "Visual Studio 16 2019" ..
```

3. Skompiluj projekt:
```powershell
# Dla MinGW:
cmake --build .

# Dla Visual Studio:
cmake --build . --config Release
```

4. Uruchom:
```powershell
.\SystemC_Project.exe
```

### Metoda 2: Makefile (bez CMake)

Zobacz plik `Makefile.example` dla konfiguracji bez CMake.

### Metoda 3: Bezpośrednia kompilacja (tylko dla testów)

```powershell
g++ -o program.exe src/main.cpp -I"C:/Users/micha/Desktop/semestr 5/SW/systemc-3.0.1/include" -L"C:/Users/micha/Desktop/semestr 5/SW/systemc-3.0.1/lib" -lsystemc -std=c++17
```

## Dodawanie nowych plików

1. Dodaj nowe pliki .cpp do katalogu `src/`
2. (Opcjonalnie) Dodaj pliki .h do katalogu `include/`
3. CMake automatycznie znajdzie wszystkie pliki .cpp w `src/`

## Uwagi

- Upewnij się, że SystemC jest poprawnie zbudowany (powinien zawierać katalog `lib` z bibliotekami)
- Na Windows biblioteka może nazywać się `systemc.lib` lub `libsystemc.a` (w zależności od kompilatora)
- Jeśli wystąpią problemy z linkowanie, sprawdź czy w katalogu SystemC znajduje się właściwa biblioteka
