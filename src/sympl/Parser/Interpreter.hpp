//
// GameSencha, LLC 5/24/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include <sympl/Memory/ManagedObject.hpp>

SymplNamespaceStart

class Interpreter : public ManagedObject
{
public:
    Interpreter() = default;

    SharedPtr<class ValueHandle> Visit(SharedPtr<class ParserNode> Node);

    SharedPtr<class ValueHandle> VisitNumberNode(SharedPtr<class ParserNode> Node);

    SharedPtr<class ValueHandle> VisitBinaryOpNode(SharedPtr<class ParserNode> Node);

    SharedPtr<class ValueHandle> VisitUnaryOpNode(SharedPtr<class ParserNode> Node);

    SharedPtr<class ValueHandle> NoVisit(SharedPtr<class ParserNode> Node);
};

SymplNamespaceEnd
