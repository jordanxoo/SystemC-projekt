#ifndef TESTBENCH_H
#define TESTBENCH_H
#include "systemc.h"

SC_MODULE(Testbench)
{
    sc_in<bool> clk;

    // wejscia symulujace przelaczniki
    sc_out<sc_uint<5>> sw_device_select;
    sc_out<sc_uint<3>> sw_burner_temp;
    sc_out<sc_uint<3>> sw_oven_func;
    sc_out<sc_uint<3>> sw_oven_temp;
    sc_out<sc_uint<2>> sw_fan_speed;

    void run_tests();

    SC_CTOR(Testbench)
    {
        SC_THREAD(run_tests);
        sensitive << clk.pos();
    }

};

#endif //TESTBENCH_H


