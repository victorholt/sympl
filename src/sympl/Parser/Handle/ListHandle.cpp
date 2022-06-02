//
// GameSencha, LLC 5/31/22.
//
#include "ListHandle.hpp"
#include "NullHandle.hpp"
#include "ExceptionHandle.hpp"
#include <sympl/Parser/Error/RuntimeError.hpp>
#include <sympl/Parser/Node/ParserListNode.hpp>
#include <sympl/Parser/Token.hpp>
SymplNamespace

void ListHandle::__Construct(int argc, va_list ArgList)
{
}

void ListHandle::Create(const std::vector<SharedPtr<ValueHandle>>& pElements)
{
    Type = ValueType::List;
    Elements = pElements;
}

SharedPtr<ValueHandle> ListHandle::AddTo(const SharedPtr <ValueHandle>& pHandle)
{
    SharedPtr<ListHandle> Result = dynamic_cast<ListHandle*>(Copy().Ptr());
	Result->Elements.emplace_back(pHandle);
    return Result.Ptr();
}

SharedPtr<ValueHandle> ListHandle::SubtractBy(const SharedPtr<ValueHandle> &pHandle)
{
	if (pHandle->Type != ValueType::Int) {
		return ValueHandle::Null();
	}

	SharedPtr<ListHandle> Result = dynamic_cast<ListHandle*>(Copy().Ptr());
	auto Index = pHandle->Value.IntNum;

	if (Index >= Elements.size() || Index < 0) {
		auto Error = SharedPtr<RuntimeError>(new RuntimeError(
			Context,
			StartPosition,
			EndPosition,
			fmt::format("Element at index '{0}' in <list> could not be removed. Index out of bounds.", Index).c_str()
		));
		return ExceptionHandle::Alloc<ExceptionHandle>(1, Error.Ptr()).Ptr();
	}

	Elements.erase(Elements.begin() + Index);

	return ValueHandle::SubtractBy(pHandle);
}

SharedPtr<ValueHandle> ListHandle::MultiplyBy(const SharedPtr<class ValueHandle>& pHandle)
{
	if (pHandle->Type != ValueType::List) {
		return ValueHandle::Null();
	}

	SharedPtr<ListHandle> Result = dynamic_cast<ListHandle*>(Copy().Ptr());
	SharedPtr<ListHandle> OtherList = dynamic_cast<ListHandle*>(pHandle.Ptr());
	for (const auto& Item : OtherList->Elements) {
		Result->Elements.emplace_back(Item);
	}

    return Result.Ptr();
}

SharedPtr<ValueHandle> ListHandle::DivideBy(const SharedPtr<ValueHandle> &pHandle)
{
	if (pHandle->Type != ValueType::Int) {
		return ValueHandle::Null();
	}

	auto Index = pHandle->Value.IntNum;
	if (Index >= Elements.size() || Index < 0) {
		auto Error = SharedPtr<RuntimeError>(new RuntimeError(
			Context,
			StartPosition,
			EndPosition,
			fmt::format("Element at index '{0}' in <list> could not be retrieved. Index out of bounds.", Index).c_str()
		));
		return ExceptionHandle::Alloc<ExceptionHandle>(1, Error.Ptr()).Ptr();
	}


	return Elements[Index];
}

void ListHandle::NormalizeValue()
{
    Value.IntNum = !Elements.empty() ? 1 : 0;
    Value.FloatNum = !Elements.empty()  ? 1.f : 0.f;
}

bool ListHandle::IsTrue() const
{
    return !Elements.empty();
}

SharedPtr<ValueHandle> ListHandle::Copy() const
{
    auto Result = ListHandle::Alloc<ListHandle>();
    Result->NormalizeValue();
    Result->SetPosition(StartPosition, EndPosition);
    Result->Context = Context;
	Result->Elements = Elements;
    return Result.Ptr();
}

CStrPtr ListHandle::ToString() {
	StringRep->Set("[");

	auto ElementIndex = 0;
    for (auto& Item : Elements) {
		StringRep->Append(Item->ToString());

		if (ElementIndex < Elements.size() - 1) {
			StringRep->Append(", ");
		}

		ElementIndex++;
    }

	StringRep->AppendByte(']');

    return StringRep->CStr();
}
