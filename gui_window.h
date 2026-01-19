#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

#include <windows.h>
#include <d3d11.h>
#include <string>
#include <queue>

struct GUICommand {
    enum Type { BURNER_ON, BURNER_OFF, OVEN_ON, OVEN_OFF, FAN_SPEED } type;
    int device_id;
    int value;
};

class StoveGUI {
public:
    static StoveGUI& getInstance();
    
    void initialize();
    void processMessages();
    void render();
    void updateDisplay(
        bool burner_status[4],
        int burner_temp[4],
        bool oven_status,
        int oven_temp,
        const char* oven_func,
        bool fan_status,
        int fan_speed,
        bool alarm_status
    );
    
    bool hasCommand();
    GUICommand getCommand();
    void shutdown();
    bool isRunning() const { return running; }

private:
    StoveGUI();
    ~StoveGUI();
    
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    
    bool CreateDeviceD3D(HWND hWnd);
    void CleanupDeviceD3D();
    void CreateRenderTarget();
    void CleanupRenderTarget();
    
    HWND hwnd;
    ID3D11Device* pd3dDevice;
    ID3D11DeviceContext* pd3dDeviceContext;
    IDXGISwapChain* pSwapChain;
    ID3D11RenderTargetView* mainRenderTargetView;
    
    std::queue<GUICommand> commandQueue;
    bool running;
    
    // Stan urządzeń
    bool burners[4];
    int burner_temps[4];
    bool oven_on;
    int oven_temperature;
    std::string oven_function;
    bool fan_on;
    int fan_speed_level;
    bool alarm_active;
    
    // Kontrolki UI
    int ui_burner_temp[4];
    int ui_oven_temp;
    int ui_oven_func;
    int ui_fan_speed;
};

#endif
