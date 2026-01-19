cmake_minimum_required(VERSION 3.10)
project(SystemC_Project)

# Ustaw standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option to enable GUI
option(ENABLE_GUI "Enable ImGui GUI interface" ON)

# Ścieżka do SystemC
if(WIN32)
    set(SYSTEMC_HOME "C:/Users/micha/Desktop/semestr 5/SW/systemc-3.0.1")
    # Dodaj ścieżki do plików nagłówkowych i bibliotek SystemC dla Windows
    include_directories(${SYSTEMC_HOME}/include)
    link_directories(${SYSTEMC_HOME}/lib)
else()
    # For Linux, SystemC is installed via package manager (libsystemc-dev)
    # Headers are in /usr/include, library in /usr/lib
    # No need to set paths manually as they are in standard locations
endif()

# Opcjonalnie: dodaj własne katalogi z nagłówkami
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR})

# Zbierz wszystkie pliki źródłowe (główny katalog)
set(SOURCES
    sc_main.cpp
    user_interface.cpp
    cpu1.cpp
    cpu2.cpp
    burner.cpp
    oven.cpp
    fan.cpp
    display_manager.cpp
)

# Add ImGui sources if GUI is enabled
if(ENABLE_GUI)
    message(STATUS "Building with ImGui GUI support")

    # ImGui directory
    set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/imgui)

    # Add ImGui sources
    list(APPEND SOURCES
        gui_manager.cpp
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    )

    # Add ImGui include directory
    include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)

    # Find required packages for GUI
    find_package(OpenGL REQUIRED)
    find_package(glfw3 REQUIRED)

    add_compile_definitions(ENABLE_GUI)
endif()

# Utwórz wykonywalny plik
add_executable(stove_simulator ${SOURCES})

# Linkuj bibliotekę SystemC
target_link_libraries(stove_simulator systemc pthread)

# Link GUI libraries if enabled
if(ENABLE_GUI)
    target_link_libraries(stove_simulator OpenGL::GL glfw dl)
endif()

# Dla Windows: dodaj dodatkowe flagi
if(WIN32)
    target_compile_definitions(stove_simulator PRIVATE WIN32 _CONSOLE)
endif()

# Dodaj target do uruchomienia programu
add_custom_target(run
    COMMAND stove_simulator
    DEPENDS stove_simulator
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Uruchamianie symulacji SystemC..."
)
