#include "testbench.h"
#include "oven.h"

void Testbench::run_tests()
{
    wait(); 
    sw_device_select.write(0);
    sw_burner_temp.write(0);
    sw_fan_speed.write(0);
    sw_oven_func.write(0);
    sw_oven_temp.write(0);
    cout << "@" << sc_time_stamp() << " Testbench: Inicjalizacja zakonczona." << endl;
    wait(3);

    cout << endl << "@" << sc_time_stamp() << " Testbench: Wlaczanie Palnika 1 ===" << endl;
    cout << "@" << sc_time_stamp() << " Testbench: Ustawiam temperature palnika na 3" << endl;
    sw_burner_temp.write(3);
    wait();
    cout << "@" << sc_time_stamp() << " Testbench: Wlaczam palnik 1" << endl;
    sw_device_select.write(0b00010);
    wait();
    cout << "@" << sc_time_stamp() << " Testbench: Wylaczam palnik 1" << endl;
    sw_device_select.write(0);
    wait();

    cout << endl << "@" << sc_time_stamp() << " Testbench: Wlaczanie Piekarnika ===" << endl;
    wait(2);
    cout << "@" << sc_time_stamp() << " Testbench: Ustawiam funkcje 'Termoobieg' i temp. 5" << endl;
    sw_oven_func.write(TERMOOBIEG);
    sw_oven_temp.write(5);
    wait();
    cout << "@" << sc_time_stamp() << " Testbench: Wlaczam piekarnik" << endl;
    sw_device_select.write(0b10000);
    wait(5); 
    cout << "@" << sc_time_stamp() << " Testbench: Zmieniam funkcje na 'Ogrzewanie dol'" << endl;
    sw_oven_func.write(HEAT_BOTTOM);
    wait(5); 
    cout << "@" << sc_time_stamp() << " Testbench: Wylaczam piekarnik" << endl;
    sw_device_select.write(0);
    wait();

    std::cout << std::endl << "@" << sc_time_stamp() << "Testbench: Sterowanie nawiewem ==" << std::endl;
    wait(2);

    std::cout << "@" << sc_time_stamp() << " Testbench: Ustawiam predkosc nawiewu na 2" << std::endl;
    sw_fan_speed.write(2);
    wait(5);

    std::cout << "@" << sc_time_stamp() << " Testbench: Zmniejszam predkosc nawiewiu na 1" << std::endl;
    sw_fan_speed.write(1);
    wait(5);

    std::cout << "@" << sc_time_stamp() << " Testbench: Wylaczam nawiew";
    sw_fan_speed.write(0);
    wait(5);



    
    wait(100);
    cout << endl << "@" << sc_time_stamp() << " Testbench: Zatrzymuje symulacje." << endl;
    sc_stop();
}
