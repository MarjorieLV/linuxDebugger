#include <iostream>
#include <cstring>
#include <cerrno>
#include "ptrace.hpp"

long Ptrace::getData(pid_t pID, uint64_t address)
{
     // std::cout << "in get ptrace data\n";
     // std::cout << "pid : " << std::hex << pID 
               // << ", address : " << std::hex << address << std::endl;
    long value = ptrace(PTRACE_PEEKDATA, pID, address, nullptr);
    if (value == -1) {
        std::cout << "error peekData : " << std::strerror(errno) << std::endl;
    }
     // std::cout << "Value found : " <<  std::hex << value << std::endl;
    return value;
}

bool Ptrace::setData(pid_t pID, uint64_t address, uint64_t value)
{
     // std::cout << "in set ptrace data\n";
     // std::cout << "pid : " << std::hex << pID
               // << ", address : " << std::hex << address
               // << ", value : " <<  std::hex << value << std::endl;
    long ret = ptrace(PTRACE_POKEDATA, pID, address, value);
    if (ret == -1) {
        std::cout << "error poke data : " << std::strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool Ptrace::continueExec(pid_t pID)
{
     // std::cout << "in ptrace continueExec\n";
     // std::cout << "pid : " << std::hex << pID << std::endl;
    long ret = ptrace(PTRACE_CONT, pID, nullptr, nullptr);
    if (ret == -1) {
        std::cout << "error continueExec : " << std::strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool Ptrace::singleStep(pid_t pID)
{
     // std::cout << "in ptrace singleStep\n";
     // std::cout << "pid : " << std::hex << pID << std::endl;
    long ret = ptrace(PTRACE_SINGLESTEP, pID, nullptr, nullptr);
    if (ret == -1) {
        std::cout << "error single step : " << std::strerror(errno) << std::endl;
        return false;
    }
    return true;
}

siginfo_t Ptrace::getSignalInfo(pid_t pID)
{
    siginfo_t info;
    int ret = ptrace(PTRACE_GETSIGINFO, pID, nullptr, &info);
    if (ret == -1) {
        std::cout << "error get signal info : " << std::strerror(errno) << std::endl;
    }
    return info;
}
