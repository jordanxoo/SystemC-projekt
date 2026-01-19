#include "gui_manager.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>

static GLFWwindow* g_window = nullptr;

void GUIManager::update_inputs_thread() {
    // This thread updates the control signals based on GUI state
    while (true) {
        wait(10, SC_MS); // Update every 10ms

        // Update device selection and control signals
        sc_uint<5> device_sel = 0;

        // Check which devices are enabled
        for (int i = 0; i < 4; i++) {
            if (burner_enabled[i]) device_sel[i] = 1;
        }
        if (oven_enabled) device_sel[4] = 1;

        sw_device_select.write(device_sel);

        // Write burner temp (for selected burner)
        if (selected_burner >= 0 && selected_burner < 4) {
            sw_burner_temp.write(burner_temps[selected_burner]);
        }

        sw_oven_func.write(oven_function);
        sw_oven_temp.write(oven_temp);
        sw_fan_speed.write(fan_speed);
    }
}

bool GUIManager::initialize() {
    selected_burner = 0;
    for (int i = 0; i < 4; i++) {
        burner_temps[i] = 0;
        burner_enabled[i] = false;
    }
    oven_temp = 0;
    oven_function = 0;
    fan_speed = 0;
    oven_enabled = false;
    fan_enabled = false;
    initialized = false;

    return gui_init();
}

void GUIManager::cleanup() {
    gui_cleanup();
}

bool GUIManager::should_close() {
    return gui_should_close();
}

void GUIManager::render() {
    gui_render(this);
}

// ===== Global GUI Functions =====

