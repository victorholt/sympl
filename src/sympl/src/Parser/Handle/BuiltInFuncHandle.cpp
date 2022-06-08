//
// GameSencha, LLC 5/31/22.
//
#include <sympl/include/Parser/Handle/BuiltInFuncHandle.hpp>
#include <sympl/include/Parser/Handle/StringHandle.hpp>
#include <sympl/include/Parser/Handle/IntHandle.hpp>
#include <sympl/include/Parser/Handle/ListHandle.hpp>
#include <sympl/include/Parser/Handle/FuncHandle.hpp>
#include <sympl/include/Parser/Handle/ExceptionHandle.hpp>
#include <sympl/include/Parser/ParserRuntimeResult.hpp>
#include <sympl/include/Parser/Interpreter.hpp>
#include <sympl/include/Parser/SymplVM.hpp>
#include <sympl/include/Parser/ParserContext.hpp>
#include <sympl/include/Parser/SymbolTable.hpp>
#include <sympl/include/Parser/Token.hpp>
#include <sympl/include/Parser/Node/ParserNode.hpp>
#include <sympl/include/Parser/Error/RuntimeError.hpp>
SymplNamespace

void BuiltInFuncHandle::__Construct(int argc, va_list ArgList)
{
    ValueHandle::__Construct(argc, ArgList);

    CStrPtr pFuncName = nullptr;

    if (argc > 0) {
        pFuncName = va_arg(ArgList, CStrPtr);
    }

	Create(pFuncName);

	// Create method/argument mappings.
	AddMethod(
		PrintFunc,
		[=](SharedPtr<ParserContext> pExecContext) { return ExecPrint(pExecContext); },
		{ "value" }
	);

	AddMethod(
		PrintRetFunc,
		[=](SharedPtr<ParserContext> pExecContext) { return ExecPrintRet(pExecContext); },
		{ "value" }
	);

	AddMethod(
		InputFunc,
		[=](SharedPtr<ParserContext> pExecContext) { return ExecInput(pExecContext); },
		{}
	);

	AddMethod(
		InputIntFunc,
		[=](SharedPtr<ParserContext> pExecContext) { return ExecInputInt(pExecContext); },
		{}
	);

	AddMethod(
		IsNumberFunc,
		[=](SharedPtr<ParserContext> pExecContext) { return ExecIsType(pExecContext, {ValueType::Int, ValueType::Float}); },
		{ "value" }
	);

	AddMethod(
		IsStringFunc,
		[=](SharedPtr<ParserContext> pExecContext) { return ExecIsType(pExecContext, {ValueType::String}); },
		{ "value" }
	);

	AddMethod(
		IsListFunc,
		[=](SharedPtr<ParserContext> pExecContext) { return ExecIsType(pExecContext, {ValueType::List}); },
		{ "value" }
	);

	AddMethod(
		IsFunctionFunc,
		[=](SharedPtr<ParserContext> pExecContext) { return ExecIsType(pExecContext, {ValueType::Func}); },
		{ "value" }
	);

	AddMethod(
		AppendFunc,
		[=](SharedPtr<ParserContext> pExecContext) { return ExecAppend(pExecContext); },
		{ "list", "value" }
	);

	AddMethod(
		RemoveFunc,
		[=](SharedPtr<ParserContext> pExecContext) { return ExecRemove(pExecContext); },
		{ "list", "index" }
	);

	AddMethod(
		ExtendFunc,
		[=](SharedPtr<ParserContext> pExecContext) { return ExecExtend(pExecContext); },
		{ "listA", "listB" }
	);

    AddMethod(
        LengthFunc,
        [=](SharedPtr<ParserContext> pExecContext) { return ExecLength(pExecContext); },
        { "value" }
    );

    AddMethod(
        StrFunc,
        [=](SharedPtr<ParserContext> pExecContext) { return ExecStr(pExecContext); },
        { "value" }
    );

    AddMethod(
        TimeFunc,
        [=](SharedPtr<ParserContext> pExecContext) { return ExecTime(pExecContext); },
        { }
    );

    AddMethod(
        ExportFunc,
        [=](SharedPtr<ParserContext> pExecContext) { return ExecExport(pExecContext); },
        { "value" }
    );

    AddMethod(
        IncludeFunc,
        [=](SharedPtr<ParserContext> pExecContext) { return ExecInclude(pExecContext); },
        { "file" }
    );

    AddMethod(
        RunFunc,
        [=](SharedPtr<ParserContext> pExecContext) { return ExecRun(pExecContext); },
        { "file" }
    );
}

