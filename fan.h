#ifndef FAN_H
#define FAN_H


#include "systemc.h"


SC_MODULE(Fan)
{
    sc_in<sc_uint<2>> speed;

    void monitor_state();

    SC_CTOR(Fan)
    {
        SC_METHOD(monitor_state);
        sensitive << speed;
        dont_initialize();
    }
};

#endif //FAN_H
