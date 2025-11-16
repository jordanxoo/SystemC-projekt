#include "cpu1.h"

#include "cpu1.h"

void CPU1::process_input() {
    while (true) {
        wait();
        
        // odczyt stanu przelacznikow
        sc_uint<5> current_devices = sw_device_select.read();
        sc_uint<3> current_burner_temp = sw_burner_temp.read();
        sc_uint<3> current_oven_func = sw_oven_func.read();
        sc_uint<3> current_oven_temp = sw_oven_temp.read();

        sc_uint<3> current_fan_speed = sw_fan_speed.read();

        cout << "@" << sc_time_stamp() << " CPU1: wykryto zmiane na wejsciu." << endl;
        
        // tworzymy polecenie
        Command cmd;

        // sprawdzanie ktory palnik jest aktywny
        for (int i = 0; i < 4; ++i) {
            if (current_devices[i] != last_devices_state[i]) { 
                if (current_devices[i] == 1) { // jesli ity bit ustawiony to palnik jest wlaczony
                    cout << "@" << sc_time_stamp() << " CPU1: Stan palnika " << i << " zmienil sie na ON." << endl;
                    cmd = {BURNER, i, TURN_ON, current_burner_temp.to_int()};
                    fifo_out->write(cmd);
                } else { // palnik wylaczony
                    cout << "@" << sc_time_stamp() << " CPU1: Stan palnika " << i << " zmienil sie na OFF." << endl;
                    cmd = {BURNER, i, TURN_OFF, 0};
                    fifo_out->write(cmd);
                }
            }
        }

        // sprawdzamy czy temp sie zmienila i czy jakis palnik wlaczony
        if (current_burner_temp != last_burner_temp_state) {
            for(int i = 0; i < 4; ++i) {
                if(current_devices[i] == 1) { // aktualnie ustawiona temp
                    cmd = {BURNER, i, SET_TEMP, current_burner_temp.to_int()};
                    fifo_out->write(cmd);
                }
            }
        }
        
        // TODO logika dla piekarnika i nawiewu
        if (current_devices[4] != last_devices_state[4]) { 
            if (current_devices[4] == 1) { // piekarnik wlaczony
                cout << "@" << sc_time_stamp() << " CPU1: Stan piekarnika zmienil sie na ON." << endl;
                cmd = {OVEN, 0, TURN_ON, 0};
                fifo_out->write(cmd);
                // polecenie wlaczenia z ustawiona temperatura
                cmd = {OVEN, 0, SET_OVEN_FUNCTION, current_oven_func.to_int()};
                fifo_out->write(cmd);
                cmd = {OVEN, 0, SET_TEMP, current_oven_temp.to_int()};
                fifo_out->write(cmd);
            } else { // piekarnik wylaczony
                cout << "@" << sc_time_stamp() << " CPU1: Stan piekarnika zmienil sie na OFF." << endl;
                cmd = {OVEN, 0, TURN_OFF, 0};
                fifo_out->write(cmd);
            }
        }

        // sprawdzamy czy temp sie zmienila i czy jakis piekarnik wlaczony
        if (current_devices[4] == 1) { 
            if (current_oven_func != last_oven_func_state) {
                cmd = {OVEN, 0, SET_OVEN_FUNCTION, current_oven_func.to_int()};
                fifo_out->write(cmd);
            }
            if (current_oven_temp != last_oven_temp_state) {
                cmd = {OVEN, 0, SET_TEMP, current_oven_temp.to_int()};
                fifo_out->write(cmd);
            }
        }

        if(current_fan_speed != last_fan_speed_state)
        {
            std::cout << "@" << sc_time_stamp() << " CPU1: Wykryto zmiane predkosci nawiewiu" << std::endl; 
            cmd = {FAN,0,SET_FAN_SPEED,current_fan_speed.to_int()};
            std::cout << "@" << sc_time_stamp() << " CPU1: Wysylam polecenie do FIFO" << std::endl;
            fifo_out->write(cmd);
        }
        
        // aktualizujemy zapamietany stan
        last_devices_state = current_devices;
        last_burner_temp_state = current_burner_temp;
        last_oven_func_state = current_oven_func;
        last_oven_temp_state = current_oven_temp;
        last_fan_speed_state = current_fan_speed;
    }
} 