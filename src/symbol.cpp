#include <iostream>
#include "symbol.hpp"

Symbol::Symbol(TYPE t, std::string n, std::uintptr_t a)
    :_type(t),
     _name(n),
     _address(a){}

Symbol::Symbol(elf::stt s, std::uintptr_t a)
    :_type(getSymbolTypeFromElf(s)),
     _name(getSymbolName(_type)),
     _address(a){}

Symbol::TYPE Symbol::getSymbolTypeFromElf (elf::stt s)
{
    auto typeIt = _typeFromElf.find(s);
    if (typeIt == _typeFromElf.end()) {
        return TYPE::notype;
    }
    return typeIt->second;
}

std::string Symbol::getSymbolName (TYPE type)
{
    auto typeNameIt = _typeNames.find(type);
    if (typeNameIt == _typeNames.end()) {
        std::cerr << "Type not found\n";
        return "";
    }
    return typeNameIt->second;
}

std::string Symbol::getName ()
{
    return _name;
}

Symbol::TYPE Symbol::getType ()
{
    return _type;
}

std::uintptr_t Symbol::getAddress ()
{
    return _address;
}
