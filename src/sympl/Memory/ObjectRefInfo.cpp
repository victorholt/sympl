//
// GameSencha, LLC 6/1/22.
//
#include "ObjectRefInfo.hpp"
SymplNamespace

ObjectRefInfo::ObjectRefInfo(const char* typeName, const ObjectRefInfo* baseTypeInfo)
		:   _TypeName(typeName),
			_BaseTypeInfo(baseTypeInfo)
{

}

ObjectRefInfo::~ObjectRefInfo() = default;

bool ObjectRefInfo::IsTypeOf(std::string type) const
{
	const ObjectRefInfo* current = this;

	while (current)
	{
		if (current->GetTypeName() == type)
			return true;

		current = current->GetBaseTypeInfo();
	}

	return false;
}

bool ObjectRefInfo::IsTypeOf(const ObjectRefInfo* typeInfo) const
{
	const ObjectRefInfo* current = this;

	while (current)
	{
		if (current == typeInfo)
			return true;

		current = current->GetBaseTypeInfo();
	}

	return false;
}