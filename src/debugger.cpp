#include <iostream>
#include <fstream>
#include <iomanip>
#include <sys/types.h>
#include <sys/wait.h>
#include <boost/algorithm/string.hpp>
#include <cstring>
#include <vector>
#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>
#include "linenoise.h"
#include "dwarf++.hh"
#include "debugger.hpp"
#include "registers.hpp"
#include "breakpoint.hpp"

Debugger::Debugger (std::string const & pName, pid_t pId)
    :_ptrace(pId, _register),
     _programName(pName),
     _processId(pId)
{
    int fd = open(_programName.c_str(), O_RDONLY);
    if (fd == -1) {
        std::cout << "error opening program " << _programName
                  << " : " << std::strerror(errno) << std::endl;
        return;
    }
    _elf = elf::elf{elf::create_mmap_loader(fd)};
    _dwarf = dwarf::dwarf{dwarf::elf::create_loader(_elf)};
};

void Debugger::run ()
{
    int waitStatus;
    int options = 0;
    waitpid(_processId, &waitStatus, options);

    char* line = nullptr;
    while ((line = linenoise("Dbg> ")) != nullptr
           && line[0] != 'q') {
        _handleCommand(line);
        linenoiseHistoryAdd(line);
        linenoiseHistoryFree();
    }
}


void Debugger::_handleCommand(char* l)
{
    std::string line(l);
    std::vector<std::string> args;
    boost::split(args, line, boost::is_any_of(" "));

    CMD cmd = _isCmd(args[0]);

    if(cmd == CMD::CONTINUE) {
        _continueExec();
    }
    else if (cmd == CMD::BREAK) {
        _handleBreakCommand(args);
    }
    else if (cmd == CMD::REGISTER) {
        _handleRegisterCommand(args);
    }
    else if (cmd == CMD::MEMORY) {
        _handleMemoryCommand(args);
    }
    else if (cmd == CMD::STEP) {
        _stepIn();
    }
    else if (cmd == CMD::NEXT) {
        _stepOver();
    }
    else if (cmd == CMD::FINISH) {
        _stepOut();
    }
    else if (cmd == CMD::SYMBOL) {
        _handleSymbol(args);
    }
    else if (cmd == CMD::BACKTRACE) {
        _printBacktrace();
    }
    else if (cmd == CMD::VARIABLES) {
        _readVariables();
    }
    else if (cmd == CMD::UNKNOWN){
     std::cout << "unknown command" << std::endl;
    }
}

void Debugger::_handleRegisterCommand(std::vector<std::string> args)
{
    // std::cout << "in Register command\n";
    if (args.size() < 2) {
        std::cerr << "error, missing arguments for action on register\n";
        return;
    }
    CMD action = _isCmd(args[1]);
    if (action == CMD::DUMP) {
        // std::cout << "in Dump action\n";
        _dumpRegisters();
    }
    else if (action == CMD::READ) {
        // std::cout << "in read action\n";
        if (args.size() < 3) {
            std::cerr << "missing arguments\n";
            return;
        }
        REGTYPE reg = _register.getRegisterByName(args[2]);
        std::cout << _register.getRegisterValue(_processId, reg) << std::endl;
    }
    else if (action == CMD::WRITE){
        // std::cout << "in write action\n";
        if (args.size() < 4) {
            std::cerr << "missing arguments\n";
            return;
        }
        REGTYPE reg = _register.getRegisterByName(args[2]);
        std::string strValue = args[3];
        if (strValue.compare(0, 2, "0x") == 0) {
            strValue = strValue.substr(2);
        }
        uint64_t value = std::stol(strValue, 0, 16);
        _register.setRegisterValue(_processId, reg, value);
    }
}

void Debugger::_handleMemoryCommand(std::vector<std::string> args)
{
    // std::cout << "in memory command\n";
    if (args.size() < 3) {
        std::cerr << "error, arguments missing\n";
        return;
    }
    CMD action = _isCmd(args[1]);
    std::string address = args[2];
    if (address.compare(0, 2, "0x") == 0) {
        address = address.substr(2);
    }
    uint64_t addressValue = std::stol(address, 0, 16);
    if (action == CMD::READ) {
        // std::cout << "in read action\n";
        std::cout << std::hex << _readMemory(addressValue) << std::endl;
    }
    else if (action == CMD::WRITE) {
        // std::cout << "in write action\n";
        if (args.size() < 4) {
            std::cerr << "error, arguments missing\n";
            return;
        }
        std::string strValue = args[3];
        if (strValue.compare(0, 2, "0x") == 0) {
            strValue = strValue.substr(2);
        }
        uint64_t value = std::stol(strValue, 0, 16);
        _writeMemory(addressValue, value);
    }
}

