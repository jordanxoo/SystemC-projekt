#ifndef GUI_MANAGER_H
#define GUI_MANAGER_H

#include <systemc.h>
#include <string>

SC_MODULE(GUIManager) {
    // Input signals - status from devices
    sc_in<bool> burner_status[4];
    sc_in<sc_uint<3>> burner_temp_leds[4];
    sc_in<bool> oven_status;
    sc_in<sc_uint<3>> oven_func_leds;
    sc_in<sc_uint<3>> oven_temp_hex;
    sc_in<sc_uint<2>> fan_speed_lcd;
    sc_in<bool> fan_on_led;
    sc_in<bool> alarm_status;

    // Output signals - controls to CPU1
    sc_out<sc_uint<5>> sw_device_select;
    sc_out<sc_uint<3>> sw_burner_temp;
    sc_out<sc_uint<3>> sw_oven_func;
    sc_out<sc_uint<3>> sw_oven_temp;
    sc_out<sc_uint<2>> sw_fan_speed;

    // Constructor
    SC_CTOR(GUIManager) {
        SC_THREAD(update_inputs_thread);
    }

    // Main GUI render function (called from main loop)
    void render();

    // Initialize GUI
    bool initialize();

    // Cleanup GUI
    void cleanup();

    // Check if GUI should close
    bool should_close();

    // Control state variables (public for GUI access)
    int selected_burner;
    int burner_temps[4];
    int oven_temp;
    int oven_function;
    int fan_speed;
    bool burner_enabled[4];
    bool oven_enabled;
    bool fan_enabled;

private:
    void update_inputs_thread();

    // GUI state
    void* window;
    bool initialized;
};

// Global functions for GUI lifecycle
bool gui_init();
void gui_render(GUIManager* manager);
void gui_cleanup();
bool gui_should_close();

#endif // GUI_MANAGER_H
