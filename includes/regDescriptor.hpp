

#ifndef _REGDESCRIPTOR_
#define _REGDESCRIPTOR_

#include <string>
enum class  REGTYPE {
    rax = 0,
    rbx = 1,
    rcx = 2,
    rdx = 3,
    rdi = 4,
    rsi = 5,
    rbp = 6,
    rsp = 7,
    r8 = 8,
    r9 = 9,
    r10 = 10,
    r11 = 11,
    r12 = 12,
    r13 = 13,
    r14 = 14,
    r15 = 15,
    rip = 16,
    rflags = 17,
    cs = 18,
    orig_rax = 19,
    fs_base = 20,
    gs_base = 21,
    fs = 22,
    gs = 23,
    ss = 24,
    ds = 25,
    es = 26,
    unknown
};

struct RegDescriptor {
    REGTYPE _reg;
    int _dwarfRegNum;
    std::string _name;
};

#endif //_REGDESCRIPTOR_

