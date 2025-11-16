#include "cpu2.h"


void CPU2::process_commands()
{
    led_alarm.write(false);
    led_fan_status.write(false);
    while (true)
    {
        //czekanie na polecenie FIFO -> operacja blokujaca
        std::cout << "@" << sc_time_stamp() << " CPU2: Czekam na polecenie..." << std::endl;
        Command cmd = fifo_in->read();
        std::cout << "@" << sc_time_stamp() << " CPU2: Odebrano polecenie: "<< cmd << std::endl;

        //przetwarzanie polecenia

        switch(cmd.target_device)
        {
            case BURNER:
            
            if(cmd.device_id >= 0 && cmd.device_id < 4)
            {
                if(cmd.action == TURN_ON)
                {
                    burner_enable[cmd.device_id].write(true);
                    burner_temp[cmd.device_id].write(cmd.value);

                    led_burner_status[cmd.device_id].write(true);
                    led_burner_temp[cmd.device_id].write(cmd.value);
                }
                else if(cmd.action == TURN_OFF)
                {
                    burner_enable[cmd.device_id].write(false);
                    burner_temp[cmd.device_id].write(0);

                    led_burner_status[cmd.device_id].write(false);
                    led_burner_temp[cmd.device_id].write(0);
                }
                else if(cmd.action == SET_TEMP)
                {
                    if(burner_enable[cmd.device_id].read() == true)
                    {
                        burner_temp[cmd.device_id].write(cmd.value);
                        led_burner_temp[cmd.device_id].write(cmd.value);
                    }
                }
            }
            break;

            case OVEN:
                if(cmd.action == TURN_ON)
                {
                    oven_enable.write(true);
                    led_oven_status.write(true);
                   
                }
                else if(cmd.action == TURN_OFF)
                {
                    oven_enable.write(false);
                    led_oven_status.write(false);
                }
                else if(cmd.action == SET_TEMP)
                {
                    oven_temp.write(cmd.value);
                    hex_oven_temp.write(cmd.value);
                }
                else if(cmd.action == SET_OVEN_FUNCTION)
                {
                    oven_func.write(cmd.value);
                    led_oven_func.write(cmd.value);
                }
        
            break;

            case FAN:
                if(cmd.action == SET_FAN_SPEED)
                {
                    fan_speed.write(cmd.value);
                    lcd_fan_speed.write(cmd.value);

                    if(cmd.value> 0)
                    {
                        led_fan_status.write(true);
                    }
                    else
                    {
                        led_fan_status.write(false);
                    }
                }
            break;

        }
        



    }
    
}