#ifndef _BREAKPOINT_
#define _BREAKPOINT_


#include <sys/types.h>
#include <unistd.h>
#include <cstdint>

#include "ptrace.hpp"

class Breakpoint {
public :

    Breakpoint (std::intptr_t ad, Ptrace & pt);

    bool enable();
    void disable();

    bool isEnabled() const;
    std::intptr_t getAddress() const;

private :

    Ptrace & _ptrace;
    bool          _enabled = false;
    std::intptr_t _address;
    uint8_t       _savedData;
    static uint8_t const _int3 = 0xcc;

};

#endif //_BREAKPOINT_
