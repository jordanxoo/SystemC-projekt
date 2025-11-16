cmake_minimum_required(VERSION 3.10)
project(SystemC_Project)

# Ustaw standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ścieżka do SystemC
set(SYSTEMC_HOME "C:/Users/micha/Desktop/semestr 5/SW/systemc-3.0.1")

# Dodaj ścieżki do plików nagłówkowych i bibliotek SystemC
include_directories(${SYSTEMC_HOME}/include)
link_directories(${SYSTEMC_HOME}/lib)

# Opcjonalnie: dodaj własne katalogi z nagłówkami
include_directories(${CMAKE_SOURCE_DIR}/include)

# Zbierz wszystkie pliki źródłowe z katalogu src
file(GLOB SOURCES "src/*.cpp")

# Utwórz wykonywalny plik
add_executable(exec ${SOURCES})

# Linkuj bibliotekę SystemC
target_link_libraries(exec systemc)

# Dla Windows: dodaj dodatkowe flagi
if(WIN32)
    target_compile_definitions(exec PRIVATE WIN32 _CONSOLE)
endif()
