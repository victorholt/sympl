//
// GameSencha, LLC 5/31/22.
//
#include "BaseFuncHandle.hpp"
#include <sympl/Parser/ParserRuntimeResult.hpp>
#include <sympl/Parser/Interpreter.hpp>
#include <sympl/Parser/ParserContext.hpp>
#include <sympl/Parser/SymbolTable.hpp>
#include <sympl/Parser/Error/RuntimeError.hpp>
SymplNamespace

void BaseFuncHandle::__Construct(int argc, va_list ArgList)
{
	CStrPtr pValue = va_arg(ArgList, CStrPtr);
	Create(pValue);
}

void BaseFuncHandle::Create(CStrPtr FuncName)
{
	Type = ValueType::Func;
	Name = StringBuffer::Alloc<StringBuffer>();
	Name->Set(FuncName && strlen(FuncName) > 0 ? FuncName : "<anonymous>");
}

SharedPtr<ParserContext> BaseFuncHandle::GenerateNewContext() const
{
	auto NewContext = ParserContext::Alloc<ParserContext>();
	NewContext->Create(Context, StartPosition, Name->CStr());
//	NewContext->VariableSymbolTable = SymbolTable::Alloc<SymbolTable>(1, Context->VariableSymbolTable->Parent.Ptr());
	return NewContext;
}

SharedPtr<ParserRuntimeResult> BaseFuncHandle::CheckArgs(const std::vector<std::string> &pArgNameList, const std::vector<SharedPtr<ValueHandle>>& pArgValueList)
{
	auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();

	if (pArgNameList.size() > pArgValueList.size()) {
		Result->Error = SharedPtr<RuntimeError>(new RuntimeError(
			Context,
			StartPosition,
			EndPosition,
			fmt::format(
					"{0} Too many args passed into '{1}'",
					pArgValueList.size() - pArgNameList.size(),
					Name->CStr()
			).c_str()
		)).Ptr();
		return Result;
	}

	if (pArgValueList.size() < pArgNameList.size()) {
		Result->Error = SharedPtr<RuntimeError>(new RuntimeError(
			Context,
			StartPosition,
			EndPosition,
			fmt::format(
					"{0} Too few args passed into '{1}'",
					pArgNameList.size() - pArgValueList.size(),
					Name->CStr()
			).c_str()
		)).Ptr();
		return Result;
	}

	Result->Success(ValueHandle::Null());
	return Result;
}

void BaseFuncHandle::PopulateArgs(
	const std::vector<std::string>& pArgNameList,
	const std::vector<SharedPtr<ValueHandle>>& pArgValueList,
	SharedPtr<class ParserContext> ExecContext
) {
	for (auto i = 0; i < pArgNameList.size(); ++i)
	{
		auto ArgName = pArgNameList[i];
		auto ArgValue = pArgValueList[i];
		ArgValue->Context = ExecContext;
		ExecContext->VariableSymbolTable->Set(ArgName.c_str(), ArgValue);
	}
}

SharedPtr<ParserRuntimeResult> BaseFuncHandle::CheckAndPopulateArgs(
	const std::vector<std::string> &pArgNameList,
	const std::vector<SharedPtr<ValueHandle>> &pArgValueList,
  	SharedPtr<struct ParserContext> ExecContext)
{
	auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();

	Result->Register(CheckArgs(pArgNameList, pArgValueList));
	if (Result->Error.IsValid()) {
		return Result;
	}

	PopulateArgs(pArgNameList, pArgValueList, ExecContext);

	Result->Success(ValueHandle::Null());
	return Result;
}

SharedPtr<ParserRuntimeResult> BaseFuncHandle::Exec(const std::vector<SharedPtr<ValueHandle>>& ArgValueList)
{
	auto Result = ParserRuntimeResult::Alloc<ParserRuntimeResult>();
	Result->Success(ValueHandle::Null());
	return Result;
}

CStrPtr BaseFuncHandle::ToString() {
	StringRep->Set(fmt::format("<function {0}>", Name->CStr()).c_str());
	return StringRep->CStr();
}
