#include <stdexcept>
#include "device_driver.h"

DeviceDriver::DeviceDriver(FlashMemoryDevice* hardware) : m_hardware(hardware)
{
}

int DeviceDriver::read(long address)
{
    int data = m_hardware->read(address);
    for (int turn = 1; turn < TEST_TRY_COUNT; ++turn)
    {
        if (m_hardware->read(address) != data) {
            throw std::runtime_error("ReadFailException");
        }
    }
    return data;
}

void DeviceDriver::write(long address, int data)
{
    int currentValue = m_hardware->read(address);
    if (currentValue != 0xFF) {
        throw std::runtime_error("WriteFailException");
    }
	m_hardware->write(address, (unsigned char)data);
}