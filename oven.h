#ifndef OVEN_H
#define OVEN_H

#include "systemc.h"
#include "command.h"

SC_MODULE(Oven)
{
    sc_in<bool> enable;
    sc_in<sc_uint<3>> temp_level;
    sc_in<sc_uint<3>> function_mode;

    void monitor_state();

    SC_CTOR(Oven)
    {
        SC_METHOD(monitor_state);

        sensitive << enable << temp_level << function_mode;
        dont_initialize();
    }

};
#endif //OVEN_H