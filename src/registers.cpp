#include <iostream>
#include <bits/stdc++.h> 
#include <sys/ptrace.h>
#include <sys/user.h>
#include <cerrno>
#include <cstring>

#include "registers.hpp"

uint64_t Register::getRegisterValue(pid_t processID, REGTYPE reg)
{
     // std::cout << "in get register value\n";
    user_regs_struct regs;
    int ret = ptrace(PTRACE_GETREGS, processID, nullptr, &regs);

    if(ret == -1) {
        std::cout << "error getRegs : " << std::strerror(errno) << std::endl;
    }

    int index = static_cast<int>(reg);
    uint64_t value = *(reinterpret_cast<uint64_t*>(&regs) + index);
     // std::cout << "reg " << _registerDescriptors.find(reg)->second._name 
     //           << " value is " << std::hex << value << std::endl;
    return value;
}

void Register::setRegisterValue(pid_t processID, REGTYPE reg, uint64_t value)
{
     // std::cout << "in set register value\n";
     // std::cout << "reg " << _registerDescriptors.find(reg)->second._name 
               // << " value to set is " << std::hex << value << std::endl;
    user_regs_struct regs;
    int ret = ptrace(PTRACE_GETREGS, processID, nullptr, &regs);
    if(ret == -1) {
        std::cout << "error getRegs : " << std::strerror(errno) << std::endl;
    }
    int index = static_cast<int>(reg);
    uint64_t* address = reinterpret_cast<uint64_t*>(&regs) + index;
    *address = value;

    ret = ptrace(PTRACE_SETREGS, processID, nullptr, &regs);
    if(ret == -1) {
        std::cout << "error setRegs : " << std::strerror(errno) << std::endl;
    }
}

uint64_t Register::getRegisterValueFromDwarfReg(pid_t processID, int regNum)
{
     // std::cout << "in get register value from dwarf reg\n";
    for (auto reg : _registerDescriptors) {
        if (reg.second._dwarfRegNum == regNum) {
             std::cout << "reg " << reg.second._name << std::endl;
            return getRegisterValue(processID, reg.second._reg);
        }
    }
    // std::cout << "unknown dwarf register\n";
    throw std::out_of_range("unknown dwarf register\n");
}

std::string Register::getRegisterName(REGTYPE reg)
{
     // std::cout << "in get register name\n";
    auto it = _registerDescriptors.find(reg);
    if (it == _registerDescriptors.end()) {
        return "unknown register";
    }
    return it->second._name;
}

REGTYPE Register::getRegisterByName(std::string const & name)
{
     // std::cout << "in get register by name\n";
    for (auto reg : _registerDescriptors) {
        if (reg.second._name == name) {
            return reg.second._reg;
        }
    }
    std::cout << "unknown register\n";
    return REGTYPE::unknown;
}


std::unordered_map<REGTYPE, RegDescriptor> const & Register::getRegistersDescriptor()
{
     // std::cout << "in get register descriptor\n";
    return _registerDescriptors;
}
