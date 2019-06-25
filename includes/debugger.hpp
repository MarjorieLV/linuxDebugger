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
#include "symbol.hpp"

class Debugger {
public :

    Debugger (std::string const & pName, pid_t pId);
    void run();


private :

    enum class CMD {
        BACKTRACE,
        BREAK,
        CONTINUE,
        DUMP,
        FINISH,
        MEMORY,
        NEXT,
        READ,
        REGISTER,
        STEP,
        SYMBOL,
        UNKNOWN,
        VARIABLES,
        WRITE,
    };

    Register _register;
    Ptrace _ptrace;
    std::string const& _programName;
    pid_t _processId;
    dwarf::dwarf _dwarf;
    elf::elf _elf;
    uint64_t const _retAddOffset = 8;
    std::unordered_map<std::string, CMD> _validCmd
        {
            {"bt", CMD::BACKTRACE},
            {"b", CMD::BREAK},
            {"c", CMD::CONTINUE},
            {"dump", CMD::DUMP},
            {"finish", CMD::FINISH},
            {"mem", CMD::MEMORY},
            {"n", CMD::NEXT},
            {"read", CMD::READ},
            {"reg", CMD::REGISTER},
            {"s", CMD::STEP},
            {"symbol", CMD::SYMBOL},
            {"variables", CMD::VARIABLES},
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
    void     _handleSymbol(std::vector<std::string> args);
    CMD      _isCmd(std::string const & cmd);
    void     _printBacktrace();
    void     _printSource(std::string const& fileName,unsigned int line,unsigned int linesContext=2);
    uint64_t _readMemory(uint64_t address);
    void     _readVariables();
    void     _removeBreakpoint(std::intptr_t address);
    void     _setBreakpoint(std::intptr_t address);
    void     _setBreakpointAtFunc(std::string const & name);
    void     _setBreakpointAtSourceLine(std::string const & file, unsigned int line);
    void     _setPc(uint64_t pc);
    void     _singleStep();
    void     _singleStepWithBreakpointCheck();
    void     _stepIn();
    void     _stepOut();
    void     _stepOver();
    void     _waitForSignal();
    void     _writeMemory(uint64_t address, uint64_t value);
    std::vector<Symbol> _findSymbols(std::string const & name);
    dwarf::die                  _getFuncFromPC(uint64_t pc);
    dwarf::line_table::iterator _getLineEntryFromPc(uint64_t pc);
};

#endif //_DEBUGGER_
