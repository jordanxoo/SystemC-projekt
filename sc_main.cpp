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

#ifdef ENABLE_GUI
#include "gui_manager.h"
#include <thread>
#include <atomic>

std::atomic<bool> simulation_running(true);
GUIManager* g_gui_manager = nullptr;

void systemc_simulation_thread() {
    sc_start();
    simulation_running = false;
}
#endif

int sc_main(int argc, char* argv[])
{
    sc_clock clk("clk", 10, SC_NS);

    CommandChannel command_channel("command_channel");

    sc_signal<sc_uint<5>> s_sw_device_select;
    sc_signal<sc_uint<3>> s_sw_burner_temp;
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

#ifdef ENABLE_GUI
    GUIManager gui("GUIManager");
    g_gui_manager = &gui;
#else
    UserInterface ui("UserInterface");
#endif

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

#ifdef ENABLE_GUI
    // No clock needed for GUI
    cpu1.clk(clk);
    cpu2.clk(clk);

    gui.sw_device_select(s_sw_device_select);
    cpu1.sw_device_select(s_sw_device_select);
    gui.sw_burner_temp(s_sw_burner_temp);
    cpu1.sw_burner_temp(s_sw_burner_temp);
    gui.sw_oven_func(s_sw_oven_func);
    cpu1.sw_oven_func(s_sw_oven_func);
    gui.sw_oven_temp(s_sw_oven_temp);
    cpu1.sw_oven_temp(s_sw_oven_temp);
    gui.sw_fan_speed(s_sw_fan_speed);
    cpu1.sw_fan_speed(s_sw_fan_speed);
#else
    ui.clk(clk);
    cpu1.clk(clk);
    cpu2.clk(clk);

    ui.sw_device_select(s_sw_device_select);
    cpu1.sw_device_select(s_sw_device_select);
    ui.sw_burner_temp(s_sw_burner_temp);
    cpu1.sw_burner_temp(s_sw_burner_temp);
    ui.sw_oven_func(s_sw_oven_func);
    cpu1.sw_oven_func(s_sw_oven_func);
    ui.sw_oven_temp(s_sw_oven_temp);
    cpu1.sw_oven_temp(s_sw_oven_temp);
    ui.sw_fan_speed(s_sw_fan_speed);
    cpu1.sw_fan_speed(s_sw_fan_speed);

    ui.display_trigger(s_display_trigger);
#endif
    
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

#ifdef ENABLE_GUI
    // Connect GUI status inputs
    for (int i = 0; i < 4; i++) {
        gui.burner_status[i](s_led_burner_status[i]);
        gui.burner_temp_leds[i](s_led_burner_temp[i]);
    }
    gui.oven_status(s_led_oven_status);
    gui.oven_func_leds(s_led_oven_func);
    gui.oven_temp_hex(s_hex_oven_temp);
    gui.fan_speed_lcd(s_lcd_fan_speed);
    gui.fan_on_led(s_led_fan_status);
    gui.alarm_status(s_led_alarm);

    // Initialize GUI
    std::cout << "Inicjalizacja GUI...\n";
    if (!gui.initialize()) {
        std::cerr << "Nie udalo sie zainicjalizowac GUI!\n";
        return 1;
    }

    std::cout << "Symulacja startuje...\n";
    // Start SystemC simulation in separate thread
    std::thread sim_thread(systemc_simulation_thread);

    // Main GUI loop
    while (!gui.should_close() && simulation_running) {
        gui.render();
    }

    // Wait for simulation to complete
    if (simulation_running) {
        sc_stop();
    }
    sim_thread.join();

    gui.cleanup();
    std::cout << "Symulacja zakonczona...\n";
#else
    std::cout << "Symulacja startuje...\n";
    sc_start();
    std::cout << "Symulacja zakonczona...\n";
#endif

    for (int i = 0; i < 4; i++)
    {
        delete burners[i];
    }
    return 0;
}