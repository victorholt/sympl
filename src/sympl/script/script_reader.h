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
#include <sympl/core/object_ref.h>
#include <sympl/script/script_token.h>

sympl_nsstart

class StringBuffer;

class SYMPL_API ScriptReader : public ObjectRef
{
    SYMPL_OBJECT(ScriptReader, ObjectRef);

private:
    /// Buffer holding the script data.
    StringBuffer* _Buffer;

    /// File path of the script.
    std::string _FilePath;

    /// Script string given.
    std::string _ScriptString;

    /// Token helper for parsing.
    ScriptToken* _ScriptToken;

    /// Adds the line numbers to the compiler's output (for debugging).
    bool _AddLineNumbers = true;

    //! Processes the script file/string.
    //! \param fileStream
    //! \param bufferLength
    void ProcessScript(std::istream& fileStream, size_t bufferLength);

public:
    //! Constructor.
    ScriptReader();

    //! Destructor.
    virtual ~ScriptReader();

    //! Reads a script from a given file path.
    //! \param filePath
    //! \return bool
    bool ReadFile(const char* filePath);

    //! Reads a string.
    //! \param scriptString
    //! \return
    bool ReadString(const char* scriptString);

    //! Saves the output to a given file path.
    //! \param filePath
    void SaveOutputTo(const char* filePath);

    //! Returns the script's string buffer.
    //! \return StringBuffer*
    StringBuffer* GetBuffer() const;

    //! Sets whether or not to add line numbers to the string buffer.
    //! \param addLineNumbers
    inline void SetAddLineNumbers(bool addLineNumbers) { _AddLineNumbers = addLineNumbers; }
};

sympl_nsend
