//
// GameSencha, LLC 5/25/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>

SymplNamespaceStart

class SYMPL_API ObjectRefInfo {
private:
	/// Type name.
	std::string _TypeName;

	/// Base class type info.
	const ObjectRefInfo* _BaseTypeInfo;

public:
	//! Constructor.
	//! \param typeName
	//! \param baseTypeInfo
	ObjectRefInfo(const char* typeName, const ObjectRefInfo* baseTypeInfo);

	//! Destructor.
	~ObjectRefInfo();

	//! Check current type is type of specified type.
	//! \param type
	//! \return bool
	bool IsTypeOf(std::string type) const;

	//! Check current type is type of specified type.
	//! \param typeInfo
	//! \return bool
	bool IsTypeOf(const ObjectRefInfo* typeInfo) const;

	//! Check current type is type of specified class type.
	//! \return bool
	template<typename T>
	inline bool IsTypeOf() const { return IsTypeOf(T::GetTypeInfoStatic()); }

	/// Return type name.
	inline const std::string& GetTypeName() const { return _TypeName;}

	/// Return base type info.
	inline const ObjectRefInfo* GetBaseTypeInfo() const { return _BaseTypeInfo; }
};

#define SYMPL_OBJECT(typeName, baseTypeName) \
    public: \
        using ClassName = typeName; \
        using BaseClassName = baseTypeName; \
        virtual const std::string& GetTypeName() const override { return GetTypeInfoStatic()->GetTypeName(); } \
        virtual const Sympl::ObjectRefInfo* GetTypeInfo() const override { return GetTypeInfoStatic(); } \
        static const std::string& GetTypeNameStatic() { return GetTypeInfoStatic()->GetTypeName(); } \
        static const Sympl::ObjectRefInfo* GetTypeInfoStatic() { static const Sympl::ObjectRefInfo typeInfoStatic(#typeName, BaseClassName::GetTypeInfoStatic()); return &typeInfoStatic; } \

SymplNamespaceEnd
