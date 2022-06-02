//
// GameSencha, LLC 5/31/22.
//
#include "FuncHandle.hpp"
#include <sympl/Parser/ParserRuntimeResult.hpp>
#include <sympl/Parser/Interpreter.hpp>
#include <sympl/Parser/ParserContext.hpp>
#include <sympl/Parser/SymbolTable.hpp>
#include <sympl/Parser/Token.hpp>
#include <sympl/Parser/Node/ParserNode.hpp>
#include <sympl/Parser/Error/RuntimeError.hpp>
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
    Name->Set(FuncName);
    BodyNode = pBodyNode;
    ArgNameList = pArgNameList;
}

SharedPtr<ParserRuntimeResult> FuncHandle::Exec(const std::vector<SharedPtr<ValueHandle>>& ArgValueList)
{
    auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
    auto Interp = Interpreter::Alloc<Interpreter>();
    auto NewContext = ParserContext::Alloc<ParserContext>();

    NewContext->Create(Context, StartPosition, Name->CStr());
    NewContext->VariableSymbolTable = SymbolTable::Alloc<SymbolTable>(1, Context->VariableSymbolTable->Parent.Ptr());

    if (ArgValueList.size() > ArgNameList.size()) {
        Result->Error = SharedPtr<RuntimeError>(new RuntimeError(
            Context,
            StartPosition,
            EndPosition,
            fmt::format(
                "{0} Too many args passed into '{1}'",
                ArgValueList.size() - ArgNameList.size(),
                Name->CStr()
            ).c_str()
        )).Ptr();
        return Result;
    }

    if (ArgValueList.size() < ArgNameList.size()) {
        Result->Error = SharedPtr<RuntimeError>(new RuntimeError(
            Context,
            StartPosition,
            EndPosition,
            fmt::format(
                "{0} Too few args passed into '{1}'",
                ArgNameList.size() - ArgValueList.size(),
                Name->CStr()
            ).c_str()
        )).Ptr();
        return Result;
    }

    for (auto i = 0; i < ArgNameList.size(); ++i)
    {
        auto ArgName = ArgNameList[i];
        auto ArgValue = ArgValueList[i];
        ArgValue->Context = NewContext;
        NewContext->VariableSymbolTable->Set(ArgName.c_str(), ArgValue);
    }

    auto ResultValue = Result->Register(Interp->Visit(BodyNode, NewContext));
    if (Result->Error.IsValid()) {
        return Result;
    }

    Result->Success(ResultValue);
    return Result;
}

SharedPtr<ValueHandle> FuncHandle::Copy() const
{
    auto NewFunc = FuncHandle::Alloc<FuncHandle>();
    NewFunc->Create(Name->CStr(), BodyNode, ArgNameList);
    NewFunc->SetPosition(StartPosition, EndPosition);
    NewFunc->Context = Context;
    return NewFunc.Ptr();
}

CStrPtr FuncHandle::ToString() {
    memset(TmpNumber_Allocation, 0, sizeof(TmpNumber_Allocation));
    strcpy(TmpNumber_Allocation, fmt::format("<function {0}>", Name->CStr()).c_str());
    return TmpNumber_Allocation;
}
