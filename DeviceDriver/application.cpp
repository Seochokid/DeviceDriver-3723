#include <iostream>
#include "application.h"

Application::Application(DeviceDriver* dd) : driver(dd)
{
}

void Application::readAndPrint(long startAddr, long endAddr) 
{
	for (long addr = startAddr; addr < endAddr; addr++) {
        int data = driver->read(addr);
        std::cout << "Address: " << addr << ", Data: " << data << std::endl;
    }
}

void Application::writeAll(int value) 
{
    for (long addr = 0; addr < WRITE_ALL_RANGE; addr++) {
        driver->write(addr, value);
    }
}