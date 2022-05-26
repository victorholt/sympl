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

    SharedPtr<class ParserRuntimeResult> Visit(SharedPtr<class ParserNode> Node);

    SharedPtr<class ParserRuntimeResult> VisitNumberNode(SharedPtr<class ParserNode> Node);

    SharedPtr<class ParserRuntimeResult> VisitBinaryOpNode(SharedPtr<class ParserNode> Node);

    SharedPtr<class ParserRuntimeResult> VisitUnaryOpNode(SharedPtr<class ParserNode> Node);

    SharedPtr<class ParserRuntimeResult> NoVisit(SharedPtr<class ParserNode> Node);
};

SymplNamespaceEnd
