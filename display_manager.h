#ifndef DISPLAY_MANAGER_H
#define DISPLAYMANAGER_H

#include "systemc.h"
#include "command.h"
#include "gui_window.h"


SC_MODULE(DisplayManager)
{
    sc_in<bool> clk;
    sc_in<bool> display_trigger;
    
    //wejscia dla palnika
    sc_in<bool> burner_status[4];
    sc_in<sc_uint<3>> burner_temp_leds[4];

    //wejscia dla stanu piekarnika
    sc_in<bool> oven_status;
    sc_in<sc_uint<3>> oven_func_leds;
    sc_in<sc_uint<3>> oven_temp_hex;

    //wejscia dla stanu nawiewiu
    sc_in<sc_uint<2>> fan_speed_lcd;
    sc_in<bool> fan_on_led; // zielona dioda dla nawiewiu

    //dioda alarmu
    sc_in<bool> alarm_status;

    void update_display();

    SC_CTOR(DisplayManager)
    {
        SC_METHOD(update_display);
        sensitive << display_trigger.pos();
        dont_initialize();
    }


};

#endif //DISPLAY_MANAGER_H