bool gui_init() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window
    g_window = glfwCreateWindow(1280, 800, "Symulator Kuchenki Elektrycznej - SystemC", nullptr, nullptr);
    if (g_window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(g_window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup style
    ImGui::StyleColorsDark();

    // Custom style for better look
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.12f, 1.0f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.2f, 0.2f, 0.25f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.3f, 0.3f, 0.35f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.4f, 0.6f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.5f, 0.7f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.35f, 0.55f, 1.0f);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(g_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    return true;
}

void gui_render(GUIManager* manager) {
    if (!g_window) return;

    // Poll events
    glfwPollEvents();

    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Get display size
    int display_w, display_h;
    glfwGetFramebufferSize(g_window, &display_w, &display_h);

    // Main control window
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(600, 750), ImGuiCond_FirstUseEver);
    ImGui::Begin("Panel Sterowania", nullptr, ImGuiWindowFlags_NoCollapse);

    ImGui::Text("Symulator Kuchenki Elektrycznej");
    ImGui::Separator();

    // ===== BURNERS SECTION =====
    ImGui::Text("");
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f));
    ImGui::Text("PALNIKI");
    ImGui::PopStyleColor();
    ImGui::Separator();

    for (int i = 0; i < 4; i++) {
        ImGui::PushID(i);

        char label[32];
        sprintf(label, "Palnik %d", i);

        // Enable/disable checkbox
        ImGui::Checkbox(label, &manager->burner_enabled[i]);
        ImGui::SameLine(150);

        // Temperature slider
        ImGui::SetNextItemWidth(200);
        if (ImGui::SliderInt("Temp", &manager->burner_temps[i], 0, 7)) {
            manager->selected_burner = i;
        }

        ImGui::SameLine(400);

        // Status LED
        bool status = manager->burner_status[i].read();
        ImVec4 led_color = status ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, led_color);
        ImGui::Button(status ? "ON" : "OFF", ImVec2(50, 0));
        ImGui::PopStyleColor();

        // Temperature display
        if (status) {
            ImGui::SameLine();
            int temp_level = manager->burner_temp_leds[i].read();
            ImGui::Text("Temp: %d/7", temp_level);
        }

        ImGui::PopID();
    }

    ImGui::Text("");
    ImGui::Separator();

    // ===== OVEN SECTION =====
    ImGui::Text("");
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.2f, 1.0f));
    ImGui::Text("PIEKARNIK");
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Checkbox("Wlacz Piekarnik", &manager->oven_enabled);

    ImGui::Text("Funkcja:");
    const char* oven_functions[] = {
        "Ogrzewanie dolne",
        "Ogrzewanie gorne",
        "Ogrzewanie lewe",
        "Ogrzewanie prawe",
        "Ogrzewanie wszystko",
        "Termoobieg"
    };
    ImGui::SetNextItemWidth(300);
    ImGui::Combo("##OvenFunction", &manager->oven_function, oven_functions, 6);

    ImGui::Text("Temperatura:");
    ImGui::SetNextItemWidth(300);
    ImGui::SliderInt("##OvenTemp", &manager->oven_temp, 0, 7);

    // Oven status
    bool oven_on = manager->oven_status.read();
    ImVec4 oven_led_color = oven_on ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    ImGui::Text("Status:");
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, oven_led_color);
    ImGui::Button(oven_on ? "ON" : "OFF", ImVec2(80, 30));
    ImGui::PopStyleColor();

    if (oven_on) {
        ImGui::SameLine();
        int func = manager->oven_func_leds.read();
        int temp_hex = manager->oven_temp_hex.read();
        ImGui::Text("Funkcja: %s | Temp: 0x%X",
                    (func < 6) ? oven_functions[func] : "???",
                    temp_hex);
    }

    ImGui::Text("");
    ImGui::Separator();

    // ===== FAN SECTION =====
    ImGui::Text("");
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.8f, 1.0f, 1.0f));
    ImGui::Text("NAWIEW");
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Text("Predkosc:");
    ImGui::SetNextItemWidth(300);
    ImGui::SliderInt("##FanSpeed", &manager->fan_speed, 0, 3,
                     manager->fan_speed == 0 ? "OFF" : "Predkosc %d");

    manager->fan_enabled = (manager->fan_speed > 0);

    // Fan status
    bool fan_on = manager->fan_on_led.read();
    int fan_speed_actual = manager->fan_speed_lcd.read();
    ImVec4 fan_led_color = fan_on ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    ImGui::Text("Status:");
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, fan_led_color);
    ImGui::Button(fan_on ? "ON" : "OFF", ImVec2(80, 30));
    ImGui::PopStyleColor();

    if (fan_on) {
        ImGui::SameLine();
        ImGui::Text("Predkosc: %d", fan_speed_actual);
    }

    ImGui::Text("");
    ImGui::Separator();

    // ===== SYSTEM STATUS =====
    ImGui::Text("");
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
    ImGui::Text("STATUS SYSTEMU");
    ImGui::PopStyleColor();
    ImGui::Separator();

    bool alarm = manager->alarm_status.read();
    if (alarm) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        ImGui::Button("ALARM!", ImVec2(200, 40));
        ImGui::PopStyleColor(2);
        ImGui::Text("UWAGA: Zbyt wiele urzadzen wlaczonych!");
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
        ImGui::Button("OK", ImVec2(200, 40));
        ImGui::PopStyleColor();
        ImGui::Text("System dziala prawidlowo");
    }

    ImGui::Text("");
    ImGui::Text("Czas symulacji: %.1f s", sc_time_stamp().to_seconds());

    ImGui::End();

    // Status monitor window
    ImGui::SetNextWindowPos(ImVec2(620, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(650, 750), ImGuiCond_FirstUseEver);
    ImGui::Begin("Monitor Statusu", nullptr, ImGuiWindowFlags_NoCollapse);

    ImGui::Text("WIZUALIZACJA STATUSU URZADZEN");
    ImGui::Separator();

    // Visual burner display
    ImGui::Text("");
    ImGui::Text("Palniki (widok z gory):");
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();

    // Draw 4 burners in 2x2 grid
    float burner_size = 100.0f;
    float spacing = 120.0f;

    for (int i = 0; i < 4; i++) {
        int row = i / 2;
        int col = i % 2;
        ImVec2 center(p.x + col * spacing + burner_size/2 + 20,
                     p.y + row * spacing + burner_size/2);

        bool is_on = manager->burner_status[i].read();
        int temp = manager->burner_temp_leds[i].read();

        // Draw burner circle
        ImU32 burner_color;
        if (is_on && temp > 0) {
            // Color from yellow to red based on temperature
            float intensity = temp / 7.0f;
            burner_color = IM_COL32(255, 255 - (int)(intensity * 155), 0, 255);
        } else {
            burner_color = IM_COL32(60, 60, 60, 255);
        }

        draw_list->AddCircleFilled(center, burner_size/2, burner_color);
        draw_list->AddCircle(center, burner_size/2, IM_COL32(200, 200, 200, 255), 32, 2.0f);

        // Draw temperature indicator
        if (is_on && temp > 0) {
            for (int t = 0; t < temp; t++) {
                float angle = (t / 7.0f) * 3.14159f * 2.0f - 3.14159f / 2.0f;
                ImVec2 indicator(center.x + cos(angle) * 35, center.y + sin(angle) * 35);
                draw_list->AddCircleFilled(indicator, 5, IM_COL32(255, 255, 0, 255));
            }
        }

        // Label
        char label[16];
        sprintf(label, "P%d", i);
        ImVec2 text_size = ImGui::CalcTextSize(label);
        draw_list->AddText(ImVec2(center.x - text_size.x/2, center.y - text_size.y/2),
                          IM_COL32(255, 255, 255, 255), label);
    }

    ImGui::Dummy(ImVec2(0, 260)); // Space for burners

    // Oven visualization
    ImGui::Separator();
    ImGui::Text("Piekarnik:");
    p = ImGui::GetCursorScreenPos();

    bool oven_on_viz = manager->oven_status.read();
    ImU32 oven_color = oven_on_viz ? IM_COL32(255, 100, 0, 255) : IM_COL32(50, 50, 50, 255);

    // Draw oven as rectangle
    draw_list->AddRectFilled(ImVec2(p.x + 20, p.y),
                            ImVec2(p.x + 220, p.y + 150),
                            oven_color, 5.0f);
    draw_list->AddRect(ImVec2(p.x + 20, p.y),
                      ImVec2(p.x + 220, p.y + 150),
                      IM_COL32(200, 200, 200, 255), 5.0f, 0, 3.0f);

    if (oven_on_viz) {
        int func = manager->oven_func_leds.read();
        const char* func_text = (func < 6) ? oven_functions[func] : "???";
        draw_list->AddText(ImVec2(p.x + 30, p.y + 60),
                          IM_COL32(255, 255, 255, 255), func_text);
    } else {
        draw_list->AddText(ImVec2(p.x + 80, p.y + 60),
                          IM_COL32(150, 150, 150, 255), "OFF");
    }

    ImGui::Dummy(ImVec2(0, 160));

    // Fan visualization
    ImGui::Separator();
    ImGui::Text("Nawiew:");
    p = ImGui::GetCursorScreenPos();

    bool fan_on_viz = manager->fan_on_led.read();
    int fan_spd = manager->fan_speed_lcd.read();

    // Draw fan
    ImVec2 fan_center(p.x + 100, p.y + 60);

    if (fan_on_viz && fan_spd > 0) {
        // Animated fan blades
        float rotation = (float)(sc_time_stamp().to_seconds() * fan_spd * 2.0);
        for (int b = 0; b < 4; b++) {
            float angle = rotation + (b * 3.14159f / 2.0f);
            ImVec2 blade_end(fan_center.x + cos(angle) * 50,
                           fan_center.y + sin(angle) * 50);
            draw_list->AddLine(fan_center, blade_end,
                             IM_COL32(100, 200, 255, 255), 4.0f);
        }
    }

    draw_list->AddCircle(fan_center, 50, IM_COL32(150, 150, 150, 255), 32, 2.0f);
    draw_list->AddCircleFilled(fan_center, 10, IM_COL32(100, 100, 100, 255));

    if (fan_on_viz) {
        char speed_text[16];
        sprintf(speed_text, "Speed: %d", fan_spd);
        draw_list->AddText(ImVec2(fan_center.x + 60, fan_center.y - 10),
                          IM_COL32(100, 200, 255, 255), speed_text);
    }

    ImGui::End();

    // Rendering
    ImGui::Render();
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(g_window);
}

void gui_cleanup() {
    if (g_window) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(g_window);
        glfwTerminate();
        g_window = nullptr;
    }
}

bool gui_should_close() {
    return g_window && glfwWindowShouldClose(g_window);
}
