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


    bool burner_status_array[4];
    int burner_temp_array[4];
    
    for(int i=0;i<4;i++)
    {
        burner_status_array[i] = burner_status[i].read();
        burner_temp_array[i] = burner_temp_leds[i].read().to_int();
        
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

        
    bool oven_on = oven_status.read();
    int oven_temp_val = oven_temp_hex.read().to_int();
    const char* oven_func_str = oven_func_to_string_dm(oven_func_leds.read());
    
    std::cout << "Piekarnik: ";
    if(oven_status.read() == true)
    {
        std::cout << "[ON / ZIELONA] Funkcja Leds:" << oven_func_str
        << " TEMP HEX: " << std::hex << oven_temp_hex.read() << std::dec;
    }
    else
    {
        std::cout << "[OFF/CZERWONA]";
    }
    std::cout << std::endl;


    bool fan_on_status = fan_on_led.read();
    int fan_speed_val = fan_speed_lcd.read().to_int();
    
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


    bool alarm = alarm_status.read();
    
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
    
    StoveGUI::getInstance().updateDisplay(
        burner_status_array,
        burner_temp_array,
        oven_on,
        oven_temp_val,
        oven_func_str,
        fan_on_status,
        fan_speed_val,
        alarm
    );
}