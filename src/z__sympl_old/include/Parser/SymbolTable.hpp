//
// GameSencha, LLC 5/26/22.
//
#pragma once
#include <__sympl_old/SymplPCH.hpp>
#include <__sympl_old/include/Memory/ManagedObject.hpp>

SymplNamespaceStart

class SYMPL_API SymbolTable : public ManagedObject
{
	SYMPL_OBJECT(SymbolTable, ManagedObject)

public:
    // List of symbols to keep track of.
    std::unordered_map<std::string, SharedPtr<class ValueHandle>> SymbolList;
    // Parent table of the symbol table.
    SharedPtr<SymbolTable> Parent;

    /**
     * Overrides the construct.
     * @param argc
     * @param ArgList
     */
    virtual void __Construct(int argc, va_list ArgList) override;

    /**
     * Destructor.
     */
    void __Destruct() override;

    /**
     * Creates the table.
     * @param pParent
     */
    void Create(const SharedPtr<SymbolTable>& pParent);

    /**
     * Copies values to another table.
     * @param SrcTable
     */
    void CopyFrom(const SharedPtr<SymbolTable>& SrcTable);

    /**
     * Removes an entry in the table.
     * @param VarName
     */
    void Remove(const char* VarName);

    /**
     * Sets or replaces a new entry in the symbol table.
     * @param VarName
     * @param Value
     */
    void Set(const char* VarName, const SharedPtr<class ValueHandle>& Value);

    /**
     * Returns a variable by a given name.
     * @param VarName
     * @return
     */
    SharedPtr<class ValueHandle> Get(const char* VarName);
};

SymplNamespaceEnd
