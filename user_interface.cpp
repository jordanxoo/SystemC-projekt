#include "user_interface.h"
#include "oven.h"
#include <iostream>

using namespace std;

void UserInterface::display_menu()
{
    cout << "\n========================================" << endl;
    cout << "    STOVE CONTROL SYSTEM" << endl;
    cout << "========================================" << endl;
    cout << "1. Burner control" << endl;
    cout << "2. Oven control" << endl;
    cout << "3. Fan control" << endl;
    cout << "4. Display system status" << endl;
    cout << "5. Exit" << endl;
    cout << "========================================" << endl;
    cout << "Select option: ";
}

void UserInterface::handle_burner_control()
{
    int burner_id;
    int operation;
    
    cout << "\n--- BURNER CONTROL ---" << endl;
    cout << "Select burner (0-3): ";
    cin >> burner_id;
    
    if (burner_id < 0 || burner_id > 3) {
        cout << "Invalid burner number!" << endl;
        return;
    }
    
    cout << "1. Turn on burner" << endl;
    cout << "2. Turn off burner" << endl;
    cout << "3. Set temperature" << endl;
    cout << "Select operation: ";
    cin >> operation;
    
    switch(operation) {
        case 1: {
            int temp;
            cout << "Enter temperature level (1-7): ";
            cin >> temp;
            if (temp >= 1 && temp <= 7) {
                sw_burner_temp[burner_id].write(temp);
                wait(2);
                current_device_state |= (1 << burner_id);
                sw_device_select.write(current_device_state);
                cout << "Burner " << burner_id << " turned on with temperature " << temp << endl;
                wait(10);
            } else {
                cout << "Invalid temperature level!" << endl;
            }
            break;
        }
        case 2:
            current_device_state &= ~(1 << burner_id);
            sw_device_select.write(current_device_state);
            cout << "Turning off burner " << burner_id << endl;
            wait(5);
            break;
        case 3: {
            int temp;
            cout << "Enter temperature level (1-7): ";
            cin >> temp;
            if (temp >= 1 && temp <= 7) {
                sw_burner_temp[burner_id].write(temp);
                wait(2);
                cout << "Burner " << burner_id << " temperature set to " << temp << endl;
            } else {
                cout << "Invalid temperature level!" << endl;
            }
            break;
        }
        default:
            cout << "Invalid operation!" << endl;
    }
}

void UserInterface::handle_oven_control()
{
    int operation;
    
    cout << "\n--- OVEN CONTROL ---" << endl;
    cout << "1. Turn on oven" << endl;
    cout << "2. Turn off oven" << endl;
    cout << "3. Set function" << endl;
    cout << "4. Set temperature" << endl;
    cout << "Select operation: ";
    cin >> operation;
    
    switch(operation) {
        case 1:
            current_device_state |= 0b10000;
            sw_device_select.write(current_device_state);
            cout << "Turning on oven" << endl;
            wait(10);
            break;
        case 2:
            current_device_state &= ~0b10000;
            sw_device_select.write(current_device_state);
            cout << "Turning off oven" << endl;
            wait(5);
            break;
        case 3: {
            int func;
            cout << "Select function:" << endl;
            cout << "0. Bottom heating" << endl;
            cout << "1. Top heating" << endl;
            cout << "2. Left heating" << endl;
            cout << "3. Right heating" << endl;
            cout << "4. Full heating" << endl;
            cout << "5. Convection" << endl;
            cout << "Choice: ";
            cin >> func;
            if (func >= 0 && func <= 5) {
                sw_oven_func.write(func);
                wait(5);
                cout << "Oven function set" << endl;
            } else {
                cout << "Invalid function!" << endl;
            }
            break;
        }
        case 4: {
            int temp;
            cout << "Enter temperature level (1-7): ";
            cin >> temp;
            if (temp >= 1 && temp <= 7) {
                sw_oven_temp.write(temp);
                wait(5);
                cout << "Oven temperature set to " << temp << endl;
            } else {
                cout << "Invalid temperature level!" << endl;
            }
            break;
        }
        default:
            cout << "Invalid operation!" << endl;
    }
}

void UserInterface::handle_fan_control()
{
    int speed;
    
    cout << "\n--- FAN CONTROL ---" << endl;
    cout << "Enter fan speed (0-3, 0=off): ";
    cin >> speed;
    
    if (speed >= 0 && speed <= 3) {
        sw_fan_speed.write(speed);
        wait(10);
        if (speed == 0) {
            cout << "Fan turned off" << endl;
        } else {
            cout << "Fan speed set to " << speed << endl;
        }
    } else {
        cout << "Invalid speed!" << endl;
    }
}

void UserInterface::interactive_loop()
{
    wait();
    
    current_device_state = 0;
    sw_device_select.write(0);
    for(int i = 0; i < 4; i++) {
        sw_burner_temp[i].write(0);
    }
    sw_fan_speed.write(0);
    sw_oven_func.write(0);
    sw_oven_temp.write(0);
    display_trigger.write(false);
    
    cout << "\nStove control system initialized" << endl;
    cout << "Control via GUI window only" << endl;
    
    while(true) {
        if(StoveGUI::getInstance().hasCommand()) {
            GUICommand cmd = StoveGUI::getInstance().getCommand();
            handleGUICommand(cmd);
        }
        wait(1);  // Czekaj 1 cykl zegara przed kolejnym sprawdzeniem
    }
}
void UserInterface::handleGUICommand(const GUICommand& cmd) {
    switch(cmd.type) {
        case GUICommand::BURNER_ON:
            cout << "GUI: Turning on burner " << cmd.device_id << " with temp " << cmd.value << endl;
            sw_burner_temp[cmd.device_id].write(cmd.value);
            current_device_state |= (1 << cmd.device_id);
            sw_device_select.write(current_device_state);
            break;
            
        case GUICommand::BURNER_OFF:
            cout << "GUI: Turning off burner " << cmd.device_id << endl;
            current_device_state &= ~(1 << cmd.device_id);
            sw_device_select.write(current_device_state);
            break;
            
        case GUICommand::OVEN_ON:
            cout << "GUI: Turning on oven with temp " << cmd.value << " and function " << cmd.device_id << endl;
            sw_oven_temp.write(cmd.value);
            sw_oven_func.write(cmd.device_id);
            current_device_state |= 0b10000;
            sw_device_select.write(current_device_state);
            break;
            
        case GUICommand::OVEN_OFF:
            cout << "GUI: Turning off oven" << endl;
            current_device_state &= ~0b10000;
            sw_device_select.write(current_device_state);
            break;
            
        case GUICommand::FAN_SPEED:
            cout << "GUI: Setting fan speed to " << cmd.value << endl;
            sw_fan_speed.write(cmd.value);
            break;
    }
    
    display_trigger.write(true);
    wait(1);
    display_trigger.write(false);
}