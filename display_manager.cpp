#include "display_manager.h"
#include <iostream>
#include <iomanip>

const char* oven_func_to_string_dm(sc_uint<3> mode)
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

void DisplayManager::update_display()
{
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "--- PANEL KUCHENKI ELEKTRYCZNEJ @" << std::setw(6) << sc_time_stamp() << "-------" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;


    //palniki
    for(int i=0;i<4;i++)
    {
        std::cout << "Palnik: " << i << " : ";
        if(burner_status[i].read() == true)
        {
            std::cout << "[ON/ZIELONA] Temp LEDS: ";
            for(int j=0;j<burner_temp_leds[i].read().to_int();j++)
            {
                std::cout << "*";
            }
        }
        else
            {
                std::cout << "[OFF / CZERWONA]";
            }
            std::cout<< std::endl;
    }

        
    //PIEKARNIK
    std::cout << "Piekarnik: ";
    if(oven_status.read() == true)
    {
        std::cout << "[ON / ZIELONA] Funkcja Leds:" << oven_func_to_string_dm(oven_func_leds.read())
        << " TEMP HEX: " << std::hex << oven_temp_hex.read() << std::dec;
    }
    else
    {
        std::cout << "[OFF/CZERWONA]";
    }
    std::cout << std::endl;


    //nawiew
    std::cout << "Nawiew: ";
    if(fan_on_led.read())
    {
        std::cout << "[ON / ZIELONA (LEDG0)] LCD: Szybkosc: " << fan_speed_lcd.read();
    }
    else
    {
        std::cout << "[OFF / CZERWONA] (LEDR0)";
    }
    std::cout << std::endl;


    //ALARM
    std::cout << "Alarm: ";
    if(alarm_status.read())
    {
        std::cout << "[AWARIA / CZERWONA DIODA]";
    }else
    {
        std::cout << "[OK]";
    }

    std::cout << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl << std::endl;
}