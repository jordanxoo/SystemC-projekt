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

# ============================================
# GUI z ImGui + DirectX 11
# ============================================

# Ścieżka do ImGui
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/external/imgui")

if(NOT EXISTS ${IMGUI_DIR})
    message(FATAL_ERROR "ImGui nie znalezione! Uruchom: .\\download_imgui.ps1")
endif()

message(STATUS "ImGui znalezione w: ${IMGUI_DIR}")

# Pliki źródłowe ImGui
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
    ${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
)

# Pliki projektu
set(SOURCES
    sc_main.cpp
    user_interface.cpp
    cpu1.cpp
    cpu2.cpp
    burner.cpp
    oven.cpp
    fan.cpp
    display_manager.cpp
    gui_window.cpp
    ${IMGUI_SOURCES}
)

# Dodaj ścieżki ImGui
include_directories(${IMGUI_DIR})
include_directories(${IMGUI_DIR}/backends)

# Utwórz wykonywalny plik
add_executable(stove_simulator ${SOURCES})

# Linkuj biblioteki
if(WIN32)
    target_link_libraries(stove_simulator 
        systemc 
        d3d11.lib 
        dxgi.lib 
        d3dcompiler.lib
        dwmapi.lib
    )
    target_compile_definitions(stove_simulator PRIVATE 
        WIN32 
        _CONSOLE
        UNICODE
        _UNICODE
    )
else()
    target_link_libraries(stove_simulator systemc)
endif()

# Dodaj target do uruchomienia programu
add_custom_target(run
    COMMAND stove_simulator.exe
    DEPENDS stove_simulator
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Uruchamianie symulacji z GUI ImGui..."
)

message(STATUS "========================================")
message(STATUS "Konfiguracja: Nowoczesne GUI (ImGui)")
message(STATUS "========================================")
