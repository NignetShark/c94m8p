// Open, read, write, close.
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdexcept>

#include "utils/serial.hpp"

Serial::Serial() {
    unsigned int index_read_buffer = 0;
    unsigned int index_write_buffer = 0;
}

Serial::~Serial() {
    if(serial_fd >= 0) {
        close(serial_fd);
    }
}

void Serial::connect(char* com, int baudrate) {
    serial_fd = open(com, O_RDWR);

    // Check for errors
    if (serial_fd < 0) {
        #ifdef DEBUG
            printf("Error %i from open: %s\n", errno, strerror(errno));
        #endif
        throw std::runtime_error("Unable to open serial.");
    }

    // Read in existing settings, and handle any error
    if(tcgetattr(serial_fd, &tty) != 0) {
        #ifdef DEBUG
            printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        #endif
        throw std::runtime_error("Unable to read existing settings for serial.");
    }

    tty.c_cflag &= ~PARENB; // Clear parity bit
    tty.c_cflag &= ~CSTOPB; // Clear stop field
    tty.c_cflag |= CS8; // 8 bits per byte
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control
    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    tty.c_lflag &= ~ICANON; // Disable canonical mode
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

    tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;

    // Set input / output baudrate
    cfsetispeed(&tty, baudrate);
    cfsetospeed(&tty, baudrate);

    // Save tty settings, also checking for error
    if (tcsetattr(serial_fd, TCSANOW, &tty) != 0) {
        #ifdef DEBUG
            printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        #endif
        throw std::runtime_error("Unable to save serial settings");
    }
}


int Serial::read_serial(char* buf, int size) {

    // Read bytes. The behaviour of read() (e.g. does it block?,
    // how long does it block for?) depends on the configuration
    // settings above, specifically VMIN and VTIME
    int num_bytes = read(serial_fd, buf, size);
    
    //printf("Buffer (size %d): %s\n", num_bytes, read_buf);

    if (num_bytes < 0) {
        #ifdef DEBUG
            printf("Error reading: %s", strerror(errno));
        #endif
        return -1;
    }
    
    return num_bytes;
}

unsigned char Serial::read_char() {
    if(index_read_buffer == index_write_buffer) {
        if(index_write_buffer == BUFFER_LENGTH) {
            index_write_buffer = 0;
            index_read_buffer = 0;
        }

        int retry = NB_RETRY_WHEN_READING_FAILURE;
        int length = 0;

        while(retry > 0) {
            length = read_serial(&input_buffer[index_read_buffer], BUFFER_LENGTH - index_write_buffer);
            if(length > 0) break;
        }
        if(retry == 0) throw std::runtime_error("Unable to read char.");

        

        index_write_buffer += length;

        //printf("Buffer (size %d) r: %d w: %d\n", length, index_read_buffer, index_write_buffer);
    }
    unsigned char value = input_buffer[index_read_buffer];
    index_read_buffer++;

    return value;
}

uint16_t Serial::read_uint16() {
    return (uint16_t)read_char() | ((uint16_t)read_char() << 8);
}

void Serial::write_serial(unsigned char* buf, int size) {
    write(serial_fd, buf, size);
    /*for(int i = 0; i < size; i++) {
        printf("0x%x ", (unsigned char)buf[i]);
    }*/
}

void Serial::write_char(unsigned char value) {
    write(serial_fd, &value, 1);
    //printf("0x%x ", value);
}

void Serial::write_uint16(uint16_t value) {
    write(serial_fd, &value, 2);
    //printf("0x%x ", value);
}
