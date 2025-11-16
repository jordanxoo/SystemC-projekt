#ifndef COMMAND_H
#define COMMAND_H

#include <systemc.h>
#include <iostream>

enum Device { // typy urzadzen do sterowania
    BURNER,
    OVEN,
    FAN
};

enum Action { //akcje ktore mozna wykonac
    TURN_ON,
    TURN_OFF,
    SET_TEMP,
    SET_OVEN_FUNCTION,
    SET_FAN_SPEED,
    
};

enum OvenFunction
{
    HEAT_BOTTOM,
    HEAT_TOP,
    HEAT_LEFT,
    HEAT_RIGHT,
    HEAT_ALL,
    TERMOOBIEG
};

struct Command { // pojedyncze polecenie przeslane przez fifo
    Device target_device;
    int    device_id;
    Action action;
    int    value;    

    friend std::ostream& operator<<(std::ostream& os, const Command& cmd) {
        os << "CMD -> Device: ";

        switch(cmd.target_device)
        {
            case BURNER: os << "BURNER"; break;
            case OVEN: os << "OVEN"; break;
            case FAN: os << "FAN"; break;
        }
        os << ", ID: " << cmd.device_id <<", Action: ";
        switch(cmd.action)
        {
            case TURN_ON: os << "TURN_ON"; break;
            case TURN_OFF: os << "TURN_OFF"; break;
            case SET_TEMP: os << "SET_TEMP"; break;
            case SET_FAN_SPEED: os << "SET_FAN_SPEED"; break;
            case SET_OVEN_FUNCTION: os <<"SET_OVEN_FUNCTION"; break;
        }
        os << " ,Value:" << cmd.value;
        return os;
    }
    bool operator==(const Command& other) const {
        return (target_device == other.target_device &&
                device_id == other.device_id &&
                action == other.action &&
                value == other.value);
    }
};

inline void sc_trace(sc_trace_file* tf, const Command& cmd, const std::string& name) {}

#endif // COMMAND_H