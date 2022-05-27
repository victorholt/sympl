//
// Created by Victor on 5/25/2022.
//
#include "Interpreter.hpp"
#include "sympl/Parser/Token.hpp"
#include "sympl/Parser/SymbolTable.hpp"
#include "sympl/Parser/ParserContext.hpp"
#include "sympl/Parser/ParseResult.hpp"
#include "sympl/Parser/Error/ParserError.hpp"
#include "sympl/Parser/Error/RuntimeError.hpp"
#include "sympl/Parser/Node/ParserNumberNode.hpp"
#include "sympl/Parser/Node/ParserBinaryOpNode.hpp"
#include "sympl/Parser/Node/ParserUnaryOpNode.hpp"
#include "sympl/Parser/Node/VarAccessNode.hpp"
#include "sympl/Parser/Node/VarAssignNode.hpp"
#include "sympl/Parser/Handle/IntHandle.hpp"
#include "sympl/Parser/Handle/FloatHandle.hpp"
#include "sympl/Parser/ParserRuntimeResult.hpp"
#include <fmt/format.h>
SymplNamespace

SharedPtr<ParserRuntimeResult> Interpreter::Visit(SharedPtr<ParserNode> Node, SharedPtr<ParserContext> Context)
{
    switch (Node->Type)
    {
        case ParseNodeType::Number:
        {
            return VisitNumberNode(Node, Context);
        }
        case ParseNodeType::Binary:
        {
            return VisitBinaryOpNode(Node, Context);
        }
        case ParseNodeType::Unary:
        {
            return VisitUnaryOpNode(Node, Context);
        }
        case ParseNodeType::VarAccess:
        {
            return VisitVarAccessNode(Node, Context);
        }
        case ParseNodeType::VarAssign:
        {
            return VisitVarAssignNode(Node, Context);
        }
    }

    return NoVisit(Node, Context);
}

SharedPtr<ParserRuntimeResult> Interpreter::VisitNumberNode(SharedPtr<ParserNode> Node, SharedPtr<ParserContext> Context)
{
    auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
    char* pNumEnd;
    auto NodeToken = Node->NodeToken;
    SharedPtr<ValueHandle> ValueResult;

    if (NodeToken->GetType() == TokenType::Int)
    {
        ValueResult = IntHandle::Alloc<IntHandle, ValueHandle>();
        ValueResult->Context = Context;

        dynamic_cast<IntHandle*>(ValueResult.Ptr())->SetValue(static_cast<int>(std::strtol(Node->NodeToken->GetValue(), &pNumEnd, 10)));
    }
    else if (NodeToken->GetType() == TokenType::Float)
    {
        ValueResult = FloatHandle::Alloc<FloatHandle, ValueHandle>();
        ValueResult->Context = Context;

        dynamic_cast<FloatHandle*>(ValueResult.Ptr())->SetValue(static_cast<float>(std::strtof(Node->NodeToken->GetValue(), &pNumEnd)));
    }
    ValueResult->SetPosition(Node->StartPosition, Node->EndPosition);

    Result->Success(ValueResult);
    return Result;
}

