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
    void check_alarm_conditions();
    
    bool local_burner_enable[4];
    bool local_oven_enable;
    bool local_fan_status;

    SC_CTOR(CPU2) {
        SC_THREAD(process_commands);
        sensitive << clk.pos();
        
        SC_METHOD(check_alarm_conditions);
        sensitive << led_burner_status[0] << led_burner_status[1] 
                  << led_burner_status[2] << led_burner_status[3]
                  << led_oven_status << led_fan_status;
        dont_initialize();
        
        // Inicjalizacja alarmu (tylko check_alarm_conditions pisze do tego sygnalu)
        for(int i = 0; i < 4; i++)
            local_burner_enable[i] = false;
        local_oven_enable = false;
        local_fan_status = false;
    }
};

#endif // CPU2_H