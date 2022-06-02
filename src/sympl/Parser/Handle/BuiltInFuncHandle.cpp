//
// GameSencha, LLC 5/31/22.
//
#include "BuiltInFuncHandle.hpp"
#include "StringHandle.hpp"
#include "IntHandle.hpp"
#include "ListHandle.hpp"
#include "ExceptionHandle.hpp"
#include <sympl/Parser/ParserRuntimeResult.hpp>
#include <sympl/Parser/Interpreter.hpp>
#include <sympl/Parser/ParserContext.hpp>
#include <sympl/Parser/SymbolTable.hpp>
#include <sympl/Parser/Token.hpp>
#include <sympl/Parser/Node/ParserNode.hpp>
#include <sympl/Parser/Error/RuntimeError.hpp>
SymplNamespace

void BuiltInFuncHandle::__Construct(int argc, va_list ArgList)
{
	CStrPtr pFuncName = va_arg(ArgList, CStrPtr);
	Create(pFuncName);

	// Create method/argument mappings.
	AddMethod(
		"Print",
		[=](SharedPtr<ParserContext> pExecContext) { return ExecPrint(pExecContext); },
		{ "value" }
	);

	AddMethod(
		"PrintRet",
		[=](SharedPtr<ParserContext> pExecContext) { return ExecPrintRet(pExecContext); },
		{ "value" }
	);

	AddMethod(
		"Input",
		[=](SharedPtr<ParserContext> pExecContext) { return ExecInput(pExecContext); },
		{}
	);

	AddMethod(
		"InputInt",
		[=](SharedPtr<ParserContext> pExecContext) { return ExecInputInt(pExecContext); },
		{}
	);

	AddMethod(
		"IsNumber",
		[=](SharedPtr<ParserContext> pExecContext) { return ExecIsType(pExecContext, {ValueType::Int, ValueType::Float}); },
		{ "value" }
	);

	AddMethod(
		"IsString",
		[=](SharedPtr<ParserContext> pExecContext) { return ExecIsType(pExecContext, {ValueType::String}); },
		{ "value" }
	);

	AddMethod(
		"IsList",
		[=](SharedPtr<ParserContext> pExecContext) { return ExecIsType(pExecContext, {ValueType::List}); },
		{ "value" }
	);

	AddMethod(
		"IsFunction",
		[=](SharedPtr<ParserContext> pExecContext) { return ExecIsType(pExecContext, {ValueType::Func}); },
		{ "value" }
	);

	AddMethod(
		"Append",
		[=](SharedPtr<ParserContext> pExecContext) { return ExecAppend(pExecContext); },
		{ "list", "value" }
	);

	AddMethod(
		"Remove",
		[=](SharedPtr<ParserContext> pExecContext) { return ExecRemove(pExecContext); },
		{ "list", "index" }
	);

	AddMethod(
		"Extend",
		[=](SharedPtr<ParserContext> pExecContext) { return ExecExtend(pExecContext); },
		{ "listA", "listB" }
	);
}

void BuiltInFuncHandle::Create(CStrPtr pFuncName)
{
	Type = ValueType::Func;
	Name = StringBuffer::Alloc<StringBuffer>();
	Name->Set(pFuncName && strlen(pFuncName) > 0 ? pFuncName : "<anonymous>");
}

SharedPtr<ParserRuntimeResult> BuiltInFuncHandle::Exec(const std::vector<SharedPtr<ValueHandle>>& ArgValueList)
{
	auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
	auto Interp = Interpreter::Alloc<Interpreter>();
	auto ExecContext = GenerateNewContext();

	CStrPtr MethodName = Name->CStr();
	if (MethodMap.find(MethodName) == MethodMap.end() || MethodArgMap.find(MethodName) == MethodArgMap.end())
	{
		return NoVisitMethod(ExecContext);
	}

	Result->Register(CheckAndPopulateArgs(MethodArgMap[MethodName], ArgValueList, ExecContext));
	if (Result->Error.IsValid()) {
		return Result;
	}

	auto ResultValue = Result->Register(MethodMap[MethodName](ExecContext));
	if (Result->Error.IsValid()) {
		return Result;
	}

	Result->Success(ResultValue);
	return Result;
}

SharedPtr<ParserRuntimeResult> BuiltInFuncHandle::ExecPrint(SharedPtr<struct ParserContext> &pExecContext)
{
	auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
	auto ReturnValue = pExecContext->VariableSymbolTable->Get("value");

	std::cout << ReturnValue->ToString() << std::endl;

	Result->Success(ValueHandle::Null());
	return Result;
}

SharedPtr<ParserRuntimeResult> BuiltInFuncHandle::ExecPrintRet(SharedPtr<struct ParserContext> &pExecContext)
{
	auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
	SharedPtr<ValueHandle> ReturnValue = pExecContext->VariableSymbolTable->Get("value");

	if (ReturnValue->Type != ValueType::String) {
		ReturnValue = StringHandle::Alloc<StringHandle>(1, ReturnValue->ToString()).Ptr();
	}

	Result->Success(ReturnValue);
	return Result;
}

SharedPtr<ParserRuntimeResult> BuiltInFuncHandle::ExecInput(SharedPtr<struct ParserContext> &pExecContext)
{
	auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
	std::string InputStr;
	std::getline(std::cin, InputStr);

	auto ReturnValue = StringHandle::Alloc<StringHandle>(1, InputStr.c_str());
	Result->Success(ReturnValue.Ptr());

	return Result;
}

