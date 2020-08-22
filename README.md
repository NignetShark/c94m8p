# C94-M8P C++ driver
**Work in progress. Not stable.**

## Goal
Have a lightweight and customizable driver to interact with the C94-M8P GPS.
It must give the basic tools like subscribe, poll and ack messages but it will not parse the
content of messages to give an object version of the data. But for fixed messages it's easy
to get fields by casting the payload to a struct.

## Features currently implemented
* Subscription to UBX / NMEA messages
* Change UART port configuration (such as baudrate, protocols  in/out)
* Disable all GPS NMEA messages (without filtering in PRT config)

## Test
See test/default.cpp for basic usage.

## Installation
```
mkdir build
cd build
cmake ..
make
./default # Run example (the C94-M8B must be connected to /dev/ttyACM0 9600B)
```





