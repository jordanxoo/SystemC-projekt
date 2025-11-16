#ifndef BURNER_H
#define BURNER_H

#include "systemc.h"

SC_MODULE(Burner)
{
    sc_in<bool> enable;
    sc_in<sc_uint<3>> temp_level;

    int id; // id zeby wiedziec ktory to palnik

    void monitor_state();

    SC_CTOR(Burner) : id(0)
    {
        SC_METHOD(monitor_state);
        sensitive << enable << temp_level ;
        dont_initialize();
    }
};

#endif //BURNER_H