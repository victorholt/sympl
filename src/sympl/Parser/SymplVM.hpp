//
// GameSencha, LLC 6/3/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include <sympl/Memory/ManagedObject.hpp>

SymplNamespaceStart

class SYMPL_API SymplVM : public ManagedObject
{
    SYMPL_OBJECT(SymplVM, ManagedObject)

public:
    /**
     * Runs a given script.
     * @param VarName
     */
    std::tuple<SharedPtr<class ValueHandle>, SharedPtr<class ParserError>> RunScript(CStrPtr FileName, CStrPtr Script);
};

SymplNamespaceEnd
