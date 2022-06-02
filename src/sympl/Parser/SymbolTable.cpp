//
// GameSencha, LLC 5/26/22.
//
#include "SymbolTable.hpp"
#include <sympl/Parser/Handle/ValueHandle.hpp>
SymplNamespace

void SymbolTable::__Construct(int argc, va_list ArgList)
{
    SharedPtr<SymbolTable> pParent = va_arg(ArgList, SymbolTable*);
    Create(pParent);
}

void SymbolTable::Create(const SharedPtr<SymbolTable>& pParent)
{
    Parent = pParent;
}

void SymbolTable::Remove(const char* VarName)
{
    auto entry = SymbolList.find(VarName);
    if (entry == SymbolList.end()) {
        return;
    }
    SymbolList.erase(entry);
}

void SymbolTable::Set(const char* VarName, const SharedPtr<ValueHandle>& Value)
{
    SymbolList[VarName] = Value;
}

SharedPtr<ValueHandle> SymbolTable::Get(const char* VarName)
{
    auto entry = SymbolList.find(VarName);
    if (entry == SymbolList.end()) {
        return Parent.IsValid() ? Parent->Get(VarName) : nullptr;
    }
    return entry->second;
}