void BuiltInFuncHandle::__Destruct()
{
    BaseFuncHandle::__Destruct();

    MethodMap.clear();
    MethodArgMap.clear();
}

void BuiltInFuncHandle::Create(CStrPtr pFuncName)
{
	Type = ValueType::Func;
	Name = StringBuffer::Alloc<StringBuffer>();
	Name->Set(pFuncName && strlen(pFuncName) > 0 ? pFuncName : "<anonymous>");
}

SharedPtr<ParserRuntimeResult> BuiltInFuncHandle::Exec(const std::vector<SharedPtr<ValueHandle>>& pArgValueList)
{
	auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
	auto ExecContext = GenerateNewContext(Name->CStr(), Context.Ptr());

	CStrPtr MethodName = Name->CStr();
	if (MethodMap.find(MethodName) == MethodMap.end() || MethodArgMap.find(MethodName) == MethodArgMap.end())
	{
		return NoVisitMethod(ExecContext);
	}

	Result->Register(CheckAndPopulateArgs(MethodArgMap[MethodName], pArgValueList, ExecContext));
	if (Result->ShouldReturn()) {
		return Result;
	}

	auto ResultValue = Result->Register(MethodMap[MethodName](ExecContext));
	if (Result->ShouldReturn()) {
		return Result;
	}

	Result->Success(ResultValue);
	return Result;
}

SharedPtr<ParserRuntimeResult> BuiltInFuncHandle::ExecPrint(SharedPtr<ParserContext> &pExecContext)
{
	auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
	auto ReturnValue = pExecContext->VariableSymbolTable->Get("value");

	std::cout << ReturnValue->ToString() << std::endl;

	Result->Success(ValueHandle::Null());
	return Result;
}

SharedPtr<ParserRuntimeResult> BuiltInFuncHandle::ExecPrintRet(SharedPtr<ParserContext> &pExecContext)
{
	auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
	SharedPtr<ValueHandle> ReturnValue = pExecContext->VariableSymbolTable->Get("value");

	if (ReturnValue->Type != ValueType::String) {
		ReturnValue = StringHandle::Alloc<StringHandle>(1, ReturnValue->ToString()).Ptr();
	}

	Result->Success(ReturnValue);
	return Result;
}

SharedPtr<ParserRuntimeResult> BuiltInFuncHandle::ExecInput(SharedPtr<ParserContext> &pExecContext)
{
	auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
	std::string InputStr;
	std::getline(std::cin, InputStr);

	auto ReturnValue = StringHandle::Alloc<StringHandle>(1, InputStr.c_str());
	Result->Success(ReturnValue.Ptr());

	return Result;
}

SharedPtr<ParserRuntimeResult> BuiltInFuncHandle::ExecInputInt(SharedPtr<ParserContext> &pExecContext)
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
	SharedPtr<ParserContext> &pExecContext,
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

SharedPtr<ParserRuntimeResult> BuiltInFuncHandle::ExecAppend(SharedPtr<ParserContext> &pExecContext)
{
	auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
	SharedPtr<ValueHandle> DestList = pExecContext->VariableSymbolTable->Get("list");
	SharedPtr<ValueHandle> ListValue = pExecContext->VariableSymbolTable->Get("value");

	if (DestList->Type != ValueType::List) {
		return InvalidMethodArgument(pExecContext, DestList->ToString(), "Expected list as first argument");
	}

	Result->Success(DestList->AddTo(ListValue));
	return Result;
}

