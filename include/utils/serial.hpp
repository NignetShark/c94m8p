#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <termios.h>
#include <stdint.h>

#define BUFFER_LENGTH 256
#define NB_RETRY_WHEN_READING_FAILURE 3

class Serial {
    private:
        int serial_fd;
        struct termios tty;

        unsigned int index_read_buffer;
        unsigned int index_write_buffer;
        char input_buffer[BUFFER_LENGTH];

        int read_serial(char* buf, int size);
    

    public:
        Serial();
        ~Serial();

        void connect(char* com, int baudrate);

        unsigned char read_char();
        uint16_t read_uint16();

        void write_serial(unsigned char* buf, int size);
        void write_char(unsigned char value);
        void write_uint16(uint16_t value);
};

#endif