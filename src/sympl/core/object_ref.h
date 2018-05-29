/**********************************************************
 * Author:  GameSencha, LLC
 * The MIT License (MIT)
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 *
 **********************************************************/
#pragma once

#include <sympl/core/sympl_pch.h>
#include <sympl/core/alloc_manager.h>

sympl_nsstart

// Pattern from Urho3D.
class SYMPL_API RefInfo {
private:
    /// Type name.
    std::string _TypeName;

    /// Base class type info.
    const RefInfo* _BaseTypeInfo;

public:
    //! Constructor.
    //! \param typeName
    //! \param baseTypeInfo
    RefInfo(const char* typeName, const RefInfo* baseTypeInfo);

    //! Destructor.
    ~RefInfo();

    //! Check current type is type of specified type.
    //! \param type
    //! \return bool
    bool IsTypeOf(std::string type) const;

    //! Check current type is type of specified type.
    //! \param typeInfo
    //! \return bool
    bool IsTypeOf(const RefInfo* typeInfo) const;

    //! Check current type is type of specified class type.
    //! \return bool
    template<typename T>
    inline bool IsTypeOf() const { return IsTypeOf(T::GetTypeInfoStatic()); }

    /// Return type name.
    inline const std::string& GetTypeName() const { return _TypeName;}

    /// Return base type info.
    inline const RefInfo* GetBaseTypeInfo() const { return _BaseTypeInfo; }
};

#define SYMPL_OBJECT(typeName, baseTypeName) \
    public: \
        using ClassName = typeName; \
        using BaseClassName = baseTypeName; \
        virtual const std::string& GetTypeName() const override { return GetTypeInfoStatic()->GetTypeName(); } \
        virtual const Sympl::RefInfo* GetTypeInfo() const override { return GetTypeInfoStatic(); } \
        static const std::string& GetTypeNameStatic() { return GetTypeInfoStatic()->GetTypeName(); } \
        static const Sympl::RefInfo* GetTypeInfoStatic() { static const Sympl::RefInfo typeInfoStatic(#typeName, BaseClassName::GetTypeInfoStatic()); return &typeInfoStatic; } \

class SYMPL_API ObjectRef {
protected:
    /// Current reference count before we can delete the reference.
    unsigned _RefCount = 0;

    /// Size of the memory.
    size_t _MemSize = 0;

    /// Memory address for the reference.
    size_t _MemIndex = 0;

    /// Guid for the reference.
    std::string _Guid;

public:
    // Attempts to dec the reference count.
    virtual ~ObjectRef() { Release(); }

    //! Sets the memory index.
    //! \param index
    inline void SetMemIndex(size_t index) { _MemIndex = index; }

    //! Returns the block memory index for the object.
    inline const size_t GetMemIndex() const { return _MemIndex; }

    //! Returns the guid for the object.
    inline const std::string Guid() const { return _Guid; }

    //! Adds to the reference count.
    inline void AddRef() { _RefCount++; }

    //! Returns the reference count.
    //! \return
    inline unsigned RefCount() const { return _RefCount; }

    //! Attempts to free the reference object.
    virtual bool Release();

    //! Return type name.
    //! \return string
    virtual const std::string& GetTypeName() const = 0;

    //! Return type info.
    //! \return RefInfo
    virtual const RefInfo* GetTypeInfo() const = 0;

    //! Return type info static.
    //! \return RefInfo
    static const RefInfo* GetTypeInfoStatic() { return nullptr; }
};

sympl_nsend
