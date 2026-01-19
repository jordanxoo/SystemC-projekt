#ifndef COMMAND_CHANNEL_H
#define COMMAND_CHANNEL_H


#include<systemc.h>
#include"command.h"

class command_write_if : virtual public sc_interface
{ 
    public:
        

    virtual void write_command(const Command& cmd) = 0;
};

class command_read_if : virtual public sc_interface
{
    public:
    virtual Command read_command()  = 0;
};

class CommandChannel : public sc_channel, public command_write_if, public command_read_if
{
    public:
    SC_CTOR(CommandChannel) : internal_fifo("internal_fifo",16) {};

    void write_command(const Command& cmd)
    {
        internal_fifo.write(cmd);
    }
    Command read_command()
    {
        return internal_fifo.read();
    }

    private:
    sc_fifo<Command> internal_fifo;
};

#endif //COMMAND_CHANNEL_H
