//
// GameSencha, LLC 5/31/22.
//
#include <sympl/include/Parser/Handle/FuncHandle.hpp>
#include <sympl/include/Parser/ParserRuntimeResult.hpp>
#include <sympl/include/Parser/Interpreter.hpp>
#include <sympl/include/Parser/ParserContext.hpp>
#include <sympl/include/Parser/SymbolTable.hpp>
#include <sympl/include/Parser/Token.hpp>
#include <sympl/include/Parser/Node/ParserNode.hpp>
#include <sympl/include/Parser/Error/RuntimeError.hpp>
SymplNamespace

void FuncHandle::__Construct(int argc, va_list ArgList)
{
    SharedPtr<ParserNode> pBodyNode;
    Create("<anonymous>", pBodyNode, {});
}

void FuncHandle::Create(
    CStrPtr FuncName,
    const SharedPtr<ParserNode>& pBodyNode,
    const std::vector<std::string>& pArgNameList
)
{
    Type = ValueType::Func;
    Name = StringBuffer::Alloc<StringBuffer>();
    Name->Set(FuncName && strlen(FuncName) > 0 ? FuncName : "<anonymous>");
    BodyNode = pBodyNode;
    ArgNameList = pArgNameList;
}

SharedPtr<ParserRuntimeResult> FuncHandle::Exec(const std::vector<SharedPtr<ValueHandle>>& ArgValueList)
{
    auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
    auto Interp = Interpreter::Alloc<Interpreter>();
    auto ExecContext = GenerateNewContext(Name->CStr(), Context);

	Result->Register(CheckAndPopulateArgs(ArgNameList, ArgValueList, ExecContext));
	if (Result->ShouldReturn()) {
		return Result;
	}

    auto ResultValue = Result->Register(Interp->Visit(BodyNode, ExecContext));
    if (Result->ShouldReturn() && !Result->FuncReturnValue.IsValid()) {
        return Result;
    }

    SharedPtr<ValueHandle> ReturnValue = ResultValue;
    if (!ShouldAutoReturn) {
        ReturnValue = Result->FuncReturnValue.IsValid() ? Result->FuncReturnValue : ValueHandle::Null();
    }
    Result->Success(ReturnValue);
    return Result;
}

SharedPtr<ValueHandle> FuncHandle::Copy() const
{
    auto NewFunc = FuncHandle::Alloc<FuncHandle>();
    NewFunc->Create(Name->CStr(), BodyNode, ArgNameList);
    NewFunc->SetPosition(StartPosition, EndPosition);
    NewFunc->Context = Context;
    NewFunc->ShouldAutoReturn = ShouldAutoReturn;
    return NewFunc.Ptr();
}

CStrPtr FuncHandle::ToString() {
	StringRep->Set(fmt::format("<function {0}>", Name->CStr()).c_str());
    return StringRep->CStr();
}