SharedPtr<ParserRuntimeResult> BuiltInFuncHandle::ExecRemove(SharedPtr<ParserContext> &pExecContext)
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

SharedPtr<ParserRuntimeResult> BuiltInFuncHandle::ExecLength(SharedPtr<ParserContext>& pExecContext)
{
    auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
    SharedPtr<ValueHandle> Value = pExecContext->VariableSymbolTable->Get("value");

    if (Value->Type != ValueType::List && Value->Type != ValueType::String) {
        return InvalidMethodArgument(pExecContext, Value->ToString(), "Expected argument to be a list or string");
    }

    auto LengthValue = IntHandle::Alloc<IntHandle>();
    if (Value->Type == ValueType::List) {
        LengthValue->SetValue(ObjectRef::CastTo<ListHandle>(Value.Ptr())->Elements.size());
    }
    else {
        LengthValue->SetValue(Value->Value.String->Length());
    }

    Result->Success(LengthValue.Ptr());
    return Result;
}

SharedPtr<ParserRuntimeResult> BuiltInFuncHandle::ExecStr(SharedPtr<ParserContext>& pExecContext)
{
    auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
    SharedPtr<ValueHandle> Value = pExecContext->VariableSymbolTable->Get("value");

    auto ReturnValue = StringHandle::Alloc<StringHandle>(1, Value->ToString());
    Result->Success(ReturnValue.Ptr());
    return Result;
}

SharedPtr<ParserRuntimeResult> BuiltInFuncHandle::ExecTime(SharedPtr<ParserContext>& pExecContext)
{
    auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();

    typedef std::chrono::high_resolution_clock Time;
    typedef std::chrono::milliseconds ms;
    typedef std::chrono::duration<float> fsec;
    auto CurrentTime = Time::now().time_since_epoch();
    ms Duration = std::chrono::duration_cast<ms>(CurrentTime);

    auto ReturnValue = IntHandle::Alloc<IntHandle>();
    ReturnValue->SetValue(static_cast<long>(Duration.count()));

    Result->Success(ReturnValue.Ptr());
    return Result;
}

SharedPtr<ParserRuntimeResult> BuiltInFuncHandle::ExecExtend(SharedPtr<ParserContext> &pExecContext)
{
	auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
	SharedPtr<ValueHandle> DestList = pExecContext->VariableSymbolTable->Get("listA");
	SharedPtr<ValueHandle> ListValue = pExecContext->VariableSymbolTable->Get("listB");

	if (!DestList.IsValid() || DestList->Type != ValueType::List) {
		return InvalidMethodArgument(pExecContext, DestList->ToString(), "Expected list as first argument");
	}

	if (!ListValue.IsValid() || ListValue->Type != ValueType::List) {
		return InvalidMethodArgument(pExecContext, ListValue->ToString(), "Expected list as second argument");
	}

	Result->Success(DestList->MultiplyBy(ListValue));
	return Result;
}

SharedPtr<ParserRuntimeResult> BuiltInFuncHandle::ExecExport(SharedPtr<ParserContext>& pExecContext)
{
    auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
    SharedPtr<ValueHandle> ExportValue = pExecContext->VariableSymbolTable->Get("value");

    if (ExportValue->Type != ValueType::Func) {
        return InvalidMethodArgument(pExecContext, ExportValue->ToString(), "Exported values must be functions");
    }

    auto ParentContext = pExecContext->Parent;
    while (true) {
        auto NextContext = ParentContext->Parent;
        if (!NextContext.IsValid()) {
            break;
        }
        ParentContext = NextContext;
    }

    if (ParentContext.IsValid()) {
        ParentContext->VariableSymbolTable->Set(
            ObjectRef::CastTo<FuncHandle>(ExportValue.Ptr())->Name->CStr(),
            ExportValue
        );
    }

    Result->Success(ValueHandle::Null());
    return Result;
}

