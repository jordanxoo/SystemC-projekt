#include "burner.h"

void Burner::monitor_state()
{
    if(enable.read() == true)
    {
        std::cout << "@" << sc_time_stamp() << " Palnik: "<< id <<": [ON] Poziom temperatury: " << temp_level.read() << std::endl;
    }
    else
    {
        std::cout << "@" << sc_time_stamp() << " Palnik: " << id << ": [OFF]" << std::endl;
    }
}