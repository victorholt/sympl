//
// Created by Victor on 5/25/2022.
//
#include "Interpreter.hpp"
#include "sympl/Parser/Token.hpp"
#include "sympl/Parser/Error/ParserError.hpp"
#include "sympl/Parser/Error/RuntimeError.hpp"
#include "sympl/Parser/Node/ParserNumberNode.hpp"
#include "sympl/Parser/Node/ParserBinaryOpNode.hpp"
#include "sympl/Parser/Node/ParserUnaryOpNode.hpp"
#include "sympl/Parser/Handle/IntHandle.hpp"
#include "sympl/Parser/Handle/FloatHandle.hpp"
#include "sympl/Parser/ParserRuntimeResult.hpp"
SymplNamespace

SharedPtr<ParserRuntimeResult> Interpreter::Visit(SharedPtr<ParserNode> Node)
{
    switch (Node->Type)
    {
        case ParseNodeType::Number:
        {
            return VisitNumberNode(Node);
        }
        case ParseNodeType::Binary:
        {
            return VisitBinaryOpNode(Node);
        }
        case ParseNodeType::Unary:
        {
            return VisitUnaryOpNode(Node);
        }
    }

    return NoVisit(Node);
}

SharedPtr<ParserRuntimeResult> Interpreter::VisitNumberNode(SharedPtr<ParserNode> Node)
{
    auto Result = SharedPtr<ParserRuntimeResult>(new ParserRuntimeResult());
    char* pNumEnd;
    auto NodeToken = Node->NodeToken;
    SharedPtr<ValueHandle> ValueResult;

    if (NodeToken->GetType() == TokenType::Int)
    {
        ValueResult = SharedPtr<ValueHandle>(new IntHandle());
        dynamic_cast<IntHandle*>(ValueResult.Ptr())->SetValue(static_cast<int>(std::strtol(Node->NodeToken->GetValue(), &pNumEnd, 10)));
    }
    else if (NodeToken->GetType() == TokenType::Float)
    {
        ValueResult = SharedPtr<ValueHandle>(new FloatHandle());
        dynamic_cast<FloatHandle*>(ValueResult.Ptr())->SetValue(static_cast<float>(std::strtof(Node->NodeToken->GetValue(), &pNumEnd)));
    }
    ValueResult->SetPosition(Node->StartPosition, Node->EndPosition);

    Result->Success(ValueResult);
    return Result;
}

SharedPtr<ParserRuntimeResult> Interpreter::VisitBinaryOpNode(SharedPtr<ParserNode> Node)
{
    auto Result = SharedPtr<ParserRuntimeResult>(new ParserRuntimeResult());
    auto OpNode = static_cast<ParserBinaryOpNode*>(Node.Ptr());

    auto Left = Result->Register(Visit(OpNode->LeftNode));
    if (Result->Error.IsValid()) {
        return Result;
    }

    auto Right = Result->Register(Visit(OpNode->RightNode));
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
    }

    ValueResult->SetPosition(Node->StartPosition, Node->EndPosition);
    if (ValueResult->Error.IsValid()) {
        Result->Failure(ValueResult->Error);
        return Result;
    }

    Result->Success(ValueResult);

    return Result;
}

SharedPtr<ParserRuntimeResult> Interpreter::VisitUnaryOpNode(SharedPtr<ParserNode> Node)
{
    auto Result = SharedPtr<ParserRuntimeResult>(new ParserRuntimeResult());
    auto OpNode = dynamic_cast<ParserUnaryOpNode*>(Node.Ptr());

    auto ValueResult = Result->Register(Visit(OpNode->Node));
    if (Result->Error.IsValid()) {
        return Result;
    }

    if (Node->NodeToken->GetType() == TokenType::Minus)
    {
        auto Number = SharedPtr<NumberHandle>(dynamic_cast<NumberHandle*>(ValueResult.Ptr()));
        auto NegOneNumber = SharedPtr<NumberHandle>(new IntHandle());
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

SharedPtr<ParserRuntimeResult> Interpreter::NoVisit(SharedPtr<ParserNode> Node)
{
    auto Result = SharedPtr<ParserRuntimeResult>(new ParserRuntimeResult());
    Result->Error = SharedPtr<RuntimeError>(new RuntimeError(
        Node->StartPosition,
        Node->EndPosition,
        "Parser unable to interpret code"
    )).Ptr();

    return Result;
}
