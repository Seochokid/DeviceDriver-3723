#pragma once
#include "device_driver.h"

class Application
{
public:
    Application(DeviceDriver* driver);
    void readAndPrint(long startAddr, long endAddr);
    void writeAll(int value);

protected:
    DeviceDriver* driver;
    const int WRITE_ALL_RANGE = 5;
};