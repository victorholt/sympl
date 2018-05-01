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

#include "../core/sympl_pch.h"
#include "../core/sympl_object.h"
#include "../core/variant.h"
#include "script_common.h"

sympl_nsstart

class Variant;
class StringBuffer;

class SYMPL_API ScriptObject : public Object
{
protected:
    /// Parent reference for the object.
    ScriptObject* _Parent = nullptr;

    /// Empty script object.
    static ScriptObject Empty;

    /// Value of the script object.
    Variant _Value;

    /// Children added to the object in the order they
    /// were added.
    std::vector<ScriptObject*> _Children;

    /// Name of the object.
    std::string _Name;

    /// Reference namespace path.
    std::string _Path;

    /// Nesting level relevant to the parent.
    unsigned _NestLevel = 0;

    /// The script object type.
    ScriptObjectType _Type = ScriptObjectType::Empty;

    //! Initializes the object.
    //! \param name
    //! \param path
    void _Initialize(const char* name, const char* path, ScriptObject* parent = nullptr);

    //! Adds a child to the script object.
    //! \param obj
    void _AddChild(ScriptObject* scriptObject);

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

    //! Destructor.
    virtual ~ScriptObject();

    //! Executes and returns the results of the object.
    //! \param result
    //! \return
    bool Execute(Variant*& result);

    //! Returns the parent object.
    //! \return ScriptObject
    ScriptObject& GetParent() const;

    //! Script value variant.
    //! \return
    const Variant& GetValue() const;

    //! Returns the child objects.
    //! \return std::vector<ScriptObject*>
    const std::vector<ScriptObject*>& GetChildren() const;

    //! Attempts to free child objects attached.
    bool Free() override ;

    //! Returns a string of the script object.
    //! \return
    std::string Print();

    //! Returns the name of the object.
    //! \return
    inline const std::string& GetName() const { return _Name; }

    //! Returns the path of the object.
    //! \return
    inline const std::string& GetPath() const { return _Path; }

    //! Returns the type of the object.
    //! \return
    inline ScriptObjectType  GetType() const { return _Type; }

    //! Returns the nest level.
    //! \return
    inline unsigned GetNestLevel() const { return _NestLevel; }

    //! Whether or not this is an empty object.
    //! \return
    inline bool IsEmpty() const { return _Type == ScriptObjectType::Empty; }

    /// Ensure the VM can access the private methods.
    friend class SymplVM;
};

sympl_nsend