SharedPtr<ParserRuntimeResult> BuiltInFuncHandle::ExecInputInt(SharedPtr<struct ParserContext> &pExecContext)
{
	auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
	std::string InputStr;
	std::getline(std::cin, InputStr);

	char* pNumEnd;
	auto IntValue = static_cast<int>(std::strtol(InputStr.c_str(), &pNumEnd, 10));

	auto ReturnValue = IntHandle::Alloc<IntHandle>();
	ReturnValue->SetValue(IntValue);
	Result->Success(ReturnValue.Ptr());

	return Result;
}

SharedPtr<ParserRuntimeResult> BuiltInFuncHandle::ExecIsType(
	SharedPtr<struct ParserContext> &pExecContext,
	const std::vector<ValueType>& pTypes
) {
	auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
	SharedPtr<ValueHandle> InputValue = pExecContext->VariableSymbolTable->Get("value");
	auto ReturnValue = ValueHandle::False();

	for (auto TypeEntry : pTypes) {
		if (InputValue->Type == TypeEntry) {
			ReturnValue = ValueHandle::True();
			break;
		}
	}

	Result->Success(ReturnValue);
	return Result;
}

SharedPtr<ParserRuntimeResult> BuiltInFuncHandle::ExecAppend(SharedPtr<struct ParserContext> &pExecContext)
{
	auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
	SharedPtr<ValueHandle> DestList = pExecContext->VariableSymbolTable->Get("list");
	SharedPtr<ValueHandle> ListValue = pExecContext->VariableSymbolTable->Get("value");

	if (DestList->Type != ValueType::List) {
		return InvalidMethodArgument(pExecContext, DestList->ToString(), "list as first argument");
	}

	Result->Success(DestList->AddTo(ListValue));
	return Result;
}

SharedPtr<ParserRuntimeResult> BuiltInFuncHandle::ExecRemove(SharedPtr<struct ParserContext> &pExecContext)
{
	auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
	SharedPtr<ValueHandle> ListValue = pExecContext->VariableSymbolTable->Get("list");
	SharedPtr<ValueHandle> ListIndexValue = pExecContext->VariableSymbolTable->Get("index");

	if (ListValue->Type != ValueType::List) {
		return InvalidMethodArgument(pExecContext, ListValue->ToString(), "list as first argument");
	}

	if (ListIndexValue->Type != ValueType::Int) {
		return InvalidMethodArgument(pExecContext, ListIndexValue->ToString(), "integer as second argument");
	}

	Result->Success(ListValue->SubtractBy(ListIndexValue));
	return Result;
}

SharedPtr<ParserRuntimeResult> BuiltInFuncHandle::ExecExtend(SharedPtr<struct ParserContext> &pExecContext)
{
	auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
	SharedPtr<ValueHandle> DestList = pExecContext->VariableSymbolTable->Get("listA");
	SharedPtr<ValueHandle> ListValue = pExecContext->VariableSymbolTable->Get("listB");

	if (DestList->Type != ValueType::List) {
		return InvalidMethodArgument(pExecContext, DestList->ToString(), "list as first argument");
	}

	if (ListValue->Type != ValueType::List) {
		return InvalidMethodArgument(pExecContext, ListValue->ToString(), "list as second argument");
	}

	Result->Success(DestList->MultiplyBy(ListValue));
	return Result;
}

SharedPtr<ParserRuntimeResult> BuiltInFuncHandle::NoVisitMethod(const SharedPtr<struct ParserContext> &pExecContext)
{
	auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
	auto Error = SharedPtr<RuntimeError>(new RuntimeError(
		pExecContext,
		StartPosition,
		EndPosition,
		fmt::format("No {0} method defined", Name->CStr()).c_str()
	));

	Result->Success(ExceptionHandle::Alloc<ExceptionHandle>(1, Error.Ptr()).Ptr());
	return Result;
}

SharedPtr<ParserRuntimeResult>
BuiltInFuncHandle::InvalidMethodArgument(const SharedPtr<struct ParserContext> &pExecContext, CStrPtr ArgName, CStrPtr ExpectedStr)
{
	auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
	auto Error = SharedPtr<RuntimeError>(new RuntimeError(
		pExecContext,
		StartPosition,
		EndPosition,
		fmt::format("Invalid argument {0} in {1} method. Expected {2}", ArgName, Name->CStr(), ExpectedStr).c_str()
	));

	Result->Success(ExceptionHandle::Alloc<ExceptionHandle>(1, Error.Ptr()).Ptr());
	return Result;
}

void BuiltInFuncHandle::AddMethod(
	CStrPtr MethodName,
	const BuiltInMethodType& MethodHandle,
	const std::vector<std::string> &MethodArgNameList)
{
	MethodMap[MethodName] = MethodHandle;
	MethodArgMap[MethodName] = MethodArgNameList;
}

SharedPtr<ValueHandle> BuiltInFuncHandle::Copy() const
{
	auto NewFunc = BuiltInFuncHandle::Alloc<BuiltInFuncHandle>();
	NewFunc->Create(Name->CStr());
	NewFunc->SetPosition(StartPosition, EndPosition);
	NewFunc->Context = Context;
	NewFunc->MethodMap = MethodMap;
	NewFunc->MethodArgMap = MethodArgMap;
	return NewFunc.Ptr();
}

CStrPtr BuiltInFuncHandle::ToString() {
	StringRep->Set(fmt::format("<function {0}>", Name->CStr()).c_str());
	return StringRep->CStr();
}
