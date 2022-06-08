//
// GameSencha, LLC 5/26/22.
//
#include <sympl/include/Parser/SymbolTable.hpp>
#include <sympl/include/Parser/Handle/ValueHandle.hpp>
SymplNamespace

void SymbolTable::__Construct(int argc, va_list ArgList)
{
    SharedPtr<SymbolTable> pParent;
    if (argc > 0) {
        pParent = va_arg(ArgList, SymbolTable*);
    }
    Create(pParent);
}

void SymbolTable::__Destruct()
{
    SymbolList.clear();
}

void SymbolTable::Create(const SharedPtr<SymbolTable>& pParent)
{
    Parent = pParent;
}

void SymbolTable::CopyFrom(const SharedPtr<SymbolTable>& SrcTable)
{
    assert(SrcTable.IsValid());
    for (const auto& Item : SrcTable->SymbolList)
    {
        SymbolList[Item.first] = Item.second->Copy();
    }
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
