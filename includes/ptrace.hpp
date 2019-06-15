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

class Ptrace {
public :

    long getData(pid_t pID, uint64_t address);
    bool setData(pid_t pID, uint64_t address, uint64_t value);
    bool continueExec(pid_t pID);
    bool singleStep(pid_t pID);
    siginfo_t getSignalInfo(pid_t pID);

private :


};

#endif //_PTRACE_
