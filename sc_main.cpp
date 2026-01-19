#include "systemc.h"
#include "command.h"
#include "user_interface.h"
#include "cpu1.h"
#include "cpu2.h"
#include "burner.h"
#include "oven.h"
#include "fan.h"
#include "display_manager.h"
#include "command_channel.h"
#include "gui_window.h"

int sc_main(int argc, char* argv[])
{
    StoveGUI::getInstance().initialize();
    
    sc_clock clk("clk", 10, SC_NS);

    CommandChannel command_channel("command_channel");

    sc_signal<sc_uint<5>> s_sw_device_select;
    sc_signal<sc_uint<3>> s_sw_burner_temp[4];  // Osobny sygnal dla kazdego palnika
    sc_signal<sc_uint<3>> s_sw_oven_func;
    sc_signal<sc_uint<3>> s_sw_oven_temp;
    sc_signal<sc_uint<2>> s_sw_fan_speed;

    sc_signal<bool> s_burner_enable[4];
    sc_signal<sc_uint<3>> s_burner_temp[4];

    sc_signal<bool> s_oven_enable;
    sc_signal<sc_uint<3>> s_oven_temp;
    sc_signal<sc_uint<3>> s_oven_func;

    sc_signal<sc_uint<2>> s_fan_speed_control;

    sc_signal<bool> s_led_burner_status[4];
    sc_signal<sc_uint<3>> s_led_burner_temp[4];
    sc_signal<bool> s_led_oven_status;
    sc_signal<sc_uint<3>> s_led_oven_func;
    sc_signal<sc_uint<3>> s_hex_oven_temp;
    sc_signal<sc_uint<2>> s_lcd_fan_speed;
    sc_signal<bool> s_led_fan_status;
    sc_signal<bool> s_led_alarm;
    sc_signal<bool> s_display_trigger;

    UserInterface ui("UserInterface");
    CPU1 cpu1("CPU1_UI_Processor");
    CPU2 cpu2("CPU2_Control_Processor");
    Oven oven("Oven");
    Fan fan("Fan");
    DisplayManager dm("DisplayManager");

    Burner* burners[4];
    for (int i = 0; i < 4; i++)
    {
        burners[i] = new Burner(("Burner_" + std::to_string(i)).c_str());
        burners[i]->id = i;
    }

    ui.clk(clk);
    cpu1.clk(clk);
    cpu2.clk(clk);

    ui.sw_device_select(s_sw_device_select);
    cpu1.sw_device_select(s_sw_device_select);
    for(int i = 0; i < 4; i++) {
        ui.sw_burner_temp[i](s_sw_burner_temp[i]);
        cpu1.sw_burner_temp[i](s_sw_burner_temp[i]);
    }
    ui.sw_oven_func(s_sw_oven_func);
    cpu1.sw_oven_func(s_sw_oven_func);
    ui.sw_oven_temp(s_sw_oven_temp);
    cpu1.sw_oven_temp(s_sw_oven_temp);
    ui.sw_fan_speed(s_sw_fan_speed);
    cpu1.sw_fan_speed(s_sw_fan_speed);
    
    ui.display_trigger(s_display_trigger);
    
    cpu1.fifo_out(command_channel);
    cpu2.fifo_in(command_channel);

    for (int i = 0; i < 4; i++)
    {
        cpu2.burner_enable[i](s_burner_enable[i]);
        cpu2.burner_temp[i](s_burner_temp[i]);
        burners[i]->enable(s_burner_enable[i]);
        burners[i]->temp_level(s_burner_temp[i]);
    }

    cpu2.oven_enable(s_oven_enable);
    cpu2.oven_temp(s_oven_temp);
    cpu2.oven_func(s_oven_func);
    oven.enable(s_oven_enable);
    oven.temp_level(s_oven_temp);
    oven.function_mode(s_oven_func);
    
    cpu2.fan_speed(s_fan_speed_control);
    fan.speed(s_fan_speed_control);

    for (int i = 0; i < 4; i++) {
        cpu2.led_burner_status[i](s_led_burner_status[i]);
        dm.burner_status[i](s_led_burner_status[i]);
        cpu2.led_burner_temp[i](s_led_burner_temp[i]);
        dm.burner_temp_leds[i](s_led_burner_temp[i]);
    }
    dm.display_trigger(s_display_trigger);
    
    dm.clk(clk);
    
    cpu2.led_oven_status(s_led_oven_status);
    dm.oven_status(s_led_oven_status);
    cpu2.led_oven_func(s_led_oven_func);
    dm.oven_func_leds(s_led_oven_func);
    cpu2.hex_oven_temp(s_hex_oven_temp);
    dm.oven_temp_hex(s_hex_oven_temp);
    
    cpu2.lcd_fan_speed(s_lcd_fan_speed);
    dm.fan_speed_lcd(s_lcd_fan_speed);
    cpu2.led_fan_status(s_led_fan_status);
    dm.fan_on_led(s_led_fan_status);
    
    cpu2.led_alarm(s_led_alarm);
    dm.alarm_status(s_led_alarm);

    std::cout << "Simulation starting...\n";
    std::cout << "GUI: Modern ImGui Interface\n";
    std::cout << "Close GUI window to stop simulation\n\n";
    
    while(StoveGUI::getInstance().isRunning()) {
        StoveGUI::getInstance().processMessages();
        
        // Aktualizuj GUI z aktualnym stanem urzadzen (co 100 NS)
        bool burner_status_array[4];
        int burner_temp_array[4];
        for(int i = 0; i < 4; i++) {
            burner_status_array[i] = s_led_burner_status[i].read();
            burner_temp_array[i] = s_led_burner_temp[i].read().to_int();
        }
        
        const char* oven_func_str;
        switch(s_led_oven_func.read().to_uint()) {
            case 0: oven_func_str = "Ogrzewanie dol"; break;      // HEAT_BOTTOM
            case 1: oven_func_str = "Ogrzewanie gora"; break;     // HEAT_TOP
            case 2: oven_func_str = "Ogrzewanie lewo"; break;     // HEAT_LEFT
            case 3: oven_func_str = "Ogrzewanie prawo"; break;    // HEAT_RIGHT
            case 4: oven_func_str = "Ogrzewanie wszystko"; break; // HEAT_ALL
            case 5: oven_func_str = "Termoobieg"; break;          // TERMOOBIEG
            default: oven_func_str = "Nieznana funkcja"; break;
        }
        
        StoveGUI::getInstance().updateDisplay(
            burner_status_array,
            burner_temp_array,
            s_led_oven_status.read(),
            s_hex_oven_temp.read().to_int(),
            oven_func_str,
            s_led_fan_status.read(),
            s_lcd_fan_speed.read().to_int(),
            s_led_alarm.read()
        );
        
        StoveGUI::getInstance().render();
        sc_start(100, SC_NS);
    }
    
    std::cout << "\nSimulation finished...\n";
    
    StoveGUI::getInstance().shutdown();

    for (int i = 0; i < 4; i++)
    {
        delete burners[i];
    } 
    return 0;
}