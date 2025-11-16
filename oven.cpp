#include "oven.h"
#include<iostream>

const char* oven_func_to_string(sc_uint<3> mode)
{
    switch(mode)
    {
        case HEAT_BOTTOM: return "ogrzewanie dol"; break;
        case HEAT_LEFT: return "ogrzewanie lewo";  break;
        case HEAT_ALL: return "ogrzewanie wszystko"; break;
        case HEAT_RIGHT: return "ogrzewanie prawo"; break;
        case HEAT_TOP: return "ogrzewanie gora"; break;
        case TERMOOBIEG: return "Termoobieg"; break;
        default: return "Nieznana funkcja"; break;
    }
}

void Oven::monitor_state()
{
    if(enable.read() == true)
    {
        std::cout << "@" << sc_time_stamp() << " Piekarnik: [ON]" <<
        " Funkcja: " << oven_func_to_string(function_mode.read()) << " , " 
        << " Temp: " << temp_level.read() << std::endl;
    }else
    {
        std::cout << "@" << sc_time_stamp() << " Piekarnik: [OFF]" << std::endl;
    }
}