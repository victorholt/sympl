//
// Created by Victor on 5/25/2022.
//
#include "Interpreter.hpp"
#include "sympl/Parser/Token.hpp"
#include "sympl/Parser/Node/ParserNumberNode.hpp"
#include "sympl/Parser/Node/ParserBinaryOpNode.hpp"
#include "sympl/Parser/Node/ParserUnaryOpNode.hpp"
#include "sympl/Parser/Handle/IntHandle.hpp"
#include "sympl/Parser/Handle/FloatHandle.hpp"
SymplNamespace

SharedPtr<ValueHandle> Interpreter::Visit(SharedPtr<ParserNode> Node)
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

SharedPtr<ValueHandle> Interpreter::VisitNumberNode(SharedPtr<ParserNode> Node)
{
    char* pNumEnd;
    auto NodeToken = Node->NodeToken;
    SharedPtr<ValueHandle> Result;

    if (NodeToken->GetType() == TokenType::Int)
    {
        Result = SharedPtr<ValueHandle>(new IntHandle());
        dynamic_cast<IntHandle*>(Result.Ptr())->SetValue(static_cast<int>(std::strtol(Node->NodeToken->GetValue(), &pNumEnd, 10)));
    }
    else if (NodeToken->GetType() == TokenType::Float)
    {
        Result = SharedPtr<ValueHandle>(new FloatHandle());
        dynamic_cast<FloatHandle*>(Result.Ptr())->SetValue(static_cast<float>(std::strtof(Node->NodeToken->GetValue(), &pNumEnd)));
    }
    Result->SetPosition(Node->StartPosition, Node->EndPosition);

    return Result;
}

SharedPtr<ValueHandle> Interpreter::VisitBinaryOpNode(SharedPtr<ParserNode> Node)
{
    auto OpNode = static_cast<ParserBinaryOpNode*>(Node.Ptr());

    auto Left = Visit(OpNode->LeftNode);
    auto Right = Visit(OpNode->RightNode);

    SharedPtr<ValueHandle> Result;

    switch (Node->NodeToken->GetType())
    {
        case TokenType::Plus:
        {
            SharedPtr<NumberHandle> LeftNumber = static_cast<NumberHandle*>(Left.Ptr());
            SharedPtr<NumberHandle> RightNumber = static_cast<NumberHandle*>(Right.Ptr());
            Result = LeftNumber->AddTo(RightNumber).Ptr();
            break;
        }
        case TokenType::Minus:
        {
            SharedPtr<NumberHandle> LeftNumber = static_cast<NumberHandle*>(Left.Ptr());
            SharedPtr<NumberHandle> RightNumber = static_cast<NumberHandle*>(Right.Ptr());
            Result = LeftNumber->SubtractBy(RightNumber).Ptr();
            break;
        }
        case TokenType::Mul:
        {
            SharedPtr<NumberHandle> LeftNumber = static_cast<NumberHandle*>(Left.Ptr());
            SharedPtr<NumberHandle> RightNumber = static_cast<NumberHandle*>(Right.Ptr());
            Result = LeftNumber->MultiplyBy(RightNumber).Ptr();
            break;
        }
        case TokenType::Div:
        {
            SharedPtr<NumberHandle> LeftNumber = static_cast<NumberHandle*>(Left.Ptr());
            SharedPtr<NumberHandle> RightNumber = static_cast<NumberHandle*>(Right.Ptr());
            Result = LeftNumber->DivideBy(RightNumber).Ptr();
            break;
        }
    }

    Result->SetPosition(Node->StartPosition, Node->EndPosition);
    return Result;
}

SharedPtr<ValueHandle> Interpreter::VisitUnaryOpNode(SharedPtr<ParserNode> Node)
{
    auto OpNode = static_cast<ParserUnaryOpNode*>(Node.Ptr());
    auto Result = Visit(OpNode->Node);

    if (Node->NodeToken->GetType() == TokenType::Minus)
    {
        auto Number = SharedPtr<NumberHandle>(static_cast<NumberHandle*>(Result.Ptr()));
        auto NegOneNumber = SharedPtr<NumberHandle>(new IntHandle());
        NegOneNumber->SetIntValue(-1);
        Result = Number->MultiplyBy(NegOneNumber).Ptr();
    }

    Result->SetPosition(Node->StartPosition, Node->EndPosition);
    return Result;
}

SharedPtr<ValueHandle> Interpreter::NoVisit(SharedPtr<ParserNode> Node)
{
    return { new ValueHandle() };
}
