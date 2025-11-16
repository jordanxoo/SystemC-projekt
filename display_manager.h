#ifndef DISPLAY_MANAGER_H
#define DISPLAYMANAGER_H

#include "systemc.h"
#include "command.h"


SC_MODULE(DisplayManager)
{
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

        for(int i=0;i<4;i++)
        {
            sensitive << burner_status[i] << burner_temp_leds[i];
        }
        sensitive << oven_status << oven_func_leds << oven_temp_hex;
        sensitive << fan_speed_lcd << fan_on_led << alarm_status;

        dont_initialize();
    }


};

#endif //DISPLAY_MANAGER_H