SharedPtr<ParserRuntimeResult> BuiltInFuncHandle::ExecInclude(SharedPtr<ParserContext>& pExecContext)
{
    auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
    SharedPtr<ValueHandle> FileNameValue = pExecContext->VariableSymbolTable->Get("file");

    if (FileNameValue->Type != ValueType::String) {
        return InvalidMethodArgument(pExecContext, FileNameValue->ToString(), "Expected filename to be a string");
    }

    std::ifstream inputStream (FileNameValue->Value.String->CStr(), std::ifstream::in);
    if (!inputStream.is_open()) {
        return InvalidMethodArgument(pExecContext, FileNameValue->ToString(), "Unable to open file.");
    }

    std::string str;

    inputStream.seekg(0, std::ios::end);
    str.reserve(inputStream.tellg());
    inputStream.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(inputStream)),
               std::istreambuf_iterator<char>());

    // Close the stream.
    inputStream.close();

    auto VM = SymplVM::Alloc<SymplVM>(1, pExecContext.Ptr());
    auto ReturnTuple = VM->RunScript(FileNameValue->Value.String->CStr(), str.c_str());
    auto ReturnError = std::get<1>(ReturnTuple);

    if (ReturnError.IsValid()) {
        return InvalidMethodArgument(
            pExecContext,
            FileNameValue->ToString(),
            fmt::format("Failed execute script {0}.\n{1}", FileNameValue->ToString(), ReturnError->ToString()).c_str()
        );
    }

    Result->Success(ValueHandle::Null());
    return Result;
}

SharedPtr<ParserRuntimeResult> BuiltInFuncHandle::ExecRun(SharedPtr<ParserContext>& pExecContext)
{
    auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
    SharedPtr<ValueHandle> FileNameValue = pExecContext->VariableSymbolTable->Get("file");

    if (FileNameValue->Type != ValueType::String) {
        return InvalidMethodArgument(pExecContext, FileNameValue->ToString(), "Expected filename to be a string");
    }

    std::ifstream inputStream (FileNameValue->Value.String->CStr(), std::ifstream::in);
    if (!inputStream.is_open()) {
        return InvalidMethodArgument(pExecContext, FileNameValue->ToString(), "Unable to open file.");
    }

    std::string str;

    inputStream.seekg(0, std::ios::end);
    str.reserve(inputStream.tellg());
    inputStream.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(inputStream)),
               std::istreambuf_iterator<char>());

    // Close the stream.
    inputStream.close();

	{
		auto VM = SymplVM::Alloc<SymplVM>();
        VM->IsChildVM = true;

        auto ReturnTuple = VM->RunScript(FileNameValue->Value.String->CStr(), str.c_str());
        auto ReturnError = std::get<1>(ReturnTuple);

		if (ReturnError.IsValid()) {
			return InvalidMethodArgument(
					VM->GetContext(),
					FileNameValue->ToString(),
					fmt::format("Failed execute script {0}.\n{1}", FileNameValue->ToString(),
								ReturnError->ToString()).c_str()
			);
		}

		Result->Success(ValueHandle::Null());
	}

    return Result;
}

SharedPtr<ParserRuntimeResult> BuiltInFuncHandle::NoVisitMethod(const SharedPtr<ParserContext> &pExecContext)
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
BuiltInFuncHandle::InvalidMethodArgument(const SharedPtr<ParserContext> &pExecContext, CStrPtr ArgName, CStrPtr ExpectedStr)
{
	auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
	Result->Error = SharedPtr<RuntimeError>(new RuntimeError(
		pExecContext,
		StartPosition,
		EndPosition,
		fmt::format("Invalid argument '{0}' in '{1}' method. {2}", ArgName, Name->CStr(), ExpectedStr).c_str()
	)).Ptr();

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