void Debugger::_handleBreakCommand(std::vector<std::string> args)
{
    // std::cout << "in break command\n";
    if (args.size() > 1) {
        if (args[1].compare(0, 2, "0x") == 0){
            std::string strAdress = args[1];
            strAdress = strAdress.substr(2);
            std::intptr_t address = std::stol(strAdress, 0, 16);
            _setBreakpoint(address);
        }
        else if (args[1].find(':') != std::string::npos) {
            std::vector<std::string> data;
            boost::split(data, args[1], boost::is_any_of(":"));
            _setBreakpointAtSourceLine(data[0],std::stoi(data[1]));
        }
        else {
            _setBreakpointAtFunc(args[1]);
        }
    }
    else {
        std::cerr << "error, missing address for breakpoint\n";
    }
}

Debugger::CMD Debugger::_isCmd(std::string const &cmd)
{
    // std::cout << "in isCmd\n";
    auto pair = _validCmd.find(cmd);
    if (pair == _validCmd.end()) {
        return CMD::UNKNOWN;
    }
    return pair->second;
}

void Debugger::_continueExec()
{
    // std::cout << "in continueExec \n";
    _handleBreakpoint();
    if (_ptrace.continueExec()) {
        _waitForSignal();
    }
}

void Debugger::_setBreakpoint(std::intptr_t address)
{
    // std::cout << "in isCmd\n";
    Breakpoint bp(address, _ptrace);
    if (!bp.enable()) {

        std::cout << "FAILED : Num of breakpoint set : " << _breakpointsByAddress.size() << std::endl;
        return ;
    }
    _breakpointsByAddress.emplace(address, std::move(bp));
    std::cout << "Number of breakpoint set : " << _breakpointsByAddress.size() << std::endl;
}

void Debugger::_dumpRegisters()
{
    // std::cout << "in dumpRegisters\n";
    std::unordered_map<REGTYPE, RegDescriptor> const &regs = _register.getRegistersDescriptor();

    for (const auto& rd : regs) {
        std::cout << rd.second._name << " 0x"
                  << std::setfill('0') << std::setw(16) << std::hex
                  << _register.getRegisterValue(_processId, rd.second._reg) << std::endl;
    }
}

uint64_t Debugger::_readMemory(uint64_t address)
{
    // std::cout << "in readMemory\n";
    return _ptrace.getData(address);
}
void Debugger::_writeMemory(uint64_t address, uint64_t value)
{
    // std::cout << "in writeMemory\n";
    _ptrace.setData(address, value);
}

uint64_t Debugger::_getPc()
{
    return _register.getRegisterValue(_processId, REGTYPE::rip);
}

void Debugger::_setPc(uint64_t pc)
{
    // std::cout << "in setPc\n";
    _register.setRegisterValue(_processId, REGTYPE::rip, pc);
}

void Debugger::_handleBreakpoint()
{
    std::cout << "in handle Breakpoint\n";
    auto it = _breakpointsByAddress.find(_getPc());
    if (it == _breakpointsByAddress.end()) {
        return;
    }

    Breakpoint & breakpoint = it->second;
    if (breakpoint.isEnabled()) {
        breakpoint.disable();
        if (_ptrace.singleStep()) {
            _waitForSignal();
            breakpoint.enable();
        }
    }
}

void Debugger::_waitForSignal()
{
    // std::cout << "in wait for signal\n";
    int waitStatus;
    int options = 0;
    waitpid(_processId, &waitStatus, options);

    siginfo_t siginfo = _ptrace.getSignalInfo();

    if (siginfo.si_signo == SIGTRAP) {
        _handleSigtrap(siginfo);
    }
    else if (siginfo.si_signo == SIGSEGV) {
        std::cerr << "Segfault : " << siginfo.si_code << std::endl;
    }
    else {
        std::cout << "Signal received : " << strsignal(siginfo.si_signo) << std::endl;
    }

}

dwarf::die Debugger::_getFuncFromPC(uint64_t pc)
{
    uint64_t pcEnd = pc & 0xfff;
    for (const dwarf::compilation_unit &compUnit : _dwarf.compilation_units()) {
        if (die_pc_range(compUnit.root()).contains(pcEnd)) {
            for (const dwarf::die& die : compUnit.root()) {
                if (die.tag == dwarf::DW_TAG::subprogram) {
                    try {
                        dwarf::rangelist const & range = die_pc_range(die);
                        if (range.contains(pcEnd)) {
                            return die;
                        }
                    }
                    catch (const std::exception& e) {
                        std::cerr << "ERROR : " <<  e.what() << std::endl;
                    }
                }
            }
        }
    }

    throw std::out_of_range{"Cannot find function"};
}

