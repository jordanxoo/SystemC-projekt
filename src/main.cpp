#include <systemc.h>
#include <iostream>
#include<systemc>

// Przykładowy prosty moduł SystemC
SC_MODULE(HelloWorld) {
    SC_CTOR(HelloWorld) {
        SC_THREAD(process);
    }
    
    void process() {
        std::cout << "Hello from SystemC 3.0.1!" << std::endl;
        std::cout << "SystemC version: " << sc_version() << std::endl;
    }
};

int sc_main(int argc, char* argv[]) {
    HelloWorld hello("HelloWorld");
    
    // Uruchom symulację
    sc_start(1, SC_NS);
    
    return 0;
}
