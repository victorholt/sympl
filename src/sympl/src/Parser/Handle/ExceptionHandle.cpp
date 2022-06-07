//
// GameSencha, LLC 5/31/22.
//
#include <sympl/include/Parser/Handle/ExceptionHandle.hpp>
#include <sympl/include/Parser/Node/ParserNode.hpp>
#include <sympl/include/Parser/Error/RuntimeError.hpp>
SymplNamespace

void ExceptionHandle::__Construct(int argc, va_list ArgList)
{
    ValueHandle::__Construct(argc, ArgList);

	SharedPtr<RuntimeError> pError = va_arg(ArgList, RuntimeError*);
	Create(pError);
}

void ExceptionHandle::Create(const SharedPtr<RuntimeError>& pError)
{
	Type = ValueType::Exception;
	Error = pError;
}

SharedPtr<ValueHandle> ExceptionHandle::Copy() const
{
	auto Result = ExceptionHandle::Alloc<ExceptionHandle>(1, Error.Ptr());
	Result->Context = Context;
	Result->SetPosition(StartPosition, EndPosition);
	return Result.Ptr();
}

CStrPtr ExceptionHandle::ToString() {
	StringRep->Set(fmt::format(
		"<exception> {0}",
		Error.IsValid() ? Error->ToString() : "Unknown error thrown"
	).c_str());

	return StringRep->CStr();
}
