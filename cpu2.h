#ifndef CPU2_H
#define CPU2_H

#include <systemc.h>
#include "command.h"
#include "command_channel.h"
SC_MODULE(CPU2) {
    //porty
    sc_in<bool> clk;
    //wejscie z fifo
    sc_port<command_read_if> fifo_in;
  
  //wejscia sterujace do peryferiow
    sc_out<bool> burner_enable[4];
    sc_out<sc_uint<3>> burner_temp[4];
    sc_out<bool> oven_enable;

    sc_out<sc_uint<3>> oven_temp;
    sc_out<sc_uint<3>> oven_func;
    sc_out<sc_uint<2>> fan_speed;


    //wejscia do displaymanager
    sc_out<bool> led_burner_status[4];
    sc_out<sc_uint<3>> led_burner_temp[4];
    sc_out<bool> led_oven_status;
    sc_out<sc_uint<3>> led_oven_func;
    sc_out<sc_uint<3>> hex_oven_temp;
    sc_out<sc_uint<2>> lcd_fan_speed;
    sc_out<bool> led_fan_status;
    sc_out<bool> led_alarm;

    //metoda procesu
    void process_commands();

    SC_CTOR(CPU2) {
        SC_THREAD(process_commands);
        sensitive << clk.pos();
    }
};

#endif // CPU2_H