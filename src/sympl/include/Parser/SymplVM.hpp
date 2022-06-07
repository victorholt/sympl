//
// GameSencha, LLC 6/3/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include <sympl/include/Memory/ManagedObject.hpp>

SymplNamespaceStart

class SYMPL_API SymplVM : public ManagedObject
{
    SYMPL_OBJECT(SymplVM, ManagedObject)

protected:
    // Symbol table for the program.
    SharedPtr<class SymbolTable> GlobalSymbolTable;
    // Context for the program.
    SharedPtr<class ParserContext> GlobalContext;

public:
    /**
     * Constructs the object.
     * @param ArgList
     */
    void __Construct(int argc, va_list ArgList) override;

    /**
     * Runs a given script.
     * @param VarName
     */
    std::tuple<SharedPtr<class ValueHandle>, SharedPtr<class ParserError>> RunScript(CStrPtr FileName, CStrPtr Script);

    /**
     * Returns the global parser.
     * @return
     */
    SharedPtr<class ParserContext> GetContext() const;

    /**
     * Returns the global symbol table.
     * @return
     */
    SharedPtr<class SymbolTable> GetSymbolTable() const;
};

SymplNamespaceEnd
