
#ifndef _REGISTERS_
#define _REGISTERS_

#include <string>
#include <unordered_map>

#include "regDescriptor.hpp"

class Register
{
public :

    Register(){};
    uint64_t getRegisterValue(pid_t processID, REGTYPE reg);
    void setRegisterValue(pid_t processID, REGTYPE reg, uint64_t value);
    uint64_t getRegisterValueFromDwarfReg(pid_t processID, int regNum);
    std::string getRegisterName(REGTYPE reg);
    REGTYPE getRegisterByName(std::string const & name);
    std::unordered_map<REGTYPE, RegDescriptor> const & getRegistersDescriptor();

private :

    size_t const _totalReg = 27;
    std::unordered_map<REGTYPE, RegDescriptor> const  _registerDescriptors =
        {
            {REGTYPE::r15, {REGTYPE::r15, 15, "r15" }},
            {REGTYPE::r14, {REGTYPE::r14, 14, "r14" }},
            {REGTYPE::r13, {REGTYPE::r13, 13, "r13" }},
            {REGTYPE::r12, {REGTYPE::r12, 12, "r12" }},
            {REGTYPE::rbp, {REGTYPE::rbp, 6, "rbp" }},
            {REGTYPE::rbx, {REGTYPE::rbx, 3, "rbx" }},
            {REGTYPE::r11, {REGTYPE::r11, 11, "r11" }},
            {REGTYPE::r10, {REGTYPE::r10, 10, "r10" }},
            {REGTYPE::r9, {REGTYPE::r9, 9, "r9" }},
            {REGTYPE::r8, {REGTYPE::r8, 8, "r8" }},
            {REGTYPE::rax, {REGTYPE::rax, 0, "rax" }},
            {REGTYPE::rcx, {REGTYPE::rcx, 2, "rcx" }},
            {REGTYPE::rdx, {REGTYPE::rdx, 1, "rdx" }},
            {REGTYPE::rsi, {REGTYPE::rsi, 4, "rsi" }},
            {REGTYPE::rdi, {REGTYPE::rdi, 5, "rdi" }},
            {REGTYPE::orig_rax, {REGTYPE::orig_rax, -1, "orig_rax" }},
            {REGTYPE::rip, {REGTYPE::rip, -1, "rip" }},
            {REGTYPE::cs, {REGTYPE::cs, 51, "cs" }},
            {REGTYPE::rflags, {REGTYPE::rflags, 49, "eflags" }},
            {REGTYPE::rsp, {REGTYPE::rsp, 7, "rsp" }},
            {REGTYPE::ss, {REGTYPE::ss, 52, "ss" }},
            {REGTYPE::fs_base, {REGTYPE::fs_base, 58, "fs_base" }},
            {REGTYPE::gs_base, {REGTYPE::gs_base, 59, "gs_base" }},
            {REGTYPE::ds, {REGTYPE::ds, 53, "ds" }},
            {REGTYPE::es, {REGTYPE::es, 50, "es" }},
            {REGTYPE::fs, {REGTYPE::fs, 54, "fs" }},
            {REGTYPE::gs, {REGTYPE::gs, 55, "gs" }}
        };
};

#endif //_REGISTERS_
