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
#include <sympl/core/weak_ptr.h>
#include <sympl/script/script_common.h>
#include <sympl/script/script_context.h>

sympl_nsstart

class StringBuffer;
class ScriptVM;

class SYMPL_API ScriptObject : public Object
{
    SYMPL_OBJECT(ScriptObject, Object);

protected:
    /// Parent reference for the object.
    WeakPtr<ScriptObject> _Parent;

    /// Reference to the script context.
    SharedPtr<ScriptContext> _Context;

    /// Value of the script object.
    Variant _Value;

    /// Children added to the object in the order they
    /// were added.
    std::vector< SharedPtr<ScriptObject> > _Children;

    /// Name of the object.
    std::string _Name;

    /// Clean non-unique namel
    std::string _CleanName;

    /// Reference namespace path.
    std::string _Path;

    /// Nesting level relevant to the parent.
    unsigned _NestLevel = 0;

    /// The script object type.
    ScriptObjectType _Type = ScriptObjectType::Empty;

    //! Initializes the object.
    //! \param name
    //! \param path
    virtual void _Initialize(const char* name, const char* path, ScriptObject* parent = nullptr);

    //! Handles cloning the object and adding it to the VM.
    //! \param name
    //! \param parent
    virtual ScriptObject* _OnCloneCreateObject(const std::string& name, ScriptObject* parent);

    //! Sets the path for the script object.
    //! \param Path
    void _SetPath(const char* path);

    //! Sets the script object type.
    //! \param type
    void _SetType(ScriptObjectType type);

    //! Sets the nesting level.
    //! \param level
    inline void _SetNestLevel(unsigned level) {
        _NestLevel = level;
    }

    //! Prints the objects in a child.
    //! \param childObj
    //! \param buffer
    void _PrintChild(ScriptObject* childObj, StringBuffer*& buffer);

public:
    //! Constructor.
    ScriptObject();

    //! Called in place of the constructor.
    void __Construct() override;

    //! Evaluates and returns the results of the object.
    //! \param args
    //! \return
    virtual Variant Evaluate(ScriptMethodArgs args);

    //! Evaluates and returns the results of the object.
    //! \return
    virtual Variant Evaluate();

    //! Adds a child to the script object.
    //! \param obj
    void AddChild(ScriptObject* scriptObject);

    //! Creates a clone of the object.
    //! \return SharedPtr<ScriptObject>
    ScriptObject* Clone();

    //! Creates a clone of the object.
    //! \param parent
    //! \param uniqueName
    //! \return SharedPtr<ScriptObject>
    virtual ScriptObject* Clone(ScriptObject* parent, bool uniqueName);

    //! Creates a context from a given context.
    //! \param context
    void CreateContext(ScriptContext* context);

    //! Finds a child based on the given name.
    //! \param name
    //! \param useCleanName
    ScriptObject* FindChildByName(const char* name, bool useCleanName = true);

    //! Traverses through parents in an attempt to find an object.
    //! \param name
    //! \param useCleanName
    ScriptObject* TraverseUpFindChildByName(const char* name, bool useCleanName = true);

    //! Attempts to find the method that called this object.
    //! \return ScriptObject
    ScriptObject* FindCalledByMethod();

    //! Remove a child object.
    //! \param name.
    void RemoveChild(const char* name);

    //! Releases the object.
    bool Release() override;

    //! Sets the parent for the object.
    //! \param parent
    inline void SetParent(ScriptObject* parent) {
        _Parent = parent;
    }

    //! Returns the parent object.
    //! \return ScriptObject
    WeakPtr<ScriptObject> GetParent() const;

    //! Returns the child objects.
    //! \return std::unordered_map<string, ScriptObject*>
    inline const std::vector<SharedPtr<ScriptObject>>& GetChildren() const {
        return _Children;
    }

    //! Returns a string of the script object.
    //! \return
    std::string Print();

    //! Sets the value of the object.
    //! \param value
    inline void SetValue(const Variant& value) {
        _Value = value;
    }

    //! Script value variant.
    //! \return Variant
    inline Variant GetValue() const {
        return _Value;
    }

    //! Sets the context.
    //! \param context
    inline void SetContext(ScriptContext* context) {
        _Context = context;
    }

    //! Returns the context for the object.
    //! \return ScriptContext
    inline ScriptContext* GetContext() const {
        return _Context.Ptr();
    }

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

    //! Returns the path of the object.
    //! \return
    inline const std::string& GetPath() const { return _Path; }

    //! Returns the type of the object.
    //! \return
    inline ScriptObjectType  GetType() const { return _Type; }

    //! Returns the type as a string.
    //! \return string
    std::string GetTypeAsString() const;

    //! Returns the nest level.
    //! \return
    inline unsigned GetNestLevel() const { return _NestLevel; }

    //! Whether or not this is an empty object.
    //! \return
    inline bool IsEmpty() const { return _Type == ScriptObjectType::Empty; }

    /// Empty script object.
    static ScriptObject Empty;

    /// Ensure the VM can access the private methods.
    friend class ScriptVM;
};

sympl_nsend
