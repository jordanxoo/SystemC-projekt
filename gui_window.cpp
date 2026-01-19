#include "gui_window.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

StoveGUI& StoveGUI::getInstance() {
    static StoveGUI instance;
    return instance;
}

StoveGUI::StoveGUI() 
    : hwnd(NULL), pd3dDevice(NULL), pd3dDeviceContext(NULL), 
      pSwapChain(NULL), mainRenderTargetView(NULL),
      running(false), oven_on(false), oven_temperature(0), 
      fan_on(false), fan_speed_level(0), alarm_active(false),
      ui_oven_temp(3), ui_oven_func(0), ui_fan_speed(0) {
    for(int i = 0; i < 4; i++) {
        burners[i] = false;
        burner_temps[i] = 0;
        ui_burner_temp[i] = 3;
    }
    oven_function = "OFF";
}

StoveGUI::~StoveGUI() {
    shutdown();
}

LRESULT CALLBACK StoveGUI::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
        return true;

    StoveGUI* gui = &getInstance();
    
    switch (msg) {
        case WM_SIZE:
            if (gui->pd3dDevice != NULL && wParam != SIZE_MINIMIZED) {
                gui->CleanupRenderTarget();
                gui->pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
                gui->CreateRenderTarget();
            }
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU)
                return 0;
            break;
        case WM_DESTROY:
            gui->running = false;
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool StoveGUI::CreateDeviceD3D(HWND hWnd) {
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, 
        featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &pSwapChain, &pd3dDevice, 
        &featureLevel, &pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void StoveGUI::CleanupDeviceD3D() {
    CleanupRenderTarget();
    if (pSwapChain) { pSwapChain->Release(); pSwapChain = NULL; }
    if (pd3dDeviceContext) { pd3dDeviceContext->Release(); pd3dDeviceContext = NULL; }
    if (pd3dDevice) { pd3dDevice->Release(); pd3dDevice = NULL; }
}

void StoveGUI::CreateRenderTarget() {
    ID3D11Texture2D* pBackBuffer;
    pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
    pBackBuffer->Release();
}

void StoveGUI::CleanupRenderTarget() {
    if (mainRenderTargetView) { mainRenderTargetView->Release(); mainRenderTargetView = NULL; }
}

void StoveGUI::initialize() {
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WindowProc, 0L, 0L, 
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL, 
                      _T("StoveSimulator"), NULL };
    RegisterClassEx(&wc);
    
    hwnd = CreateWindow(wc.lpszClassName, _T("Symulator Kuchenki Elektrycznej - ImGui"), 
                        WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, 
                        NULL, NULL, wc.hInstance, NULL);

    if (!CreateDeviceD3D(hwnd)) {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return;
    }

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    
    // Własny styl
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 4.0f;
    style.GrabRounding = 4.0f;
    
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.30f, 0.30f, 0.31f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.40f, 0.40f, 0.41f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.40f, 0.40f, 0.41f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.46f, 0.47f, 0.48f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(pd3dDevice, pd3dDeviceContext);

    running = true;
}

void StoveGUI::processMessages() {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_QUIT)
            running = false;
    }
}

