#ifndef _SYMBOL_
#define _SYMBOL_

#include <string>
#include <map>

#include "elf++.hh"

class Symbol {
public :

    enum class TYPE {
        notype,
        object,
        func,
        section,
        file
    };

    Symbol (TYPE t, std::string n, std::uintptr_t a);
    Symbol (elf::stt s, std::uintptr_t a);
    TYPE getSymbolTypeFromElf (elf::stt s);
    std::string getSymbolName (TYPE type);
    std::string getName ();
    TYPE getType ();
    std::uintptr_t getAddress ();

private :

    std::map<TYPE, std::string> _typeNames =
        {
            {TYPE::notype, "notype"},
            {TYPE::object, "object"},
            {TYPE::func, "func"},
            {TYPE::section, "section"},
            {TYPE::file, "file"},
        };

    std::map<elf::stt, TYPE> _typeFromElf =
        {
            {elf::stt::notype, TYPE::notype},
            {elf::stt::object, TYPE::object},
            {elf::stt::func, TYPE::func},
            {elf::stt::section, TYPE::section},
            {elf::stt::file, TYPE::file},
        };

    TYPE _type;
    std::string _name;
    std::uintptr_t _address;
};

#endif //_SYMBOL_
