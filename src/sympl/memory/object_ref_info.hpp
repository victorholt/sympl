//
// GameSencha, LLC 1/31/24.
//
#pragma once
#include <sympl/sympl_pch.hpp>

SymplNamespaceStart

class SYMPL_API ObjectRefInfo {
private:
	/// Type name.
	std::string _type_name;

	/// Base class type info.
	const ObjectRefInfo* _base_type_info;

public:
	//! Constructor.
	//! \param p_type_name
	//! \param p_base_type_info
	ObjectRefInfo(const char* p_type_name, const ObjectRefInfo* p_base_type_info);

	//! Destructor.
	~ObjectRefInfo();

	//! Check current type is type of specified type.
	//! \param type
	//! \return bool
	bool is_type_of(std::string type) const;

	//! Check current type is type of specified type.
	//! \param p_type_info
	//! \return bool
	bool is_type_of(const ObjectRefInfo* p_type_info) const;

	//! Check current type is type of specified class type.
	//! \return bool
	template<typename T>
	inline bool is_type_of() const { return is_type_of(T::get_type_info_static()); }

	/// Return type name.
	inline const std::string& get_type_name() const { return _type_name;}

	/// Return base type info.
	inline const ObjectRefInfo* get_base_type_info() const { return _base_type_info; }
};

#define SYMPL_OBJECT(type_name, base_type_name) \
public: \
using ClassName = type_name; \
using BaseClassName = base_type_name; \
virtual const std::string& get_type_name() const override { return get_type_info_static()->get_type_name(); } \
virtual const Sympl::ObjectRefInfo* GetTypeInfo() const override { return get_type_info_static(); } \
static const std::string& get_type_name_static() { return get_type_info_static()->get_type_name(); } \
static const Sympl::ObjectRefInfo* get_type_info_static() { static const Sympl::ObjectRefInfo type_info_static(#type_name, BaseClassName::get_type_info_static()); return &type_info_static; } \

SymplNamespaceEnd