SharedPtr<ParserRuntimeResult> Interpreter::VisitBinaryOpNode(SharedPtr<ParserNode> Node, SharedPtr<ParserContext> Context)
{
    auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
    auto OpNode = static_cast<ParserBinaryOpNode*>(Node.Ptr());

    auto Left = Result->Register(Visit(OpNode->LeftNode, Context));
    if (Result->Error.IsValid()) {
        return Result;
    }

    auto Right = Result->Register(Visit(OpNode->RightNode, Context));
    if (Result->Error.IsValid()) {
        return Result;
    }

    SharedPtr<ValueHandle> ValueResult;

    switch (Node->NodeToken->GetType())
    {
        case TokenType::Plus:
        {
            SharedPtr<NumberHandle> LeftNumber = dynamic_cast<NumberHandle*>(Left.Ptr());
            SharedPtr<NumberHandle> RightNumber = dynamic_cast<NumberHandle*>(Right.Ptr());
            ValueResult = LeftNumber->AddTo(RightNumber).Ptr();
            break;
        }
        case TokenType::Minus:
        {
            SharedPtr<NumberHandle> LeftNumber = dynamic_cast<NumberHandle*>(Left.Ptr());
            SharedPtr<NumberHandle> RightNumber = dynamic_cast<NumberHandle*>(Right.Ptr());
            ValueResult = LeftNumber->SubtractBy(RightNumber).Ptr();
            break;
        }
        case TokenType::Mul:
        {
            SharedPtr<NumberHandle> LeftNumber = dynamic_cast<NumberHandle*>(Left.Ptr());
            SharedPtr<NumberHandle> RightNumber = dynamic_cast<NumberHandle*>(Right.Ptr());
            ValueResult = LeftNumber->MultiplyBy(RightNumber).Ptr();
            break;
        }
        case TokenType::Div:
        {
            SharedPtr<NumberHandle> LeftNumber = dynamic_cast<NumberHandle*>(Left.Ptr());
            SharedPtr<NumberHandle> RightNumber = dynamic_cast<NumberHandle*>(Right.Ptr());
            ValueResult = LeftNumber->DivideBy(RightNumber).Ptr();
            break;
        }
        case TokenType::Power:
        {
            SharedPtr<NumberHandle> LeftNumber = dynamic_cast<NumberHandle*>(Left.Ptr());
            SharedPtr<NumberHandle> RightNumber = dynamic_cast<NumberHandle*>(Right.Ptr());
            ValueResult = LeftNumber->PowerBy(RightNumber).Ptr();
            break;
        }
    }

    ValueResult->SetPosition(Node->StartPosition, Node->EndPosition);
    if (ValueResult->Error.IsValid()) {
        Result->Failure(ValueResult->Error);
        return Result;
    }

    Result->Success(ValueResult);

    return Result;
}

SharedPtr<ParserRuntimeResult> Interpreter::VisitUnaryOpNode(SharedPtr<ParserNode> Node, SharedPtr<ParserContext> Context)
{
    auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
    auto OpNode = dynamic_cast<ParserUnaryOpNode*>(Node.Ptr());

    auto ValueResult = Result->Register(Visit(OpNode->Node, Context));
    if (Result->Error.IsValid()) {
        return Result;
    }

    if (Node->NodeToken->GetType() == TokenType::Minus)
    {
        auto Number = SharedPtr<NumberHandle>(dynamic_cast<NumberHandle*>(ValueResult.Ptr()));
        auto NegOneNumber = IntHandle::Alloc<IntHandle, NumberHandle>();
        NegOneNumber->SetIntValue(-1);
        ValueResult = Number->MultiplyBy(NegOneNumber).Ptr();
    }

    ValueResult->SetPosition(Node->StartPosition, Node->EndPosition);
    if (ValueResult->Error.IsValid()) {
        Result->Failure(ValueResult->Error);
        return Result;
    }

    Result->Success(ValueResult);

    return Result;
}

SharedPtr<class ParserRuntimeResult> Interpreter::VisitVarAccessNode(SharedPtr<ParserNode> Node, SharedPtr<ParserContext> Context)
{
    auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
    auto VarName = Node->NodeToken->GetValue();
    auto Value = Context->VariableSymbolTable->Get(VarName);

    if (!Value.IsValid()) {
        Result->Failure(new RuntimeError(
            Context,
            Node->StartPosition,
            Node->EndPosition,
            fmt::format("'{0}' is not defined", VarName).c_str()
        ));
        return Result;
    }

    Value = Value->Copy();
    Value->SetPosition(Node->StartPosition, Node->EndPosition);

    Result->Success(Value);
    return Result;
}

SharedPtr<class ParserRuntimeResult> Interpreter::VisitVarAssignNode(SharedPtr<ParserNode> Node, SharedPtr<ParserContext> Context)
{
    auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
    auto VarName = Node->NodeToken->GetValue();

    auto AssignNode = SharedPtr<VarAssignNode>(dynamic_cast<VarAssignNode*>(Node.Ptr()));
    auto Value = Result->Register(Visit(AssignNode->Value, Context));

    if (Result->Error.IsValid()) {
        return Result;
    }

    Context->VariableSymbolTable->Set(VarName, Value);

    Result->Success(Value);
    return Result;
}

SharedPtr<ParserRuntimeResult> Interpreter::NoVisit(SharedPtr<ParserNode> Node, SharedPtr<ParserContext> Context)
{
    auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
    Result->Error = SharedPtr<RuntimeError>(new RuntimeError(
        Context,
        Node->StartPosition,
        Node->EndPosition,
        "Interpreter unable to execute code"
    )).Ptr();

    return Result;
}
