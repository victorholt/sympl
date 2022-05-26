//
// Created by Victor on 5/25/2022.
//
#include "NumberHandle.hpp"
SymplNamespace

void NumberHandle::NormalizeValue()
{
    switch (Type)
    {
        case ValueType::Int:
            Value.FloatNum = static_cast<float>(Value.IntNum);
            break;
        case ValueType::Float:
            Value.IntNum = static_cast<int>(Value.FloatNum);
            break;
        case ValueType::Null:
            break;
    }
}