dwarf::line_table::iterator Debugger::_getLineEntryFromPc(uint64_t pc) {
    uint64_t pcEnd = pc & 0xfff;
    for (const dwarf::compilation_unit &compUnit : _dwarf.compilation_units()) {
        if (die_pc_range(compUnit.root()).contains(pcEnd)) {
            dwarf::line_table const &lineTable = compUnit.get_line_table();
            dwarf::line_table::iterator it = lineTable.find_address(pcEnd);
            if (it == lineTable.end()) {
                throw std::out_of_range{"Cannot find line entry"};
            }
            else {
                return it;
            }
        }
    }

    throw std::out_of_range{"End of search, cannot find line entry"};
}

void Debugger::_printSource(std::string const& fileName, unsigned int line,unsigned int linesContext)
{
    std::ifstream file(fileName);
    unsigned int start = 1;
    if (line > linesContext) {
        start = line - linesContext;
    }
    unsigned int end = line + linesContext + 1;
    if (line < linesContext) {
        end += linesContext - line;
    }

    std::string buff;
    unsigned int curr = 1;
    while (curr != start
           && std::getline(file, buff)) {
        curr++;
    }
    if (curr == line) {
        std::cout << "> ";
    }
    else {
        std::cout << " ";
    }

    while (curr <= end
           && std::getline(file, buff)) {
        std::cout << buff << std::endl;
        curr++;
        if (curr == line) {
            std::cout << "> ";
        }
        else {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
}

void Debugger::_handleSigtrap(siginfo_t info)
{
    if (info.si_code == SI_KERNEL
        || info.si_code == TRAP_BRKPT) {
        uint64_t pc = _getPc() - 1;
        _setPc(pc);
        std::cout << "Hit breakpoint at address 0x" << std::hex << pc << std::endl;
        dwarf::line_table::iterator lineEntry = _getLineEntryFromPc(pc);
        _printSource(lineEntry->file->path, lineEntry->line);
        return;
    }
    else if (info.si_code == TRAP_TRACE) {
        return;
    }
    std::cout << "Unknown SIGTRAP code " << std:: hex << info.si_code << std::endl;
}

void Debugger::_singleStep()
{
    _ptrace.singleStep();
    _waitForSignal();
}

void Debugger::_singleStepWithBreakpointCheck()
{
    auto bpIt = _breakpointsByAddress.find(_getPc());
    if (bpIt != _breakpointsByAddress.end()) {
        _handleBreakpoint();
    }
    else {
        _singleStep();
    }
}

void Debugger::_stepOut()
{
    uint64_t framePtr = _register.getRegisterValue(_processId, REGTYPE::rbp);
    uint64_t returnAdd = _readMemory(framePtr + _retAddOffset);

    bool removeBreakpoint = false;
    if (_breakpointsByAddress.find(returnAdd) == _breakpointsByAddress.end()) {
        _setBreakpoint(returnAdd);
        removeBreakpoint = true;
    }
    _continueExec();

    if (removeBreakpoint) {
        _removeBreakpoint(returnAdd);
    }
}

void Debugger::_removeBreakpoint(std::intptr_t address)
{
    auto bpIt = _breakpointsByAddress.find(address);
    if (bpIt == _breakpointsByAddress.end()) {
        std::cout << "No breakpoint found at 0x" << std::hex << address << std::endl;
        return;
    }
    if (bpIt->second.isEnabled()) {
        bpIt->second.disable();
    }
    _breakpointsByAddress.erase(address);
}

void Debugger::_stepIn()
{

    unsigned int const line = _getLineEntryFromPc(_getPc())->line;

    while (_getLineEntryFromPc(_getPc())->line == line) {
        _singleStepWithBreakpointCheck();
    }

    dwarf::line_table::iterator lineEntry = _getLineEntryFromPc(_getPc());
    _printSource(lineEntry->file->path,lineEntry->line);
}

void Debugger::_stepOver()
{
    dwarf::die func = _getFuncFromPC(_getPc());
    uint64_t funcStart = at_low_pc(func);
    uint64_t funcEnd = at_high_pc(func);

    dwarf::line_table::iterator lineEntry = _getLineEntryFromPc(funcStart);
    dwarf::line_table::iterator startLine = _getLineEntryFromPc(_getPc());

    std::vector<std::intptr_t> toDelete;

    while (lineEntry->address < funcEnd) {
        if (lineEntry->address != startLine->address
            && !_breakpointsByAddress.count(lineEntry->address)) {
            _setBreakpoint(lineEntry->address);
            toDelete.push_back(lineEntry->address);
        }
        lineEntry++;
    }

    uint64_t framePtr = _register.getRegisterValue(_processId, REGTYPE::rbp);
    uint64_t returnAdd = _readMemory(framePtr + _retAddOffset);
    if (!_breakpointsByAddress.count(returnAdd)) {
        _setBreakpoint(returnAdd);
        toDelete.push_back(returnAdd);
    }

    _continueExec();
    for(std::intptr_t add : toDelete) {
        _removeBreakpoint(add);
    }
}

void Debugger::_setBreakpointAtFunc(std::string const & name)
{
    for (const dwarf::compilation_unit &compUnit : _dwarf.compilation_units()) {
        for (const dwarf::die& die : compUnit.root()) {
            if (die.has(dwarf::DW_AT::name)
                && at_name(die) == name) {
                uint64_t lowPc = at_low_pc(die);
                dwarf::line_table::iterator entry = _getLineEntryFromPc(lowPc);
                ++entry; //skip prologue
                _setBreakpoint(entry->address);
            }
        }
    }
}

bool isSuffix(const std::string& s, const std::string& of) {
    if (s.size() > of.size()) return false;
    auto diff = of.size() - s.size();
    return std::equal(s.begin(), s.end(), of.begin() + diff);
}

void Debugger::_setBreakpointAtSourceLine(std::string const & file, unsigned int line)
{
    for (const dwarf::compilation_unit &compUnit : _dwarf.compilation_units()) {
        if (isSuffix(file, at_name(compUnit.root()))) {
            dwarf::line_table const & lineTable = compUnit.get_line_table();
            for (dwarf::line_table::entry const & entry : lineTable) {
                if (entry.is_stmt
                    && entry.line == line) {
                    _setBreakpoint(entry.address);
                    return;
                }
            }
        }
        else {
            std::cout << "is not suffix\n";
        }
    }
}

std::vector<Symbol> Debugger::_findSymbols(std::string const & name)
{
    std::vector<Symbol> symbols;

    for (elf::section const &section : _elf.sections()) {
        if (section.get_hdr().type != elf::sht::symtab 
            && section.get_hdr().type != elf::sht::dynsym)
            continue;

        for (elf::sym symbol : section.as_symtab()) {
            if (symbol.get_name() == name) {
                elf::Sym<> const &d = symbol.get_data();
                Symbol newSymbol (d.type(), d.value);
                symbols.push_back(newSymbol);
            }
        }
    }

    return symbols;
}

void Debugger::_handleSymbol(std::vector<std::string> args)
{
    if (args.size() < 2) {
        std::cout << "missing arguments\n";
        return;
    }
    std::vector<Symbol> symbols = _findSymbols(args[1]);
    if (symbols.empty()) {
        std::cerr << "Error, no symbols found\n";
        return;
    }
    for (Symbol& s : symbols) {
        std::cout << s.getName() << ", "
                  << s.getSymbolName(s.getType()) << " at 0x"
                  << s.getAddress() << std::endl;
    }
}

void Debugger::_printBacktrace()
{
    auto outFrame = [frameNumber = 0] (auto&& func) mutable {
        std::cout << "frame #" << frameNumber++ << ": 0x" << dwarf::at_low_pc(func)
                  << ' ' << dwarf::at_name(func) << std::endl;
    };

    auto currFunc = _getFuncFromPC(_getPc());
    outFrame(currFunc);

    auto framPtr = _register.getRegisterValue(_processId, REGTYPE::rbp);
    auto retAdd = _readMemory(framPtr + _retAddOffset);

    while (dwarf::at_name(currFunc) != "main") {
        currFunc = _getFuncFromPC(retAdd);
        outFrame(currFunc);
        framPtr = _readMemory(framPtr);
        retAdd = _readMemory(framPtr+8);
    }
}

void Debugger::_readVariables() {
    dwarf::die func = _getFuncFromPC(_getPc());
    for (const dwarf::die& die : func) {
        if (die.tag == dwarf::DW_TAG::variable) {
            dwarf::value locVal = die[dwarf::DW_AT::location];
            //only supports exprlocs for now
            if (locVal.get_type() == dwarf::value::type::exprloc) {
                dwarf::expr_result result = locVal.as_exprloc().evaluate(&_ptrace);
                if (result.location_type == dwarf::expr_result::type::address) {
                    uint64_t value = _readMemory(result.value);
                    std::cout << at_name(die)
                              << " (0x" << std::hex << result.value
                              << ") = " << value << std::endl;
                }
                if (result.location_type == dwarf::expr_result::type::reg) {
                    uint64_t value = _register.getRegisterValueFromDwarfReg(_processId, result.value);
                    std::cout << at_name(die)
                              << " (reg " << result.value
                              << ") = " << value << std::endl;
                }
                else {
                    throw std::runtime_error{"Unhandled variable location"};
                }
            }
            else {
                throw std::runtime_error{"Unhandled variable location"};
            }
        }
    }
}
