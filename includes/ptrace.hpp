#ifndef _PTRACE_
#define _PTRACE_


#include <sys/ptrace.h>
#include <cstdint>
#include <cerrno>
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include "dwarf++.hh"
#include "registers.hpp"

class Ptrace : public dwarf::expr_context
{
public :

    Ptrace(pid_t pID, Register const & reg);
    long      getData(uint64_t address);
    bool      setData(uint64_t address, uint64_t value);
    bool      continueExec();
    bool      singleStep();
    siginfo_t getSignalInfo();
    dwarf::taddr reg (unsigned regnum) override;
    dwarf::taddr pc() override;
    dwarf::taddr deref_size (dwarf::taddr address, unsigned int size) override;
private :

    pid_t _processId;
    Register _register;

};

#endif //_PTRACE_
