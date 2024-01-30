//
// Created by Victor on 5/25/2022.
//
#include <__sympl_old/include/Parser/Handle/NumberHandle.hpp>
SymplNamespace

bool NumberHandle::IsTrue() const
{
	return Value.IntNum != 0 && static_cast<int>(Value.FloatNum) != 0;
}
