#include <iostream>
#include <cstring>
#include <cerrno>
#include "ptrace.hpp"
#include <sys/user.h>

Ptrace::Ptrace(pid_t _processId, Register const & reg)
    :_processId(_processId),
     _register(reg){}

long Ptrace::getData(uint64_t address)
{
     // std::cout << "in get ptrace data\n";
    long value = ptrace(PTRACE_PEEKDATA, _processId, address, nullptr);
    if (value == -1) {
        std::cout << "error peekData : " << std::strerror(errno) << std::endl;
    }
     // std::cout << "Value found : " <<  std::hex << value << std::endl;
    return value;
}

bool Ptrace::setData(uint64_t address, uint64_t value)
{
     // std::cout << "in set ptrace data\n";
    long ret = ptrace(PTRACE_POKEDATA, _processId, address, value);
    if (ret == -1) {
        std::cout << "error poke data : " << std::strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool Ptrace::continueExec()
{
     // std::cout << "in ptrace continueExec\n";
    long ret = ptrace(PTRACE_CONT, _processId, nullptr, nullptr);
    if (ret == -1) {
        std::cout << "error continueExec : " << std::strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool Ptrace::singleStep()
{
     // std::cout << "in ptrace singleStep\n";
    long ret = ptrace(PTRACE_SINGLESTEP, _processId, nullptr, nullptr);
    if (ret == -1) {
        std::cout << "error single step : " << std::strerror(errno) << std::endl;
        return false;
    }
    return true;
}

siginfo_t Ptrace::getSignalInfo()
{
    siginfo_t info;
    long ret = ptrace(PTRACE_GETSIGINFO, _processId, nullptr, &info);
    if (ret == -1) {
        std::cout << "error get signal info : " << std::strerror(errno) << std::endl;
    }
    return info;
}

dwarf::taddr Ptrace::reg (unsigned regnum) {
    return _register.getRegisterValueFromDwarfReg(_processId, regnum);
}

dwarf::taddr Ptrace::pc() {
    user_regs_struct regs;
    long ret = ptrace(PTRACE_GETREGS, _processId, nullptr, &regs);
    if (ret == -1) {
        std::cout << "error pc override : " << std::strerror(errno) << std::endl;
    }
    return regs.rip;
}

dwarf::taddr Ptrace::deref_size (dwarf::taddr address, unsigned int) {
    //TODO take into account size
    return getData(address);
}
