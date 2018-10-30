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
#include <sympl/core/object.h>
#include <sympl/core/variant.h>
#include <sympl/core/shared_ptr.h>
#include <sympl/script/script_common.h>

sympl_nsstart

class StringBuffer;
class ScriptVM;

class SYMPL_API ScriptObject : public Object
{
    SYMPL_OBJECT(ScriptObject, Object);

protected:
    /// Reference object this used by this object.
    SharedPtr<ScriptObject> _Reference;

    /// Parent reference for the object.
    SharedPtr<ScriptObject> _Parent;

    /// Name of the object.
    std::string _Name;

    /// Clean non-unique name
    std::string _CleanName;

    /// Reference namespace path.
    std::string _Path;

    /// The script object type.
    ScriptObjectType _Type = ScriptObjectType::Empty;

    /// Children added to the object in the order they
    /// were added.
    std::vector< SharedPtr<ScriptObject> > _Children;

    /// Value of the object.
    Variant _Value;

public:
    //! Constructor.
    ScriptObject();

    //! Called in place of the constructor.
    void __Construct() override;

    //! Initializes the object.
    //! \param name
    //! \param path
    //! \param type
    //! \param parent
    void Initialize(const std::string& name, const std::string& path, ScriptObjectType type, ScriptObject* parent = nullptr);

    //! Adds a child to the object.
    //! \param ref
    void AddChild(ScriptObject* ref);

    //! Removes a child from the object.
    //! \param ref
    void RemoveChild(ScriptObject* ref);

    //! Returns whether or not a reference is a child.
    //! \param ref
    //! \return bool
    bool IsChild(ScriptObject* ref);

    //! Attempts to find a child by a given name.
    //! \param name
    //! \param traverseUp
    //! \return ScriptObjectRef
    ScriptObject* FindChildByName(const std::string& name, bool traverseUp = true);

    //! Attempts to find a child by a given path name.
    //! \param path
    //! \param traverseUp
    //! \return ScriptObjectRef
    ScriptObject* FindChildByPath(const std::string& path, bool traverseUp = true);

    //! Attempts to find a child by a given address.
    //! \param address
    //! \param traverseUp
    //! \return ScriptObjectRef
    ScriptObject* FindChildByAddress(MemAddressType address, bool traverseUp = true);

    //! Returns the type as a string.
    //! \return string
    std::string GetTypeAsString() const;

    //! Releases the object.
    bool Release() override;

    //! Sets the parent for the object.
    //! \param parent
    inline void SetParent(ScriptObject* parent) {
        _Parent = parent;
    }

    //! Returns the parent object.
    //! \return ScriptObject
    inline const SharedPtr<ScriptObject>& GetParent() const { return _Parent; }

    //! Sets the name for the object.
    //! \param name
    inline void SetName(const std::string& name) { _Name = name; }

    //! Returns the name of the object.
    //! \return string
    inline const std::string& GetName() const { return _Name; }

    //! Sets the clean name for the object.
    //! \param name
    inline void SetCleanName(const std::string& name) { _CleanName = name; }

    //! Returns the clean name of the object.
    //! \return string
    inline const std::string& GetCleanName() const { return _CleanName; }

    //! Sets the path for the object.
    //! \param path
    inline void SetPath(const std::string& path) { _Path = path; }

    //! Returns the path of the object.
    //! \return
    inline const std::string& GetPath() const { return _Path; }

    //! Sets the type of the object.
    //! \param type
    inline void SetType(ScriptObjectType type) { _Type = type; }

    //! Returns the type of the object.
    //! \return
    inline ScriptObjectType  GetType() const { return _Type; }

    //! Whether or not this is an empty object.
    //! \return bool
    inline bool IsEmpty() const { return _Type == ScriptObjectType::Empty; }

    //! Whether or not this is a variable object.
    //! \return bool
    inline bool IsVariable() const { return _Type == ScriptObjectType::Object; }

    //! Whether or not this is an array object.
    //! \return bool
    inline bool IsArray() const { return _Type == ScriptObjectType::Array; }

    //! Whether or not this is a method object.
    //! \return bool
    inline bool IsMethod() const { return _Type == ScriptObjectType::Method; }

    //! Returns whether or not this is a referenced object.
    //! \return bool
    inline bool IsReference() const { return _Reference.IsValid(); }

    //! Returns the child objects.
    //! \return std::unordered_map<string, ScriptObject*>
    inline const std::vector< SharedPtr<ScriptObject> >& GetChildren() const {
        return _Children;
    }

    //! Sets the value of the object.
    //! \param value
    inline void SetValue(const Variant& value) {
        _Value = value;
    }

    //! Script value variant.
    //! \return Variant
    inline const Variant& GetValue() const {
        return _Value;
    }

    //! Sets the reference for this object.
    //! \param value
    inline void SetReference(ScriptObject* value) { _Reference = value; }

    //! Returns the reference from this script object.
    //! \return ScriptObject
    inline ScriptObject* GetReference() const { return _Reference.IsValid() ? _Reference.Ptr() : &ScriptObject::Empty; }

    /// Empty script object.
    static ScriptObject Empty;

    /// Ensure the VM can access the private methods.
    friend class ScriptVM;
};

sympl_nsend
