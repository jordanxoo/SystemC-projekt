#include "user_interface.h"
#include "oven.h"
#include <iostream>

using namespace std;

void UserInterface::display_menu()
{
    cout << "\n========================================" << endl;
    cout << "    SYSTEM STEROWANIA KUCHENKA" << endl;
    cout << "========================================" << endl;
    cout << "1. Sterowanie palnikami" << endl;
    cout << "2. Sterowanie piekarnikiem" << endl;
    cout << "3. Sterowanie nawiewem" << endl;
    cout << "4. Wyswietl stan systemu" << endl;
    cout << "5. Wyjscie" << endl;
    cout << "========================================" << endl;
    cout << "Wybierz opcje: ";
}

void UserInterface::handle_burner_control()
{
    int burner_id;
    int operation;
    
    cout << "\n--- STEROWANIE PALNIKAMI ---" << endl;
    cout << "Wybierz palnik (0-3): ";
    cin >> burner_id;
    
    if (burner_id < 0 || burner_id > 3) {
        cout << "Nieprawidlowy numer palnika!" << endl;
        return;
    }
    
    cout << "1. Wlacz palnik" << endl;
    cout << "2. Wylacz palnik" << endl;
    cout << "3. Ustaw temperature" << endl;
    cout << "Wybierz operacje: ";
    cin >> operation;
    
    switch(operation) {
        case 1: {
            int temp;
            cout << "Podaj poziom temperatury (1-7): ";
            cin >> temp;
            if (temp >= 1 && temp <= 7) {
                sw_burner_temp.write(temp);
                wait(2);
                current_device_state |= (1 << burner_id); // Set bit for this burner
                sw_device_select.write(current_device_state);
                cout << "Wlaczam palnik " << burner_id << " z temperatura " << temp << endl;
                wait(10);
            } else {
                cout << "Nieprawidlowy poziom temperatury!" << endl;
            }
            break;
        }
        case 2:
            current_device_state &= ~(1 << burner_id); // Clear bit for this burner
            sw_device_select.write(current_device_state);
            cout << "Wylaczam palnik " << burner_id << endl;
            wait(5);
            break;
        case 3: {
            int temp;
            cout << "Podaj poziom temperatury (1-7): ";
            cin >> temp;
            if (temp >= 1 && temp <= 7) {
                sw_burner_temp.write(temp);
                wait(5);
                cout << "Ustawiono temperature palnika na " << temp << endl;
            } else {
                cout << "Nieprawidlowy poziom temperatury!" << endl;
            }
            break;
        }
        default:
            cout << "Nieprawidlowa operacja!" << endl;
    }
}

void UserInterface::handle_oven_control()
{
    int operation;
    
    cout << "\n--- STEROWANIE PIEKARNIKIEM ---" << endl;
    cout << "1. Wlacz piekarnik" << endl;
    cout << "2. Wylacz piekarnik" << endl;
    cout << "3. Ustaw funkcje" << endl;
    cout << "4. Ustaw temperature" << endl;
    cout << "Wybierz operacje: ";
    cin >> operation;
    
    switch(operation) {
        case 1:
            current_device_state |= 0b10000; // Set bit 4 for oven
            sw_device_select.write(current_device_state);
            cout << "Wlaczam piekarnik" << endl;
            wait(10);
            break;
        case 2:
            current_device_state &= ~0b10000; // Clear bit 4 for oven
            sw_device_select.write(current_device_state);
            cout << "Wylaczam piekarnik" << endl;
            wait(5);
            break;
        case 3: {
            int func;
            cout << "Wybierz funkcje:" << endl;
            cout << "0. Ogrzewanie dol" << endl;
            cout << "1. Ogrzewanie gora" << endl;
            cout << "2. Ogrzewanie lewo" << endl;
            cout << "3. Ogrzewanie prawo" << endl;
            cout << "4. Ogrzewanie wszystko" << endl;
            cout << "5. Termoobieg" << endl;
            cout << "Wybor: ";
            cin >> func;
            if (func >= 0 && func <= 5) {
                sw_oven_func.write(func);
                wait(5);
                cout << "Ustawiono funkcje piekarnika" << endl;
            } else {
                cout << "Nieprawidlowa funkcja!" << endl;
            }
            break;
        }
        case 4: {
            int temp;
            cout << "Podaj poziom temperatury (1-7): ";
            cin >> temp;
            if (temp >= 1 && temp <= 7) {
                sw_oven_temp.write(temp);
                wait(5);
                cout << "Ustawiono temperature piekarnika na " << temp << endl;
            } else {
                cout << "Nieprawidlowy poziom temperatury!" << endl;
            }
            break;
        }
        default:
            cout << "Nieprawidlowa operacja!" << endl;
    }
}

void UserInterface::handle_fan_control()
{
    int speed;
    
    cout << "\n--- STEROWANIE NAWIEWEM ---" << endl;
    cout << "Podaj predkosc nawiewu (0-3, 0=wylaczony): ";
    cin >> speed;
    
    if (speed >= 0 && speed <= 3) {
        sw_fan_speed.write(speed);
        wait(10);
        if (speed == 0) {
            cout << "Wylaczono nawiew" << endl;
        } else {
            cout << "Ustawiono predkosc nawiewu na " << speed << endl;
        }
    } else {
        cout << "Nieprawidlowa predkosc!" << endl;
    }
}

void UserInterface::interactive_loop()
{
    wait();
    
    current_device_state = 0;
    sw_device_select.write(0);
    sw_burner_temp.write(0);
    sw_fan_speed.write(0);
    sw_oven_func.write(0);
    sw_oven_temp.write(0);
    display_trigger.write(false);
    
    cout << "\nSystem sterowania kuchenka zainicjalizowany" << endl;
    wait(3);
    
    while(true) {
        int choice;
        display_menu();
        cin >> choice;
        
        switch(choice) {
            case 1:
                handle_burner_control();
                break;
            case 2:
                handle_oven_control();
                break;
            case 3:
                handle_fan_control();
                break;
            case 4:
                cout << "Wyswietlam aktualny stan..." << endl;
                display_trigger.write(true);
                wait(1);
                display_trigger.write(false);
                wait(1);
                break;
            case 5:
                cout << "\nZakończenie symulacji..." << endl;
                sc_stop();
                return;
            default:
                cout << "Nieprawidlowy wybor!" << endl;
        }
        
        wait(1);
    }
}
