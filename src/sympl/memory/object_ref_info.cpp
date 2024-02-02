//
// GameSencha, LLC 1/31/24.
//
#include "object_ref_info.hpp"
SymplNamespace

ObjectRefInfo::ObjectRefInfo(const char* p_type_name, const ObjectRefInfo* p_base_type_info)
		:   _type_name(p_type_name),
			_base_type_info(p_base_type_info)
{

}

ObjectRefInfo::~ObjectRefInfo() = default;

bool ObjectRefInfo::is_type_of(std::string type) const
{
	const ObjectRefInfo* current = this;

	while (current)
	{
		if (current->get_type_name() == type)
			return true;

		current = current->get_base_type_info();
	}

	return false;
}

bool ObjectRefInfo::is_type_of(const ObjectRefInfo* p_type_info) const
{
	const ObjectRefInfo* current = this;

	while (current)
	{
		if (current == p_type_info)
			return true;

		current = current->get_base_type_info();
	}

	return false;
}