void StoveGUI::render() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Główne okno
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(1260, 780), ImGuiCond_FirstUseEver);
    
    ImGui::Begin("Symulator Kuchenki Elektrycznej", nullptr, 
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

    ImGui::SetWindowFontScale(1.5f);
    ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "KUCHENKA ELEKTRYCZNA");
    ImGui::SetWindowFontScale(1.0f);
    
    ImGui::SameLine(ImGui::GetWindowWidth() - 200);
    if (alarm_active) {
        ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "!!! ALARM !!!");
    } else {
        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "System OK");
    }
    
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Columns(2, "mainLayout", true);
    
    ImGui::BeginChild("StatusDisplay", ImVec2(0, 720), true);
    ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "WYSWIETLACZ STANU");
    ImGui::Separator();
    
    ImGui::Text("PALNIKI:");
    for (int row = 0; row < 2; row++) {
        ImGui::BeginGroup();
        for (int col = 0; col < 2; col++) {
            int i = row * 2 + col;
            if (col > 0) ImGui::SameLine();
            
            ImGui::BeginGroup();
            char label[32];
            sprintf(label, "Palnik %d", i);
            
            if (burners[i]) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.8f, 0.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.9f, 0.3f, 1.0f));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
            }
            
            ImGui::Button(label, ImVec2(250, 60));
            ImGui::PopStyleColor(2);
            
            if (burners[i]) {
                ImGui::Text("Status: ON | Temp: %d", burner_temps[i]);
            } else {
                ImGui::Text("Status: OFF");
            }
            ImGui::EndGroup();
        }
        ImGui::EndGroup();
        ImGui::Spacing();
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    
    ImGui::Text("PIEKARNIK:");
    if (oven_on) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.4f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.5f, 0.1f, 1.0f));
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
    }
    ImGui::Button("PIEKARNIK", ImVec2(520, 80));
    ImGui::PopStyleColor(2);
    
    if (oven_on) {
        ImGui::Text("Status: ON | Temp: %d | Funkcja: %s", 
                    oven_temperature, oven_function.c_str());
    } else {
        ImGui::Text("Status: OFF");
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    
    ImGui::Text("NAWIEW:");
    if (fan_on) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.8f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.7f, 0.9f, 1.0f));
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
    }
    ImGui::Button("WENTYLATOR", ImVec2(520, 60));
    ImGui::PopStyleColor(2);
    
    if (fan_on) {
        ImGui::Text("Status: ON | Predkosc: %d", fan_speed_level);
    } else {
        ImGui::Text("Status: OFF");
    }
    
    ImGui::EndChild();
    
    ImGui::NextColumn();
    ImGui::BeginChild("ControlPanel", ImVec2(0, 720), true);
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.8f, 1.0f), "PANEL STEROWANIA");
    ImGui::Separator();
    
    if (ImGui::CollapsingHeader("PALNIKI", ImGuiTreeNodeFlags_DefaultOpen)) {
        for (int i = 0; i < 4; i++) {
            ImGui::PushID(i);
            ImGui::Text("Palnik %d:", i);
            ImGui::SameLine(100);
            
            if (ImGui::Button("ON", ImVec2(60, 0))) {
                GUICommand cmd;
                cmd.type = GUICommand::BURNER_ON;
                cmd.device_id = i;
                cmd.value = ui_burner_temp[i];
                commandQueue.push(cmd);
            }
            ImGui::SameLine();
            
            if (ImGui::Button("OFF", ImVec2(60, 0))) {
                GUICommand cmd;
                cmd.type = GUICommand::BURNER_OFF;
                cmd.device_id = i;
                cmd.value = 0;
                commandQueue.push(cmd);
            }
            
            ImGui::SameLine();
            ImGui::Text("Temp:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);
            char temp_label[32];
            sprintf(temp_label, "##temp%d", i);
            ImGui::SliderInt(temp_label, &ui_burner_temp[i], 1, 7);
            
            ImGui::PopID();
        }
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    
    if (ImGui::CollapsingHeader("PIEKARNIK", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Button("Wlacz Piekarnik", ImVec2(200, 40))) {
            GUICommand cmd;
            cmd.type = GUICommand::OVEN_ON;
            cmd.device_id = ui_oven_func;
            cmd.value = ui_oven_temp;
            commandQueue.push(cmd);
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Wylacz Piekarnik", ImVec2(200, 40))) {
            GUICommand cmd;
            cmd.type = GUICommand::OVEN_OFF;
            cmd.device_id = 0;
            cmd.value = 0;
            commandQueue.push(cmd);
        }
        
        ImGui::Text("Temperatura:");
        ImGui::SliderInt("##oventemp", &ui_oven_temp, 1, 7);
        
        ImGui::Text("Funkcja:");
        const char* oven_modes[] = { 
            "Ogrzewanie dol", "Ogrzewanie gora", 
            "Ogrzewanie lewo", "Ogrzewanie prawo",
            "Ogrzewanie wszystko", "Termoobieg" 
        };
        ImGui::Combo("##ovenmode", &ui_oven_func, oven_modes, 6);
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    
    if (ImGui::CollapsingHeader("NAWIEW", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Predkosc wentylatora:");
        ImGui::SliderInt("##fanspeed", &ui_fan_speed, 0, 3);
        
        if (ImGui::Button("Ustaw Predkosc", ImVec2(200, 30))) {
            GUICommand cmd;
            cmd.type = GUICommand::FAN_SPEED;
            cmd.device_id = 0;
            cmd.value = ui_fan_speed;
            commandQueue.push(cmd);
        }
    }
    
    ImGui::EndChild();
    ImGui::Columns(1);
    
    ImGui::End();

    // Rendering
    ImGui::Render();
    const float clear_color[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    pd3dDeviceContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
    pd3dDeviceContext->ClearRenderTargetView(mainRenderTargetView, clear_color);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    pSwapChain->Present(1, 0);
}

void StoveGUI::updateDisplay(
    bool burner_status[4],
    int burner_temp[4],
    bool oven_status,
    int oven_temp,
    const char* oven_func,
    bool fan_status,
    int fan_speed,
    bool alarm_status
) {
    for(int i = 0; i < 4; i++) {
        burners[i] = burner_status[i];
        burner_temps[i] = burner_temp[i];
    }
    
    oven_on = oven_status;
    oven_temperature = oven_temp;
    oven_function = oven_func ? oven_func : "OFF";
    fan_on = fan_status;
    fan_speed_level = fan_speed;
    alarm_active = alarm_status;
}

bool StoveGUI::hasCommand() {
    return !commandQueue.empty();
}

GUICommand StoveGUI::getCommand() {
    GUICommand cmd = commandQueue.front();
    commandQueue.pop();
    return cmd;
}

void StoveGUI::shutdown() {
    running = false;
    
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    
    if (hwnd) {
        DestroyWindow(hwnd);
        hwnd = NULL;
    }
    
    UnregisterClass(_T("StoveSimulator"), GetModuleHandle(NULL));
}
