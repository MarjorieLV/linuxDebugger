#ifndef _DEBUGGER_
#define _DEBUGGER_

#include <string>
#include <vector>
#include <unordered_map>

#include "dwarf++.hh"
#include "elf++.hh"

#include "breakpoint.hpp"
#include "registers.hpp"
#include "ptrace.hpp"

class Debugger {
public :

    Debugger (std::string const & pName, pid_t pId);
    void run();


private :

    enum class CMD {
        BREAK,
        CONTINUE,
        DUMP,
        FINISH,
        MEMORY,
        NEXT,
        READ,
        REGISTER,
        STEP,
        UNKNOWN,
        WRITE,
    };

    Ptrace _ptrace;
    Register _register;
    std::string const& _programName;
    pid_t _processId;
    dwarf::dwarf _dwarf;
    elf::elf _elf;
    uint64_t const retAddOffset = 8;
    std::unordered_map<std::string, CMD> _validCmd
        {
            {"b", CMD::BREAK},
            {"c", CMD::CONTINUE},
            {"dump", CMD::DUMP},
            {"finish", CMD::FINISH},
            {"mem", CMD::MEMORY},
            {"n", CMD::NEXT},
            {"read", CMD::READ},
            {"reg", CMD::REGISTER},
            {"s", CMD::STEP},
            {"write", CMD::WRITE},
        };
    std::unordered_map<std::intptr_t, Breakpoint> _breakpointsByAddress;

    void     _continueExec();
    void     _dumpRegisters();
    uint64_t _getPc();
    void     _handleBreakCommand(std::vector<std::string> args);
    void     _handleBreakpoint();
    void     _handleCommand(char* line);
    void     _handleMemoryCommand(std::vector<std::string> args);
    void     _handleRegisterCommand(std::vector<std::string> args);
    void     _handleSigtrap(siginfo_t info);
    CMD      _isCmd(std::string const & cmd);
    void     _printSource(std::string const& fileName,unsigned int line,unsigned int linesContext=2);
    uint64_t _readMemory(uint64_t address);
    void     _removeBreakpoint(std::intptr_t address);
    void     _setBreakpoint(std::intptr_t address);
    void     _setPc(uint64_t pc);
    void     _singleStep();
    void     _singleStepWithBreakpointCheck();
    void     _stepIn();
    void     _stepOut();
    void     _stepOver();
    void     _waitForSignal();
    void     _writeMemory(uint64_t address, uint64_t value);

    dwarf::die                  _getFuncFromPC(uint64_t pc);
    dwarf::line_table::iterator _getLineEntryFromPc(uint64_t pc);
};

#endif //_DEBUGGER_
