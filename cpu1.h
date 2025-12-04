#ifndef CPU1_H
#define CPU1_H

#include <systemc.h>
#include "command.h"
#include "command_channel.h"
SC_MODULE(CPU1) {
    sc_in<bool> clk;
    // wejscia z panelu uzytkownika
    sc_in<sc_uint<5>> sw_device_select;
    sc_in<sc_uint<3>> sw_burner_temp;
    sc_in<sc_uint<3>> sw_oven_func;
    sc_in<sc_uint<3>> sw_oven_temp;
    sc_in<sc_uint<2>> sw_fan_speed;

    // wyjscie do fifo
    sc_port<command_write_if> fifo_out;

    //metoda procesu
    void process_input();


    SC_CTOR(CPU1) {
        SC_THREAD(process_input);
        sensitive << sw_device_select << sw_burner_temp << sw_fan_speed << sw_oven_func << sw_oven_temp;
        dont_initialize(); // proces nie uruchamiany na starcie symulacji
        last_burner_temp_state = 0;
        last_devices_state = 0;

        last_oven_func_state = 0;
        last_oven_temp_state = 0;

        last_fan_speed_state = 0;
        
    }

    private:

    sc_uint<5> last_devices_state;
    sc_uint<3> last_burner_temp_state;

    sc_uint<3> last_oven_func_state;
    sc_uint<3> last_oven_temp_state;

    sc_uint<3> last_fan_speed_state;

};

#endif // CPU1_H