#include "fan.h"
#include "iostream"

void Fan::monitor_state()
{
    sc_uint<2> current_speed = speed.read();

    if(current_speed == 0)
    {
        std::cout << "@" << sc_time_stamp() << " Nawiew: [OFF]" << std::endl;
    }
    else
    {
        std::cout << "@" << sc_time_stamp() << " Nawiew: [ON], Predkosc: " << current_speed << std::endl;
    }
}

