#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H
#include "systemc.h"
#include "gui_window.h"

SC_MODULE(UserInterface)
{
    sc_in<bool> clk;

    sc_out<sc_uint<5>> sw_device_select;
    sc_out<sc_uint<3>> sw_burner_temp[4];  // Osobny sygnal dla kazdego palnika
    sc_out<sc_uint<3>> sw_oven_func;
    sc_out<sc_uint<3>> sw_oven_temp;
    sc_out<sc_uint<2>> sw_fan_speed;
    sc_out<bool> display_trigger;

    void interactive_loop();
    void display_menu();
    void handle_burner_control();
    void handle_oven_control();
    void handle_fan_control();
    void handleGUICommand(const GUICommand& cmd);

    sc_uint<5> current_device_state;

    SC_CTOR(UserInterface) : current_device_state(0)
    {
        SC_THREAD(interactive_loop);
        sensitive << clk.pos();
    }

};

#endif
