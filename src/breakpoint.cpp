#include <iostream>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include "breakpoint.hpp"

Breakpoint::Breakpoint (pid_t pId, std::intptr_t ad, Ptrace & pt)
    :_ptrace(pt),
     _processId(pId),
     _address(ad)
{
}

void Breakpoint::enable()
{
     // std::cout << "in enable\n";
    long data = _ptrace.getData(_processId, _address);
    if (data == -1) {
        return;
    }
    _savedData = static_cast<uint8_t>(data & 0xff);

    uint64_t dataWithInt3 = ((data & ~0xff) | _int3);
    bool ret = _ptrace.setData(_processId, _address, dataWithInt3);
    if (ret) {
        _enabled = true;
        std::cout << "breakpoint set at address 0x" << std::hex << _address << std::endl;
        return;
    }
    std::cout << "breakpoint not set at address 0x" << std::hex << _address << std::endl;
}

void Breakpoint::disable()
{
     // std::cout << "in disable\n";
    long data = _ptrace.getData(_processId, _address);
    if (data != -1) {
        uint64_t dataToReplace = ((data & ~0xff) | _savedData);
        if (_ptrace.setData(_processId, _address, dataToReplace)) {
            _enabled = false;
        }
    }
}

bool Breakpoint::isEnabled() const
{
     // std::cout << "in is enable\n";
    return _enabled;
}

std::intptr_t Breakpoint::getAddress() const
{
     // std::cout << "in getAddress\n";
    return _address;
}
