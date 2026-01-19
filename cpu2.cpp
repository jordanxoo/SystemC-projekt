#include "cpu2.h"


void CPU2::process_commands()
{
    led_alarm.write(false);
    led_fan_status.write(false);
    
    for(int i = 0; i < 4; i++)
        local_burner_enable[i] = false;
    local_oven_enable = false;
    local_fan_status = false;
    
    while (true)
    {
        //czekanie na polecenie FIFO -> operacja blokujaca
        std::cout << "@" << sc_time_stamp() << " CPU2: Czekam na polecenie..." << std::endl;
        Command cmd = fifo_in->read_command();
        std::cout << "@" << sc_time_stamp() << " CPU2: Odebrano polecenie: "<< cmd << std::endl;

        //przetwarzanie polecenia

        switch(cmd.target_device)
        {
            case BURNER:
            
            if(cmd.device_id >= 0 && cmd.device_id < 4)
            {
                if(cmd.action == TURN_ON)
                {
                    if(cmd.value < 1 || cmd.value > 7)
                    {
                        std::cout << "@" << sc_time_stamp() << " CPU2: BLAD! Nieprawidlowa wartosc temperatury palnika: " << cmd.value << " (dozwolone: 1-7)" << std::endl;
                        break;
                    }
                    
                    burner_enable[cmd.device_id].write(true);
                    burner_temp[cmd.device_id].write(cmd.value);

                    led_burner_status[cmd.device_id].write(true);
                    led_burner_temp[cmd.device_id].write(cmd.value);
                    
                    local_burner_enable[cmd.device_id] = true;
                }
                else if(cmd.action == TURN_OFF)
                {
                    burner_enable[cmd.device_id].write(false);
                    burner_temp[cmd.device_id].write(0);

                    led_burner_status[cmd.device_id].write(false);
                    led_burner_temp[cmd.device_id].write(0);
                    
                    local_burner_enable[cmd.device_id] = false;
                }
                else if(cmd.action == SET_TEMP)
                {
                    if(cmd.value < 1 || cmd.value > 7)
                    {
                        std::cout << "@" << sc_time_stamp() << " CPU2: BLAD! Nieprawidlowa wartosc temperatury: " << cmd.value << " (dozwolone: 1-7)" << std::endl;
                        break;
                    }
                    
                    burner_temp[cmd.device_id].write(cmd.value);
                    led_burner_temp[cmd.device_id].write(cmd.value);
                }
            }
            else
            {
                std::cout << "@" << sc_time_stamp() << " CPU2: BLAD! Nieprawidlowy ID palnika: " << cmd.device_id << " (dozwolone: 0-3)" << std::endl;
            }
            break;

            case OVEN:
                if(cmd.action == TURN_ON)
                {
                    oven_enable.write(true);
                    led_oven_status.write(true);
                    local_oven_enable = true;
                   
                }
                else if(cmd.action == TURN_OFF)
                {
                    oven_enable.write(false);
                    led_oven_status.write(false);
                    local_oven_enable = false;
                }
                else if(cmd.action == SET_TEMP)
                {   
                    if(cmd.value < 1 || cmd.value > 7)
                    {
                        std::cout << "@" << sc_time_stamp() << " CPU2: BLAD! Nieprawidlowa wartosc temperatury piekarnika: " << cmd.value << " (dozwolone: 1-7)" << std::endl;
                        break;
                    }
                    
                    oven_temp.write(cmd.value);
                    hex_oven_temp.write(cmd.value);
                }
                else if(cmd.action == SET_OVEN_FUNCTION)
                {
                    if(cmd.value > 5)
                    {
                        std::cout << "@" << sc_time_stamp() << " CPU2: BLAD! Nieprawidlowa funkcja piekarnika: " << cmd.value << " (dozwolone: 0-5)" << std::endl;
                        break;
                    }
                    
                    oven_func.write(cmd.value);
                    led_oven_func.write(cmd.value);
                }
        
            break;

            case FAN:
                if(cmd.action == SET_FAN_SPEED)
                {
                    if(cmd.value > 3)
                    {
                        std::cout << "@" << sc_time_stamp() << " CPU2: BLAD! Nieprawidlowa predkosc nawiewu: " << cmd.value << " (dozwolone: 0-3)" << std::endl;
                        break;
                    }
                    
                    fan_speed.write(cmd.value);
                    lcd_fan_speed.write(cmd.value);

                    if(cmd.value > 0 && cmd.value <= 3)
                    {
                        led_fan_status.write(true);
                        local_fan_status = true;
                    }
                    else
                    {
                        led_fan_status.write(false);
                        local_fan_status = false;
                    }
                }
            break;

        }
        
        check_alarm_conditions();
    }
}

void CPU2::check_alarm_conditions()
{
    bool alarm = false;
    
    int active_burners = 0;
    for(int i = 0; i < 4; i++)
    {
        if(led_burner_status[i].read() == true)
            active_burners++;
    }
    
    if(active_burners > 3)
    {
        std::cout << "@" << sc_time_stamp() << " CPU2: ALARM! Zbyt wiele palnikow wlaczonych: " << active_burners << std::endl;
        alarm = true;
    }
    
    if(active_burners == 4 && led_oven_status.read() == true)
    {
        std::cout << "@" << sc_time_stamp() << " CPU2: ALARM! Przeciazenie - wszystkie palniki i piekarnik wlaczone!" << std::endl;
        alarm = true;
    }
    
    std::cout << "@" << sc_time_stamp() << " CPU2: Sprawdzam alarm nawiewu: fan=" 
              << local_fan_status << " oven=" << local_oven_enable << std::endl;
    
    if(local_fan_status == true && local_oven_enable == false)
    {
        std::cout << "@" << sc_time_stamp() << " CPU2: ALARM! Nawiew wlaczony bez piekarnika!" << std::endl;
        alarm = true;
    }
    
    led_alarm.write(alarm);
}