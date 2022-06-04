//
// Created by Victor on 5/25/2022.
//
#include "Interpreter.hpp"
#include <sympl/Memory/MemPool.hpp>
#include "sympl/Parser/Token.hpp"
#include "sympl/Parser/SymbolTable.hpp"
#include "sympl/Parser/ParserContext.hpp"
#include "sympl/Parser/ParseResult.hpp"
#include "sympl/Parser/Error/ParserError.hpp"
#include "sympl/Parser/Error/RuntimeError.hpp"
#include "sympl/Parser/Node/ParserNumberNode.hpp"
#include "sympl/Parser/Node/ParserBinaryOpNode.hpp"
#include "sympl/Parser/Node/ParserListNode.hpp"
#include "sympl/Parser/Node/ParserIfNode.hpp"
#include "sympl/Parser/Node/ParserForNode.hpp"
#include "sympl/Parser/Node/ParserWhileNode.hpp"
#include "sympl/Parser/Node/ParserUnaryOpNode.hpp"
#include "sympl/Parser/Node/ParserStringNode.hpp"
#include "sympl/Parser/Node/ParserReturnNode.hpp"
#include "sympl/Parser/Node/ParserContinueNode.hpp"
#include "sympl/Parser/Node/ParserBreakNode.hpp"
#include "sympl/Parser/Node/VarAccessNode.hpp"
#include "sympl/Parser/Node/VarAssignNode.hpp"
#include "sympl/Parser/Handle/CompareHandle.hpp"
#include "sympl/Parser/Handle/IntHandle.hpp"
#include "sympl/Parser/Handle/FloatHandle.hpp"
#include "sympl/Parser/Node/ParserFuncDefNode.hpp"
#include "sympl/Parser/Node/ParserObjectNode.hpp"
#include "sympl/Parser/Node/ParserNewObjectNode.hpp"
#include "sympl/Parser/Node/ParserCallNode.hpp"
#include "sympl/Parser/Node/ParserScopeAccessNode.hpp"
#include "sympl/Parser/Handle/FuncHandle.hpp"
#include "sympl/Parser/Handle/ObjectHandle.hpp"
#include "sympl/Parser/Handle/StringHandle.hpp"
#include "sympl/Parser/Handle/NullHandle.hpp"
#include "sympl/Parser/Handle/ListHandle.hpp"
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
        case ParseNodeType::String:
        {
            return VisitStringNode(Node, Context);
        }
        case ParseNodeType::Binary:
        {
            return VisitBinaryOpNode(Node, Context);
        }
        case ParseNodeType::Unary:
        {
            return VisitUnaryOpNode(Node, Context);
        }
		case ParseNodeType::If:
		{
			return VisitIfNode(Node, Context);
		}
		case ParseNodeType::For:
		{
			return VisitForNode(Node, Context);
		}
		case ParseNodeType::While:
		{
			return VisitWhileNode(Node, Context);
		}
		case ParseNodeType::List:
		{
			return VisitListNode(Node, Context);
		}
        case ParseNodeType::ScopeAccess:
        {
            return VisitScopeAccessNode(Node, Context);
        }
        case ParseNodeType::VarAccess:
        {
            return VisitVarAccessNode(Node, Context);
        }
        case ParseNodeType::VarAssign:
        {
            return VisitVarAssignNode(Node, Context);
        }
        case ParseNodeType::Return:
        {
            return VisitReturnNode(Node, Context);
        }
        case ParseNodeType::Continue:
        {
            return VisitContinueNode(Node, Context);
        }
        case ParseNodeType::Break:
        {
            return VisitBreakNode(Node, Context);
        }
        case ParseNodeType::Func:
        {
            return VisitFuncDefNode(Node, Context);
        }
        case ParseNodeType::Object:
        {
            return VisitObjectNode(Node, Context);
        }
        case ParseNodeType::NewObject:
        {
            return VisitNewObjectNode(Node, Context);
        }
        case ParseNodeType::Call:
        {
            return VisitCallNode(Node, Context);
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

SharedPtr<ParserRuntimeResult> Interpreter::VisitStringNode(SharedPtr<ParserNode> Node, SharedPtr<ParserContext> Context)
{
    auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
    auto NewStr = StringHandle::Alloc<StringHandle>(1, Node->NodeToken->GetValue());

    NewStr->Context = Context;
    NewStr->SetPosition(Node->StartPosition, Node->EndPosition);

    Result->Success(NewStr.Ptr());
    return Result;
}

SharedPtr<ParserRuntimeResult> Interpreter::VisitBinaryOpNode(SharedPtr<ParserNode> Node, SharedPtr<ParserContext> Context)
{
    auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
    auto OpNode = static_cast<ParserBinaryOpNode*>(Node.Ptr());

    auto Left = Result->Register(Visit(OpNode->LeftNode, Context));
    if (Result->ShouldReturn()) {
        return Result;
    }

    auto Right = Result->Register(Visit(OpNode->RightNode, Context));
    if (Result->ShouldReturn()) {
        return Result;
    }

    SharedPtr<ValueHandle> ValueResult;

    switch (Node->NodeToken->GetType())
    {
        case TokenType::Plus:
        {
            ValueResult = Left->AddTo(Right);
            break;
        }
        case TokenType::Minus:
        {
            ValueResult = Left->SubtractBy(Right);
            break;
        }
        case TokenType::Mul:
        {
            ValueResult = Left->MultiplyBy(Right);
            break;
        }
        case TokenType::Div:
        {
            ValueResult = Left->DivideBy(Right);
            break;
        }
        case TokenType::Power:
        {
            ValueResult = Left->PowerBy(Right).Ptr();
            break;
        }
		case TokenType::IsEqual:
		{
			SharedPtr<CompareHandle> LeftComp = CompareHandle::CreateFrom(Left, Context);
			SharedPtr<CompareHandle> RightComp = CompareHandle::CreateFrom(Right, Context);
			ValueResult = LeftComp->CompareEqual(RightComp).Ptr();
			break;
		}
		case TokenType::NotEqual:
		{
			SharedPtr<CompareHandle> LeftComp = CompareHandle::CreateFrom(Left, Context);
			SharedPtr<CompareHandle> RightComp = CompareHandle::CreateFrom(Right, Context);
			ValueResult = LeftComp->CompareNotEqual(RightComp).Ptr();
			break;
		}
		case TokenType::LessThan:
		{
			SharedPtr<CompareHandle> LeftComp = CompareHandle::CreateFrom(Left, Context);
			SharedPtr<CompareHandle> RightComp = CompareHandle::CreateFrom(Right, Context);
			ValueResult = LeftComp->CompareLessThan(RightComp).Ptr();
			break;
		}
		case TokenType::LessThanOrEqual:
		{
			SharedPtr<CompareHandle> LeftComp = CompareHandle::CreateFrom(Left, Context);
			SharedPtr<CompareHandle> RightComp = CompareHandle::CreateFrom(Right, Context);
			ValueResult = LeftComp->CompareLessThanOrEqual(RightComp).Ptr();
			break;
		}
		case TokenType::GreaterThan:
		{
			SharedPtr<CompareHandle> LeftComp = CompareHandle::CreateFrom(Left, Context);
			SharedPtr<CompareHandle> RightComp = CompareHandle::CreateFrom(Right, Context);
			ValueResult = LeftComp->CompareGreaterThan(RightComp).Ptr();
			break;
		}
		case TokenType::GreaterThanOrEqual:
		{
			SharedPtr<CompareHandle> LeftComp = CompareHandle::CreateFrom(Left, Context);
			SharedPtr<CompareHandle> RightComp = CompareHandle::CreateFrom(Right, Context);
			ValueResult = LeftComp->CompareGreaterThanOrEqual(RightComp).Ptr();
			break;
		}
		case TokenType::And:
		{
			SharedPtr<CompareHandle> LeftComp = CompareHandle::CreateFrom(Left, Context);
			SharedPtr<CompareHandle> RightComp = CompareHandle::CreateFrom(Right, Context);
			ValueResult = LeftComp->CompareAnd(RightComp).Ptr();
			break;
		}
		case TokenType::Or:
		{
			SharedPtr<CompareHandle> LeftComp = CompareHandle::CreateFrom(Left, Context);
			SharedPtr<CompareHandle> RightComp = CompareHandle::CreateFrom(Right, Context);
			ValueResult = LeftComp->CompareOr(RightComp).Ptr();
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
    if (Result->ShouldReturn()) {
        return Result;
    }

    if (Node->NodeToken->GetType() == TokenType::Minus)
    {
        auto Number = SharedPtr<NumberHandle>(dynamic_cast<NumberHandle*>(ValueResult.Ptr()));
        auto NegOneNumber = IntHandle::Alloc<IntHandle, NumberHandle>();
        NegOneNumber->SetIntValue(-1);
        ValueResult = Number->MultiplyBy(NegOneNumber.Ptr()).Ptr();
    }

    ValueResult->SetPosition(Node->StartPosition, Node->EndPosition);
    if (ValueResult->Error.IsValid()) {
        Result->Failure(ValueResult->Error);
        return Result;
    }

    Result->Success(ValueResult);

    return Result;
}

SharedPtr<ParserRuntimeResult> Interpreter::VisitIfNode(SharedPtr<ParserNode> Node, SharedPtr<ParserContext> Context)
{
	auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
	auto IfNode = SharedPtr<ParserIfNode>(dynamic_cast<ParserIfNode*>(Node.Ptr()));
	assert(IfNode.IsValid());

	for (const auto& entry : IfNode->Cases) {
		auto Condition = std::get<0>(entry);
		auto Expr = std::get<1>(entry);
        auto ShouldReturnNull = std::get<2>(entry);

		auto ConditionValue = Result->Register(Visit(Condition, Context));
		if (Result->ShouldReturn()) {
			return Result;
		}

		if (ConditionValue->IsTrue()) {
			auto ExprValue = Result->Register(Visit(Expr, Context));
			if (Result->ShouldReturn()) {
				return Result;
			}

			Result->Success(ShouldReturnNull ? ValueHandle::Null() : ExprValue);
			return Result;
		}
	}

    auto ElseCase = std::get<0>(IfNode->ElseCase);
    if (ElseCase.IsValid()) {
        auto ElseCaseShouldReturnNull = std::get<1>(IfNode->ElseCase);
		auto ElseValue = Result->Register(Visit(ElseCase, Context));
		if (Result->ShouldReturn()) {
			return Result;
		}

		Result->Success(ElseCaseShouldReturnNull ? ValueHandle::Null() : ElseValue);
		return Result;
	}

	Result->Success(ValueHandle::Null());
	return Result;
}

SharedPtr<ParserRuntimeResult> Interpreter::VisitForNode(SharedPtr<ParserNode> Node, SharedPtr<ParserContext> Context)
{
	auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
	SharedPtr<ParserForNode> ForNode = dynamic_cast<ParserForNode*>(Node.Ptr());
	std::vector<SharedPtr<ValueHandle>> ResultElements;

	auto StartValue = Result->Register(Visit(ForNode->StartValueNode, Context));
	if (StartValue->Error.IsValid() || StringNotEquals(StartValue->GetTypeName().c_str(), "IntHandle")) {
		return Result;
	}

	auto EndValue = Result->Register(Visit(ForNode->EndValueNode, Context));
	if (!EndValue.IsValid() || EndValue->Error.IsValid() || StringNotEquals(EndValue->GetTypeName().c_str(), "IntHandle")) {
		return Result;
	}

	SharedPtr<ValueHandle> StepValue;
	if (ForNode->StepValueNode.IsValid() && StringEquals(ForNode->StepValueNode->GetTypeName().c_str(), "IntHandle")) {
		StepValue = Result->Register(Visit(ForNode->StepValueNode, Context));
		if (StepValue->Error.IsValid()) {
			return Result;
		}
	} else {
		auto IntValue = IntHandle::Alloc<IntHandle>();
		IntValue->SetValue(1);
		StepValue = IntValue.Ptr();
	}

	int CurrentStepValue = static_cast<int>(dynamic_cast<IntHandle*>(StartValue.Ptr())->Value.IntNum);
	std::function<bool(int StepValue)> Condition;

	if (StepValue->Value.IntNum >= 0) {
		Condition = [=](int StepValue) {
			return StepValue < dynamic_cast<IntHandle*>(EndValue.Ptr())->Value.IntNum;
		};
	}
	else {
		Condition = [=](int StepValue) {
			return StepValue > dynamic_cast<IntHandle*>(EndValue.Ptr())->Value.IntNum;
		};
	}

	while (Condition(CurrentStepValue)) {
		auto StepHandle = IntHandle::Alloc<IntHandle>();
		StepHandle->SetValue(CurrentStepValue);

		Context->VariableSymbolTable->Set(
			Node->NodeToken->GetValue(),
			StepHandle.Ptr()
		);

		CurrentStepValue += static_cast<int>(StepValue->Value.IntNum);

		auto LoopValue = Result->Register(Visit(ForNode->BodyNode, Context));
		if (Result->ShouldReturn() && !Result->LoopShouldContinue && !Result->LoopShouldBreak) {
			return Result;
		}

        if (Result->LoopShouldContinue) {
            continue;
        }

        if (Result->LoopShouldBreak) {
            break;
        }

        ResultElements.emplace_back(LoopValue);
	}

    if (Node->ShouldReturnNull) {
        Result->Success(ValueHandle::Null());
        return Result;
    }

	auto ResultList = ListHandle::Alloc<ListHandle>();
	ResultList->Create(ResultElements);
	ResultList->Context = Context;
	ResultList->SetPosition(Node->StartPosition, Node->EndPosition);

	Result->Success(ResultList.Ptr());
	return Result;
}

SharedPtr<ParserRuntimeResult> Interpreter::VisitWhileNode(SharedPtr<ParserNode> Node, SharedPtr<ParserContext> Context)
{
	auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
	SharedPtr<ParserWhileNode> WhileNode = dynamic_cast<ParserWhileNode*>(Node.Ptr());
	std::vector<SharedPtr<ValueHandle>> ResultElements;

	while (!MemPool::Instance()->IsMaxMemUsage()) {
		auto Condition = Result->Register(Visit(WhileNode->ConditionNode, Context));
		if (Result->ShouldReturn()) {
			return Result;
		}

		if (!Condition->IsTrue()) {
			break;
		}

		auto LoopValue = Result->Register(Visit(WhileNode->BodyNode, Context));
        if (Result->ShouldReturn() && !Result->LoopShouldContinue && !Result->LoopShouldBreak) {
            return Result;
        }

        if (Result->LoopShouldContinue) {
            continue;
        }

        if (Result->LoopShouldBreak) {
            break;
        }

        ResultElements.emplace_back(LoopValue);
	}

    if (Node->ShouldReturnNull) {
        Result->Success(ValueHandle::Null());
        return Result;
    }

	auto ResultList = ListHandle::Alloc<ListHandle>();
	ResultList->Create(ResultElements);
	ResultList->Context = Context;
	ResultList->SetPosition(Node->StartPosition, Node->EndPosition);

	Result->Success(ResultList.Ptr());
	return Result;
}

SharedPtr<class ParserRuntimeResult> Interpreter::VisitListNode(SharedPtr<ParserNode> Node, SharedPtr<ParserContext> Context)
{
	auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
	SharedPtr<ParserListNode> ListNode = dynamic_cast<ParserListNode*>(Node.Ptr());
	std::vector<SharedPtr<ValueHandle>> Elements;

	for (const auto& ItemNode : ListNode->ElementNodeList)
	{
		Elements.emplace_back(Result->Register(Visit(ItemNode, Context)));
		if (Result->ShouldReturn()) {
			return Result;
		}
	}

	auto NewList = ListHandle::Alloc<ListHandle>();
	NewList->Create(Elements);
	NewList->Context = Context;
	NewList->SetPosition(Node->StartPosition, Node->EndPosition);
	Result->Success(NewList.Ptr());

	return Result;
}

SharedPtr<class ParserRuntimeResult>
Interpreter::VisitScopeAccessNode(SharedPtr<ParserNode> Node, SharedPtr<ParserContext> Context)
{
    auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
    auto AccessNode = ObjectRef::CastTo<ParserScopeAccessNode>(Node.Ptr());
    auto ParentVarName = AccessNode->ParentScopeToken->GetValue();
    auto VarName = Node->NodeToken->GetValue();

    auto ParentValue = Context->VariableSymbolTable->Get(ParentVarName);
    if (!ParentValue.IsValid() || ParentValue->Type != ValueType::Object) {
        Result->Failure(new RuntimeError(
            Context,
            Node->StartPosition,
            Node->EndPosition,
            fmt::format("'{0}' must be an object", ParentVarName).c_str()
        ));
        return Result;
    }

    SharedPtr<ValueHandle> Value = ParentValue->Context->VariableSymbolTable->Get(VarName);
    if (AccessNode->AssignNode.IsValid()) {
        Value = Result->Register(Visit(AccessNode->AssignNode, ParentValue->Context));
    } else {
        Value = ParentValue->Context->VariableSymbolTable->Get(VarName);
    }

    if (!Value.IsValid()) {
        Result->Failure(new RuntimeError(
            Context,
            Node->StartPosition,
            Node->EndPosition,
            fmt::format("'{0}' is not defined", VarName).c_str()
        ));
        return Result;
    }

    auto NewValue = Value->Copy();
    NewValue->SetPosition(Node->StartPosition, Node->EndPosition);
    NewValue->Context = ParentValue->Context;

    Result->Success(NewValue);
    return Result;
}

SharedPtr<ParserRuntimeResult> Interpreter::VisitVarAccessNode(SharedPtr<ParserNode> Node, SharedPtr<ParserContext> Context)
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

    auto RetValue = Value->Copy();
    RetValue->SetPosition(Node->StartPosition, Node->EndPosition);

    Result->Success(RetValue);
    return Result;
}

SharedPtr<class ParserRuntimeResult> Interpreter::VisitVarAssignNode(SharedPtr<ParserNode> Node, SharedPtr<ParserContext> Context)
{
    auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
    auto VarName = Node->NodeToken->GetValue();

    auto AssignNode = SharedPtr<VarAssignNode>(dynamic_cast<VarAssignNode*>(Node.Ptr()));
    auto Value = Result->Register(Visit(AssignNode->Value, Context));

    if (Result->ShouldReturn()) {
        return Result;
    }

	// Check if we have this value, and it's mutable.
	auto ContextValue = Context->VariableSymbolTable->Get(VarName);
	if (ContextValue.IsValid() && ContextValue->Immutable) {
		Result->Failure(new RuntimeError(
			Context,
			Node->StartPosition,
			Node->EndPosition,
			fmt::format("'{0}' is not a mutable value", VarName).c_str()
		));
		return Result;
	}

//    auto VarContext = Context->Parent.IsValid() ? Context->Parent : Context;
    auto VarContext = Context;
    VarContext->VariableSymbolTable->Set(VarName, Value);

    Result->Success(Value);
    return Result;
}

SharedPtr<class ParserRuntimeResult> Interpreter::VisitFuncDefNode(SharedPtr<ParserNode> Node, SharedPtr<ParserContext> Context)
{
    auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
    SharedPtr<ParserFuncDefNode> FuncDefNode = dynamic_cast<ParserFuncDefNode*>(Node.Ptr());

    auto FuncName = FuncDefNode->NodeToken.IsValid() ? FuncDefNode->NodeToken->GetValue() : "";
    auto BodyNode = FuncDefNode->BodyNode;

    std::vector<std::string> ArgNameList;
    for (const auto& Arg : FuncDefNode->ArgNameTokenList)
    {
        ArgNameList.emplace_back(Arg->GetValue());
    }

    auto NewFuncValue = FuncHandle::Alloc<FuncHandle>();
    NewFuncValue->Create(FuncName, BodyNode, ArgNameList);
    NewFuncValue->Context = Context;
    NewFuncValue->SetPosition(Node->StartPosition, Node->EndPosition);
    NewFuncValue->ShouldAutoReturn = Node->ShouldAutoReturn;

    if (FuncDefNode->NodeToken.IsValid())
    {
        Context->VariableSymbolTable->Set(FuncName, NewFuncValue.Ptr());
    }

    Result->Success(NewFuncValue.Ptr());
    return Result;
}

SharedPtr<class ParserRuntimeResult> Interpreter::VisitObjectNode(SharedPtr<ParserNode> Node, SharedPtr<ParserContext> Context)
{
    auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
    SharedPtr<ParserObjectNode> ObjectNode = ObjectRef::CastTo<ParserObjectNode>(Node.Ptr());

    auto ObjectName = ObjectNode->NodeToken->GetValue();
    auto BodyNode = ObjectNode->BodyNode;

    auto NewObjectContext = ParserContext::Alloc<ParserContext>();
    NewObjectContext->Create(Context, Node->StartPosition, ObjectName);
    NewObjectContext->VariableSymbolTable = SymbolTable::Alloc<SymbolTable>(1, Context->VariableSymbolTable.Ptr());

    auto NewObjectValue = ObjectHandle::Alloc<ObjectHandle>(2, ObjectName, BodyNode.Ptr());
    NewObjectValue->Context = NewObjectContext;
    NewObjectValue->SetPosition(Node->StartPosition, Node->EndPosition);

    Context->VariableSymbolTable->Set(ObjectName, NewObjectValue.Ptr());

    // Register body variables
    SharedPtr<ParserListNode> BodyListValues = ObjectRef::CastTo<ParserListNode>(BodyNode.Ptr());
    if (BodyListValues.IsValid()) {
        for (auto& Item: BodyListValues->ElementNodeList) {
            auto Value = Result->Register(Visit(Item, NewObjectContext));
            Value->Context = NewObjectContext;
            NewObjectContext->VariableSymbolTable->Set(Item->NodeToken->GetValue(), Value);
        }
    }

    Result->Success(NewObjectValue.Ptr());
    return Result;
}

SharedPtr<class ParserRuntimeResult> Interpreter::VisitNewObjectNode(SharedPtr<ParserNode> Node, SharedPtr<ParserContext> Context)
{
    auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
    SharedPtr<ParserNewObjectNode> NewObjectNode = ObjectRef::CastTo<ParserNewObjectNode>(Node.Ptr());

    auto ObjectToCopyName = NewObjectNode->NodeToken->GetValue();
    auto CopyObject = Context->VariableSymbolTable->Get(ObjectToCopyName);

    if (CopyObject->Type != ValueType::Object) {
        Result->Failure(new RuntimeError(
            Context,
            Node->StartPosition,
            Node->EndPosition,
            fmt::format("'{0}' cannot be instantiated", CopyObject->ToString()).c_str()
        ));
        return Result;
    }

    auto NewObjectValue = CopyObject->Copy();
    auto ContextObjectName = fmt::format("{0}:#{1}", ObjectToCopyName, NewObjectValue->InstanceId);
    NewObjectValue->Context = CopyObject->GenerateNewContext(ContextObjectName.c_str(), Context);

    ObjectRef::CastTo<ObjectHandle>(NewObjectValue.Ptr())->IsInstancedObject = true;
    NewObjectValue->SetPosition(Node->StartPosition, Node->EndPosition);

    // Copy over the body variables
    auto ObjectContext = NewObjectValue->Context;

    SharedPtr<ParserListNode> BodyListValues = ObjectRef::CastTo<ParserListNode>(
        ObjectRef::CastTo<ObjectHandle>(NewObjectValue.Ptr())->BodyNode.Ptr()
    );
    if (BodyListValues.IsValid()) {
        for (auto& Item: BodyListValues->ElementNodeList) {
            auto Value = Result->Register(Visit(Item, ObjectContext));
            ObjectContext->VariableSymbolTable->Set(Item->NodeToken->GetValue(), Value);
        }
    }

    Context->VariableSymbolTable->Set(ContextObjectName.c_str(), NewObjectValue.Ptr());

    Result->Success(NewObjectValue);
    return Result;
}

SharedPtr<class ParserRuntimeResult> Interpreter::VisitCallNode(SharedPtr<ParserNode> Node, SharedPtr<ParserContext> Context)
{
    auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
    SharedPtr<ParserCallNode> CallNode = dynamic_cast<ParserCallNode*>(Node.Ptr());
    std::vector<SharedPtr<ValueHandle>> ArgList;

    auto ValueToCall = Result->Register(Visit(CallNode->CallNode, Context));
    if (Result->ShouldReturn()) {
        return Result;
    }

    ValueToCall = ValueToCall->Copy();
    ValueToCall->SetPosition(Node->StartPosition, Node->EndPosition);

    if (ValueToCall->GetTypeName() == "BuiltInFuncHandle" || !ValueToCall->Context.IsValid()) {
        ValueToCall->Context = Context;
    }

    for (const auto& ArgNode : CallNode->ArgNodeList)
    {
        ArgList.emplace_back(Result->Register(Visit(ArgNode, Context)));
        if (Result->ShouldReturn()) {
            return Result;
        }
    }

    auto ReturnValue = Result->Register(ValueToCall->Exec(ArgList));
    if (Result->ShouldReturn()) {
        return Result;
    }

    ReturnValue = ReturnValue->Copy();
    ReturnValue->SetPosition(Node->StartPosition, Node->EndPosition);
    ReturnValue->Context = Context;

    Result->Success(ReturnValue);
    return Result;
}

SharedPtr<class ParserRuntimeResult> Interpreter::VisitReturnNode(SharedPtr<ParserNode> Node, SharedPtr<ParserContext> Context)
{
    auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
    auto ReturnNode = ObjectRef::CastTo<ParserReturnNode>(Node.Ptr());
    SharedPtr<ValueHandle> Value = ValueHandle::Null();

    if (ReturnNode->NodeToReturn.IsValid()) {
        Value = Result->Register(Visit(ReturnNode->NodeToReturn.Ptr(), Context));
        if (Result->ShouldReturn()) {
            return Result;
        }
    }

    Result->SuccessReturn(Value);
    return Result;
}

SharedPtr<class ParserRuntimeResult>
Interpreter::VisitContinueNode(SharedPtr<ParserNode> Node, SharedPtr<ParserContext> Context)
{
    auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
    Result->SuccessContinue();
    return Result;
}

SharedPtr<class ParserRuntimeResult> Interpreter::VisitBreakNode(SharedPtr<ParserNode> Node, SharedPtr<ParserContext> Context)
{
    auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
    Result->SuccessBreak();
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
